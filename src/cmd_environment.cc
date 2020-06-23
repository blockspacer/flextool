#include "flextool/cmd_environment.hpp" // IWYU pragma: associated

#include <entt/signal/dispatcher.hpp>

#include "flextool/app_cmd_options.hpp"
#include "flextool/clang_util.hpp"
#include "flextool/command_line_constants.hpp"
#include "flextool/version.hpp"

#include <basis/PluginManager.hpp>
#include <basis/boost_command_line.hpp>
#include <basis/cmd_util.hpp>
#include <basis/i18n.hpp>
#include <basis/icu_util.hpp>
#include <basis/log_util.hpp>
#include <basis/thread_pool_util.hpp>
#include <basis/tracing_util.hpp>

#include <base/process/memory.h>
#include <base/base_paths.h>
#include <base/bind.h>
#include <base/callback_forward.h>
#include <base/debug/stack_trace.h>
#include <base/feature_list.h>
#include <base/files/file.h>
#include <base/files/file_util.h>
#include <base/logging.h>
#include <base/memory/ref_counted_memory.h>
#include <base/memory/scoped_refptr.h>
#include <base/message_loop/message_loop.h>
#include <base/message_loop/message_loop_current.h>
#include <base/metrics/statistics_recorder.h>
#include <base/path_service.h>
#include <base/run_loop.h>
#include <base/sampling_heap_profiler/sampling_heap_profiler.h>
#include <base/single_thread_task_runner.h>
#include <base/strings/string_piece.h>
#include <base/strings/string_piece_forward.h>
#include <base/strings/string_util.h>
#include <base/system/sys_info.h>
#include <base/task/thread_pool/thread_pool.h>
#include <base/threading/platform_thread.h>
#include <base/trace_event/memory_dump_manager.h>
#include <base/trace_event/trace_buffer.h>
#include <base/trace_event/trace_event.h>
#include <base/trace_event/trace_log.h>

#include <algorithm>
#include <functional>
#include <iterator>
#include <locale>
#include <sstream>
#include <string.h>

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

namespace flextool {

ScopedCmdEnvironment::ScopedCmdEnvironment()
  : appCmd(boostCmdParser)
{
  DETACH_FROM_SEQUENCE(sequence_checker_);
}

ScopedCmdEnvironment::~ScopedCmdEnvironment()
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
}

/// \todo refactor long method
bool ScopedCmdEnvironment::init(
  int argc, char* argv[])
{
  DCHECK(argc > 0);

  if (!base::PathService::Get(base::DIR_EXE, &dir_exe_)) {
    NOTREACHED();
    // stop app execution with EXIT_FAILURE
    return
      false;
  }

  /// \note log all command-line arguments before
  /// parsing them as program options
  {
    VLOG(9)
        << "started "
        << dir_exe_
        << " with arguments:";
    for(int i = 0; i < argc; ++i) {
      VLOG(9)
        << " "
        << argv[i]
        << " ";
    }
  }

  // set description for command-line arguments
  {
    using boost::program_options::options_description_easy_init;

    options_description_easy_init& options
      = boostCmdParser.options();
    options = appCmd.registerOptions(options);
  }

  // set parser for command-line arguments
  {
    bool initOk = boostCmdParser.init(argc, argv);
    if(!initOk)
    {
      LOG(ERROR)
        << "Unable to parse command-line arguments";
      DCHECK(false);
      // stop app execution with EXIT_FAILURE
      return
        false;
    }
  }

  return
    true;
}

} // flextool
