#include "flextool/clang_util.hpp" // IWYU pragma: associated

#include <base/base_paths.h>
#include <base/files/file_path.h>
#include <base/files/file_util.h>
#include <base/logging.h>
#include <base/path_service.h>
#include <base/stl_util.h>
#include <base/strings/string_piece.h>
#include <base/strings/string_util.h>
#include <base/optional.h>
#include <base/strings/string_piece_forward.h>

#include <basis/boost_command_line.hpp>

#include <clang/Basic/FileManager.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Rewrite/Core/RewriteBuffer.h>
#include <clang/Rewrite/Core/Rewriter.h>

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

#include <stddef.h>
#include <algorithm>
#include <iterator>
#include <ostream>
#include <system_error>
#include <type_traits>
#include <chrono>

// __has_include is currently supported by GCC and Clang. However GCC 4.9 may have issues and
// returns 1 for 'defined( __has_include )', while '__has_include' is actually not supported:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63662
#if __has_include(<filesystem>)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif // __has_include

#if __has_include(<filesystem>)
namespace fs = std::filesystem;
#else
namespace fs = std::experimental::filesystem;
#endif // __has_include

namespace clang {
class FileID;
} // namespace clang

namespace {

/// \todo code repeat
template<class T>
std::ostream& operator<<(
  std::ostream& stream, const std::vector<T>& data)
{
  std::copy(data.begin(), data.end(),
            std::ostream_iterator<T>(stream, " "));
  return
    stream;
}

static const char kExtraClangFlag[] = "-extra-arg";

static const char kExtraIncludeFlag[] = "-extra-arg=-I";

static const char kClingIncludeFlag[] = "-I";

static const char kClangBuildPathFlag[] = "-p=";

static void addBoostDefines(
  std::vector<std::string>& args
  , const std::string& prefix)
{
  // see https://stackoverflow.com/a/30877725
  args.push_back(
    prefix + "-DBOOST_SYSTEM_NO_DEPRECATED");
  args.push_back(
    prefix + "-DBOOST_ERROR_CODE_HEADER_ONLY");
}

static void addWarningFlags(
  std::vector<std::string>& args
  , const std::string& prefix)
{
  args.push_back(
    prefix + "-Wno-unused-command-line-argument");
}

[[nodiscard]] /* do not ignore return value */
static bool add_default_clang_args(
  const base::FilePath clangBuildPath
  , std::vector<std::string>& args)
{

  base::FilePath dir_exe;
  if (!base::PathService::Get(base::DIR_EXE, &dir_exe)) {
    NOTREACHED();
    return
      false;
  }

  base::FilePath app_path;
  if (!base::PathService::Get(base::FILE_EXE, &app_path)) {
    NOTREACHED();
    return
      false;
  }

  {
    /// \note must be first arg
    CHECK(args.empty())
      << "clang arguments already populated";
    args.push_back(
      "clang_app"); // app name
  }

  if(!clangBuildPath.empty()) {
    if(base::PathExists(clangBuildPath)) {
      args.push_back(
        kClangBuildPathFlag
        + clangBuildPath.value());
      LOG(INFO)
        << "files will be written into directory: "
        << clangBuildPath;
    } else {
      LOG(WARNING)
        << "Unable to add clang build path: "
        << clangBuildPath;
    }
  }

  const std::string prefixForClangFlag
    = std::string(kExtraClangFlag) + "=";

  addWarningFlags(args, prefixForClangFlag);

  args.push_back(
    prefixForClangFlag + "-DCLANG_ENABLED=1");

  args.push_back(
    prefixForClangFlag + "-DCLANG_IS_ON=1");

  addBoostDefines(args, prefixForClangFlag);

  // include paths
  {
    args.push_back(
      kExtraIncludeFlag
      + dir_exe.value());

    args.push_back(
      kExtraIncludeFlag
      /// \todo recieve from cmake as definition
      /// \todo docs: why we need it here?
      + dir_exe.Append("./resources").value());
  }

  args.push_back(
    prefixForClangFlag + "-std=c++17");

  return
    true;
}

} // namespace

