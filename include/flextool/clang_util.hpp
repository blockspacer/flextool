#pragma once

#include <base/macros.h>

#include <string>
#include <vector>

#include <base/optional.h>
#include <base/strings/string_piece_forward.h>

namespace base {
class FilePath;
} // namespace base

namespace clang {
class FileEntry;
class FileID;
class Rewriter;
} // namespace clang

namespace cmd {
class BoostCmd;
} // namespace cmd

namespace clang_util {

/// \note must be "extra-arg=", not "--extra-arg="
/// after parsing of cmd args
extern const char kExtraArgPrefix[];

// all args passed to Cling must start with '-'
extern const char kClingArgPrefix[];

// parsed args must not start with "-"
extern const char kInvalidParsedArgPrefix[];

// we must pass arguments to Clang with "-" prefix
extern const char kClangArgPrefix[];

base::Optional<std::string> clangArgToClingArg(
  /// \note base::StringPiece points to a sized piece of memory
  /// you must manage StringPiece lifetime!
  const base::StringPiece& arg);

bool populateClangArguments(
  const base::FilePath& clangBuildPath
  , std::vector<std::string>& args_storage
  , std::vector<std::string>& cling_extra_args
  , cmd::BoostCmd& boostCmd
  );

bool add_default_cling_args(
  std::vector<std::string>& clingInterpreterArgs);

class FileSaveHandler {
public:
  FileSaveHandler() = default;

  void saveFile(
    const base::FilePath outDir
    , bool shouldFlushFile
    , bool shouldPrintConsole
    , const clang::FileID& fileID
    , const clang::FileEntry* fileEntry
    , clang::Rewriter& rewriter
    );

private:
  DISALLOW_COPY_AND_ASSIGN(FileSaveHandler);
};
} // namespace clang_util
