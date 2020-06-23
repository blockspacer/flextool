#pragma once

#include "app_cmd_options.hpp"
#include "cmd_environment.hpp"
#include "clang_util.hpp"

#include <base/at_exit.h>
#include <base/files/file_path.h>
#include <base/macros.h>
#include <base/memory/scoped_refptr.h>
#include <base/message_loop/message_loop.h>
#include <base/sequence_checker.h>

#include <basis/scoped_log_run_time.hpp>
#include <basis/PluginManager.hpp>

#include <flexlib/annotation_match_handler.hpp>
#include <flexlib/annotation_parser.hpp>
#include <flexlib/clangPipeline.hpp>
#include <flexlib/matchers/annotation_matcher.hpp>
#include <flexlib/parser_constants.hpp>

#include <string>
#include <vector>

namespace flexlib { class AnnotationMatchHandler; }

namespace clang_utils { class AnnotationMatchOptions; }

namespace clang_utils { class SourceTransformPipeline; }

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
