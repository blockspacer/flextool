#pragma once

#include "boost_command_line.hpp"

#include <base/files/file_path.h>
#include <base/macros.h>

#include <clang/AST/ASTContext.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchersMacros.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Frontend/ASTConsumers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

#include <string>
#include <vector>

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
