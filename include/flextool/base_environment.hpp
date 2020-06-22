#pragma once

#include "PluginManager.hpp"
#include "app_cmd_options.hpp"
#include "boost_command_line.hpp"
#include "clang_util.hpp"

#include <base/at_exit.h>
#include <base/files/file_path.h>
#include <base/macros.h>
#include <base/memory/scoped_refptr.h>
#include <base/message_loop/message_loop.h>

#include <basis/scoped_log_run_time.hpp>

#include <string>
#include <vector>

namespace base { class SingleThreadTaskRunner; }

namespace entt { class dispatcher; }

namespace i18n { class I18n; }

namespace flextool {

/// \note must store data related to base and basis libs
/// inits basic requirements, like thread pool, logging, etc.
class ScopedBaseEnvironment {
public:
  ScopedBaseEnvironment();

  ~ScopedBaseEnvironment();

  // init with provided settings
  bool init(
    int argc
    , char* argv[]
    , const bool auto_start_tracer
    , const std::string event_categories
    , const base::FilePath& outDir
    , const int threadsNum);

public:
  base::FilePath dir_exe_{};

  basis::ScopedLogRunTime scopedLogRunTime{};

  // This object instance is required (for example,
  // LazyInstance, MessageLoop).
  base::AtExitManager at_exit{};

  /// \note creates base::MessageLoop::current()
  base::MessageLoopForIO main_thread_message_loop{};

  // allows to schedule arbitrary tasks on main loop
  scoped_refptr<base::SingleThreadTaskRunner> main_loop_task_runner;

  std::unique_ptr<i18n::I18n> i18n;

private:
  DISALLOW_COPY_AND_ASSIGN(ScopedBaseEnvironment);
};

} // flextool
