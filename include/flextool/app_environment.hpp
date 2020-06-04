#pragma once

#include "boost_command_line.hpp"
#include "app_cmd_options.hpp"
#include "PluginManager.hpp"
#include "clang_util.hpp"

#include <base/macros.h>
#include <base/files/file_path.h>
#include <base/at_exit.h>
#include <base/message_loop/message_loop.h>
#include <basis/i18n.hpp>

#include <basis/scoped_log_run_time.hpp>

#include "flexlib/ToolPlugin.hpp"
#include "flexlib/parser_constants.hpp"
#include "flexlib/utils.hpp"
#include "flexlib/funcParser.hpp"
#include "flexlib/clangUtils.hpp"
#include "flexlib/clangPipeline.hpp"
#include "flexlib/annotation_parser.hpp"
#include "flexlib/annotation_match_handler.hpp"
#include "flexlib/matchers/annotation_matcher.hpp"
#include "flexlib/options/ctp/options.hpp"
#if defined(CLING_IS_ON)
#include "flexlib/ClingInterpreterModule.hpp"
#endif // CLING_IS_ON

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <entt/entt.hpp>

#include <vector>
#include <string>

namespace flextool {

class ScopedAppEnvironment {
public:
  ScopedAppEnvironment();

  ~ScopedAppEnvironment();

  // init with provided settings
  bool init(int argc, char* argv[]);

public:
  cmd::BoostCmd boostCmd{};

  cmd::AppCmdOptions appCmd;

  base::FilePath dir_exe_{};

  basis::ScopedLogRunTime scopedLogRunTime{};

  backend::PluginManager plug_mgr{};

  /// \note dispatcher must be thread-safe,
  /// so use entt after patch https://github.com/skypjack/entt/issues/449
  /// see https://github.com/skypjack/entt/commit/74f3df83dbc9fc4b43b8cfb9d71ba02234bd5c4a
  std::unique_ptr<entt::dispatcher> main_events_dispatcher;

  std::vector<std::string> args_storage;

  std::vector<std::string> cling_extra_args;

  std::unique_ptr<::flexlib::AnnotationMethods> annotationMethods;

  std::unique_ptr<::clang_utils::SourceTransformPipeline> sourceTransformPipeline;

  std::unique_ptr<::flexlib::AnnotationParser> annotationParser;

  clang_util::FileSaveHandler fileSaveHandler{};

  std::unique_ptr<::flexlib::AnnotationMatchHandler> anotationMatchHandler;

  scoped_refptr<clang_utils::AnnotationMatchOptions>
    annotationMatchOptions;

  std::unique_ptr<cling_utils::ClingInterpreter> clingInterpreter;

  // This object instance is required (for example,
  // LazyInstance, MessageLoop).
  base::AtExitManager at_exit{};

  /// \note creates base::MessageLoop::current()
  base::MessageLoopForIO main_thread_message_loop{};

  // allows to schedule arbitrary tasks on main loop
  scoped_refptr<base::SingleThreadTaskRunner> main_loop_task_runner;

  std::unique_ptr<i18n::I18n> i18n;

private:
  DISALLOW_COPY_AND_ASSIGN(ScopedAppEnvironment);
};

} // flextool
