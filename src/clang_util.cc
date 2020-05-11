#include "flextool/clang_util.hpp" // IWYU pragma: associated

#include <base/files/file_util.h>
#include <base/path_service.h>
#include <base/sequenced_task_runner.h>
#include <base/trace_event/trace_event.h>

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

namespace {

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}

const char kExtraClangFlag[] = "-extra-arg";

const char kExtraIncludeFlag[] = "-extra-arg=-I";

const char kClingIncludeFlag[] = "-I";

const char kClangBuildPathFlag[] = "-p=";

static bool add_default_clang_args(
    const base::FilePath clangBuildPath
    , std::vector<std::string>& args)
{

  base::FilePath dir_exe;
  if (!base::PathService::Get(base::DIR_EXE, &dir_exe)) {
    NOTREACHED();
    return false;
  }

  base::FilePath app_path;
  if (!base::PathService::Get(base::FILE_EXE, &app_path)) {
    NOTREACHED();
    return false;
  }

  {
    /// \note must be first arg
    CHECK(args.empty());
    args.push_back("clang_app"); // app name
  }

  if(!clangBuildPath.empty()) {
    if(base::PathExists(clangBuildPath)) {
      args.push_back(kClangBuildPathFlag
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

  //args.push_back("-extra-arg=-nostdinc");

  args.push_back(
    std::string(kExtraClangFlag) + "=-Wno-unused-command-line-argument");

  args.push_back(
    std::string(kExtraClangFlag) + "=-DCLANG_ENABLED=1");

  args.push_back(
    std::string(kExtraClangFlag) + "=-DCLANG_IS_ON=1");

  // see https://stackoverflow.com/a/30877725
  {
    args.push_back(
      std::string(kExtraClangFlag) + "=-DBOOST_SYSTEM_NO_DEPRECATED");
    args.push_back(
      std::string(kExtraClangFlag) + "=-DBOOST_ERROR_CODE_HEADER_ONLY");
  }

  // include paths
  {
    args.push_back(kExtraIncludeFlag
      + dir_exe.value());

    args.push_back(kExtraIncludeFlag
      + dir_exe.Append("./resources").value());
  }

  args.push_back(
    std::string(kExtraClangFlag) + "=-std=c++17");

  return true;
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

  if(base::StartsWith(
       base::StringPiece{arg}
       , base::StringPiece{"help"}
       , base::CompareCase::INSENSITIVE_ASCII))
  {
    return true;
  }

  if(base::StartsWith(
       base::StringPiece{arg}
       , base::StringPiece{"help-list"}
       , base::CompareCase::INSENSITIVE_ASCII))
  {
    return true;
  }

  if(base::StartsWith(
       base::StringPiece{arg}
       , base::StringPiece{"version"}
       , base::CompareCase::INSENSITIVE_ASCII))
  {
    return true;
  }

  if(base::StartsWith(
       base::StringPiece{arg}
       , base::StringPiece{"extra-arg="}
       , base::CompareCase::INSENSITIVE_ASCII))
  {
    return true;
  }

  if(base::StartsWith(
       base::StringPiece{arg}
       , base::StringPiece{"extra-arg-before="}
       , base::CompareCase::INSENSITIVE_ASCII))
  {
    return true;
  }

  if(base::StartsWith(
       base::StringPiece{arg}
       , base::StringPiece{"p="}
       , base::CompareCase::INSENSITIVE_ASCII))
  {
    return true;
  }

  // clang can accept file paths
  if(base::PathExists(base::FilePath(arg)))
  {
    return true;
  }

  return false;
}

bool add_default_cling_args(
  std::vector<std::string>& clingInterpreterArgs)
{
  base::FilePath dir_exe;
  if (!base::PathService::Get(base::DIR_EXE, &dir_exe)) {
    NOTREACHED();
    return false;
  }

  {
    /// \note must be first arg
    CHECK(clingInterpreterArgs.empty());
    clingInterpreterArgs.push_back("cling_app"); // app name
  }

  //clingInterpreterArgs.push_back("-nostdinc");

  clingInterpreterArgs.push_back("-Wno-unused-command-line-argument");

  clingInterpreterArgs.push_back("-DCLING_ENABLED=1");
  clingInterpreterArgs.push_back("-DCLING_IS_ON=1");

  // https://stackoverflow.com/a/30877725
  clingInterpreterArgs.push_back("-DBOOST_SYSTEM_NO_DEPRECATED");
  clingInterpreterArgs.push_back("-DBOOST_ERROR_CODE_HEADER_ONLY");

  clingInterpreterArgs.push_back("--std=c++17");

  // include paths
  {
    clingInterpreterArgs.push_back(kClingIncludeFlag
      + dir_exe.value());

    clingInterpreterArgs.push_back(kClingIncludeFlag
      + dir_exe.Append(
          "./resources").value());
  }

  return true;
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
    return base::nullopt;
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
    return base::nullopt;
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

  return combined_for_cling;
}

bool populateClangArguments(
  const base::FilePath& clangBuildPath
  , std::vector<std::string>& args_storage
  , std::vector<std::string>& cling_extra_args
  , cmd::BoostCmd& boostCmd
){
  {
    /// \note must be before all custom arguments
    const bool ok = add_default_clang_args(
      clangBuildPath
      , args_storage);
    DCHECK(!args_storage.empty());
    if(!ok) {
      return false;
    }
  }

  if(boostCmd.unregisteredOptions().empty()) {
    VLOG(9)
      << "no unregistered arguments provided for clang and cling";
  }

  /// \note forward unregistered (unknown) options to clang libtooling
  for (const cmd::UnergisteredOption& unergisteredOption
       : boostCmd.unregisteredOptions())
  {
    //DCHECK(unergisteredOption.key);
    //DCHECK(unergisteredOption.values);
    VLOG(9)
      << "forwarding unregistered (unknown)"
         " command-line argument: "
      << /***/unergisteredOption.key
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

    for(size_t i = 0; i < (/***/unergisteredOption.values).size(); i++)
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
            i
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

        args_storage.push_back(std::move(withPrefix));
      }

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
          continue;
        }
#if defined(CLING_IS_ON)
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
            continue;
          }

          /// \note we do not add '-' before argument
          /// if it does not have key
          /// example: path to source file that tool needs to process
          /// does not have key
          const std::string withPrefix
            = unergisteredOption.key.empty()
              ? combined_for_cling.value()
              : (kClingArgPrefix + combined_for_cling.value());

          cling_extra_args.push_back(
            std::move(withPrefix));
        }
#endif // CLING_IS_ON
      } else {
        VLOG(9)
          << "failed to pipeline clang argument to cling: "
          << combined_for_clang;
      }
    } // for
  } // for

  return true;
}

void FileSaveHandler::saveFile(
  const base::FilePath outDir
  , bool shouldFlushFile
  , bool shouldPrintConsole
  , const clang::FileID &fileID
  , const clang::FileEntry *fileEntry
  , clang::Rewriter &rewriter
){
  std::string full_file_path = fileEntry->getName();
  VLOG(9)
      << "full_file_path is "
      << full_file_path;
  if(full_file_path.empty()) {
    LOG(WARNING)
        << "unable to save file, invalid file path";
    return;
  }

  const std::string filename = fs::path(full_file_path).filename();
  VLOG(9)
      << "filename is "
      << filename;
  if(filename.empty()) {
    LOG(WARNING)
        << "unable to save file, invalid file name";
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

  const char kGeneratedFileExtentionSuffix[]
      = ".generated";

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
          << "Could not save file "
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
  if (shouldPrintConsole) {
    clang::SourceManager &sm = rewriter.getSourceMgr();
    rewriter.getEditBuffer(
          sm.getMainFileID()).write(llvm::outs());
  }
}

} // namespace clang_util
