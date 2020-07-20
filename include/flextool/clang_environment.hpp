#pragma once

#include "clang_util.hpp"

#include <base/macros.h>
#include <base/memory/scoped_refptr.h>
#include <base/sequence_checker.h>

#include <flexlib/annotation_parser.hpp>

#include <string>
#include <vector>

namespace clang_utils { class AnnotationMatchOptions; }

namespace clang_utils { class SourceTransformPipeline; }

namespace flexlib { class AnnotationMatchHandler; }

namespace flextool { class ScopedCmdEnvironment; }

namespace flextool {

/// \note must store data related to clang libtooling
class ScopedClangEnvironment {
public:
  ScopedClangEnvironment();

  ~ScopedClangEnvironment();

  // init with provided settings
  [[nodiscard]] /* do not ignore return value */
  bool init(
    flextool::ScopedCmdEnvironment& cmd_env);

public:
  std::vector<std::string> args_storage;

  std::vector<std::string> cling_extra_args;

  std::unique_ptr<::flexlib::AnnotationMethods> annotationMethods;

  std::unique_ptr<::clang_utils::SourceTransformPipeline>
  sourceTransformPipeline;

  std::unique_ptr<::flexlib::AnnotationParser> annotationParser;

  std::unique_ptr<::flexlib::AnnotationMatchHandler>
  anotationMatchHandler;

  scoped_refptr<clang_utils::AnnotationMatchOptions>
  annotationMatchOptions;

  clang_util::FileSaveHandler fileSaveHandler{};

private:
  SEQUENCE_CHECKER(sequence_checker_);

  DISALLOW_COPY_AND_ASSIGN(ScopedClangEnvironment);
};

} // flextool