namespace clang_util {

// extern
const char kExtraArgPrefix[] = "extra-arg=";

// extern
const char kInvalidParsedArgPrefix[] = "-";

// extern
const char kClangArgPrefix[] = "-";

// extern
const char kClingArgPrefix[] = "-";

static bool isAllowedClangArgument(const base::StringPiece& arg)
{
  CHECK(base::size(clang_util::kClangArgPrefix));

  // accept both `-extra-arg=` and `extra-arg=`
  base::StringPiece arg_without_prefix = arg;
  if(base::StartsWith(
       base::StringPiece{arg}
       , base::StringPiece{clang_util::kClangArgPrefix}
       , base::CompareCase::INSENSITIVE_ASCII))
  {
    arg_without_prefix.remove_prefix(
      base::size(clang_util::kClangArgPrefix));
  }

  // arguments that are built-in into clang libtooling
  /**
   * Clang provides some options out-of-the-box:
   *
   * Generic Options:
   *
   *   -help
   *     - Display available options (-help-hidden for more)
   *   -help-list
   *     - Display list of available options (-help-list-hidden for more)
   *   -version
   *     - Display the version of this program
   *
   * Use override options:
   *
   *   -extra-arg=<string>
   *     - Additional argument to append to the compiler command line
   *   -extra-arg-before=<string>
   *     - Additional argument to prepend to the compiler command line
   *   -p=<string>
   *     - Build path
   **/
  static const std::vector<std::string> allowedClangArguments{
    "help"
    , "help-list"
    , "version"
    , "extra-arg="
    , "extra-arg-before="
    , "p="
  };

  for(const auto& allowedArg: allowedClangArguments) {
    if(base::StartsWith(
         base::StringPiece{arg}
         , base::StringPiece{allowedArg}
         , base::CompareCase::INSENSITIVE_ASCII))
    {
      return
        true;
    }
  }

  // clang can accept file paths
  if(base::PathExists(base::FilePath(arg)))
  {
    return
      true;
  } else {
    LOG(WARNING)
        << "path does not exist: "
        << arg;
  }

  return
    false;
}

bool add_default_cling_args(
  std::vector<std::string>& clingInterpreterArgs)
{
  base::FilePath dir_exe;
  if (!base::PathService::Get(base::DIR_EXE, &dir_exe)) {
    NOTREACHED();
    return
      false;
  }

  {
    /// \note must be first arg
    CHECK(clingInterpreterArgs.empty())
      << "cling arguments already populated";
    clingInterpreterArgs.push_back(
      "cling_app"); // app name
  }

  addWarningFlags(clingInterpreterArgs,
                  "" // prefix
                  );

  clingInterpreterArgs.push_back(
    "-DCLING_ENABLED=1");
  clingInterpreterArgs.push_back(
    "-DCLING_IS_ON=1");

  addBoostDefines(clingInterpreterArgs,
                  "" // prefix
                  );

  clingInterpreterArgs.push_back(
    "--std=c++17");

  // include paths
  {
    clingInterpreterArgs.push_back(
      kClingIncludeFlag
      + dir_exe.value());

    clingInterpreterArgs.push_back(
      kClingIncludeFlag
      + dir_exe.Append(
        /// \todo recieve from cmake as definition
        /// \todo docs: why we need it here?
        "./resources").value());
  }

  return
    true;
}

base::Optional<std::string> clangArgToClingArg(
  const base::StringPiece& arg)
{
  // VALID: "extra-arg="
  // NOT VALID: "-extra-arg="
  if(base::StartsWith(
       base::StringPiece{arg}
       , base::StringPiece{kInvalidParsedArgPrefix}
       , base::CompareCase::INSENSITIVE_ASCII))
  {
    LOG(ERROR)
        << "Command-line argument can not start with "
        << kInvalidParsedArgPrefix
        << " Error in argument: "
        << arg;
    return
      base::nullopt;
  }

  // pipeline only args starting with "--extra-arg="
  if(!base::StartsWith(
       base::StringPiece{arg}
       , base::StringPiece{kExtraArgPrefix}
       , base::CompareCase::INSENSITIVE_ASCII))
  {
    VLOG(9)
        << "unable to pipeline clang argument to cling: "
        << arg
        << " cause it does not have prefix: "
        << kExtraArgPrefix;
    return
      base::nullopt;
  }

  base::StringPiece arg_without_prefix = arg;
  // remove prefix
  {
    CHECK(base::size(kExtraArgPrefix));
    arg_without_prefix.remove_prefix(
      base::size(kExtraArgPrefix));
  }

  std::string combined_for_cling;
  arg_without_prefix.CopyToString(&combined_for_cling);

  return
    base::Optional<std::string>{
      std::move(combined_for_cling)};
}

bool pipelineClangArgToCling(
  const std::string& combined_for_clang
  , const cmd::UnergisteredOption& unergisteredOption
  , std::vector<std::string>& cling_extra_args)
{
  /// \note only arguments starting with `--extra-arg=`
  /// will be forwarded
  base::Optional<std::string> combined_for_cling
    = clangArgToClingArg(combined_for_clang);
  if(combined_for_cling.has_value())
  {
    if(combined_for_cling.value().empty()) {
      CHECK(combined_for_clang == kExtraArgPrefix);
      LOG(WARNING)
        << "Empty argument:"
        << kExtraArgPrefix;
      return false;
    }
    // pipeline clang args to cling
    {
      VLOG(9)
        << "pipelined clang argument to cling: "
        << combined_for_cling.value();

      /// \note argument without '-' prefix
      /// example: extra-arg=-I/home/
      // VALID: "extra-arg="
      // NOT VALID: "-extra-arg="
      if(base::StartsWith(
           base::StringPiece{combined_for_cling.value()}
           , base::StringPiece{kClingArgPrefix}
           , base::CompareCase::INSENSITIVE_ASCII))
      {
        LOG(ERROR)
            << "Command-line argument can not start with "
            << kClingArgPrefix
            << " Error in argument: "
            << combined_for_cling.value();
        return false;
      }

      /// \note We do not add '-' before argument
      /// if it does not have key.
      /// Example: path to source file
      /// that tool needs to process
      /// does not have key
      const std::string withPrefix
        = unergisteredOption.key.empty()
          ? combined_for_cling.value()
          : (kClingArgPrefix + combined_for_cling.value());

      cling_extra_args.push_back(
        std::move(withPrefix));
    }
  } else {
    VLOG(9)
        << "failed to pipeline clang argument to cling: "
        << combined_for_clang;
    return false;
  }

  return true;
}

/// \todo refactor long method
bool populateClangArguments(
  const base::FilePath& clangBuildPath
  , std::vector<std::string>& args_storage
  , std::vector<std::string>& cling_extra_args
  , cmd::BoostCmdParser& boostCmdParser
  )
{
  {
    /// \note must be before all custom arguments
    const bool add_ok = add_default_clang_args(
      clangBuildPath
      , args_storage);
    DCHECK(!args_storage.empty());
    if(!add_ok) {
      return
        false;
    }
  }

  if(boostCmdParser.unregisteredOptions().empty()) {
    VLOG(9)
      << "no unregistered arguments provided for clang and cling";
  }

  /// \note forward unregistered (unknown) options
  /// to clang libtooling
  for (const cmd::UnergisteredOption& unergisteredOption
       : boostCmdParser.unregisteredOptions())
  {
    VLOG(9)
        << "forwarding unregistered (unknown)"
      " command-line argument: "
        << unergisteredOption.key
        << " with values: "
        << unergisteredOption.values;

    if(base::StartsWith(
         base::StringPiece{unergisteredOption.key}
         , base::StringPiece{kInvalidParsedArgPrefix}
         , base::CompareCase::INSENSITIVE_ASCII))
    {
      LOG(ERROR)
        << "Command-line argument can not start with "
        << kInvalidParsedArgPrefix
        << " Error in argument: "
        << unergisteredOption.key;
      continue;
    }

    /// \note single key may have multiple values
    /// Example with key "extra-arg":
    /// extra-arg=-I/home/ extra-arg=-I/other/
    for(size_t i = 0
        ; i < unergisteredOption.values.size();
        i++)
    {
      // separator for key and value concatenation
      /// \note we do not add '=' before argument value
      /// if it does not have key
      /// example: path to source file that tool needs to process
      /// does not have key
      const std::string KVseparator
        = unergisteredOption.key.empty()
          ? ""
          : "=";

      // example: key "extra-arg" and value "-I/home/"
      // becomes "extra-arg=-I/home/"
      const std::string combined_for_clang
        = unergisteredOption.KVToSting(
            i // value index
            , KVseparator
            );

      VLOG(9)
        << "pipelining unregistered option "
        "to clang libtooling: "
        << combined_for_clang;

      if(combined_for_clang.empty()) {
        NOTREACHED();
        continue;
      }

      if(!isAllowedClangArgument(combined_for_clang))
      {
        VLOG(9)
            << "unable to pipeline unknown"
          " to clang libtooling argument: "
            << combined_for_clang;
        continue;
      }

      {
        /// \note argument without '-' prefix
        /// example: extra-arg=-I/home/
        // VALID: "extra-arg="
        // NOT VALID: "-extra-arg="
        if(base::StartsWith(
             base::StringPiece{combined_for_clang}
             , base::StringPiece{kClangArgPrefix}
             , base::CompareCase::INSENSITIVE_ASCII))
        {
          LOG(ERROR)
            << "Command-line argument can not start with "
            << kClangArgPrefix
            << " Error in argument: "
            << combined_for_clang;
          continue;
        }

        /// \note we do not add '-' before argument
        /// if it does not have key
        /// example: path to source file that tool needs to process
        /// does not have key
        const std::string withPrefix
          = unergisteredOption.key.empty()
            ? combined_for_clang
            : (kClangArgPrefix + combined_for_clang);

        args_storage.push_back(
          std::move(withPrefix));
      }

#if defined(CLING_IS_ON)
      const bool pipelineDone
        = pipelineClangArgToCling(
            combined_for_clang
            , unergisteredOption
            , cling_extra_args);
      if(!pipelineDone) {
        VLOG(9)
            << "Command-line argument "
          "can not be piplined into cling."
            << " Error in argument: "
            << combined_for_clang;
        continue;
      }
#endif // CLING_IS_ON
    } // for
  } // for

  return
    true;
}

void FileSaveHandler::saveFile(
  const base::FilePath outDir
  // can be used to disable file saving
  , bool shouldFlushFile
  // can be used to print debug information
  , bool shouldPrintEditBufferToConsole
  , const clang::FileID &fileID
  , const clang::FileEntry *fileEntry
  , clang::Rewriter &rewriter
  )
{
  std::string full_file_path = fileEntry->getName();
  VLOG(9)
    << "full_file_path is "
    << full_file_path;
  if(full_file_path.empty()) {
    LOG(WARNING)
      << "unable to save file, invalid file path";
    return;
  }

  VLOG(9)
    << "** EndSourceFileAction for: "
    << fileEntry->getName().str();

  const std::string originalFileExtention
    = fs::path(full_file_path).extension();
  if(originalFileExtention.empty()) {
    LOG(WARNING)
      << "unable to save file, invalid file extension";
    return;
  }

  // extract filename from path
  const std::string filename
    = fs::path(full_file_path).filename();
  VLOG(9)
    << "filename is "
    << filename;
  if(filename.empty()) {
    LOG(WARNING)
      << "unable to save file, invalid file name";
    return;
  }

  /// \todo make configurable
  static const char kGeneratedFileExtentionSuffix[]
    = ".generated";

  // build path to generated file
  const base::FilePath out_path
    = outDir.Append(
        filename
        + kGeneratedFileExtentionSuffix
        + originalFileExtention);

  if (shouldFlushFile) {
    VLOG(9)
        << "writing data to file with path = "
        << out_path.value();

    std::error_code error_code;

    llvm::raw_fd_ostream outFile(
      out_path.value(),
      error_code,
      llvm::sys::fs::F_None);
    rewriter.getEditBuffer(fileID).write(outFile);
    outFile.close();
    if(error_code != std::error_code{}) {
      LOG(ERROR)
        << "Could not save file: "
        << out_path;
    } else {
      LOG(INFO)
        << "Saved file: "
        << out_path;
    }

    // we expect absolute path
    /// \note On POSIX, |MakeAbsoluteFilePath| fails
    /// if the path does not exist
    DCHECK(out_path == base::MakeAbsoluteFilePath(out_path))
        << "unable to find absolute path to "
        << out_path;
  }

  // this will output to screen
  if (shouldPrintEditBufferToConsole) {
    clang::SourceManager &sourceManager
      = rewriter.getSourceMgr();
    rewriter.getEditBuffer(
      sourceManager.getMainFileID())
      .write(llvm::outs());
  }
}

} // namespace clang_util
