#include "flextool/cmd_environment.hpp" // IWYU pragma: associated

#include <boost/program_options/options_description.hpp>
#include <entt/signal/dispatcher.hpp>

#include "flextool/PluginManager.hpp"
#include "flextool/app_cmd_options.hpp"
#include "flextool/boost_command_line.hpp"
#include "flextool/clang_util.hpp"
#include "flextool/command_line_constants.hpp"
#include "flextool/path_provider.hpp"
#include "flextool/version.hpp"

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

namespace {

static const base::FilePath::CharType kIcuDataFileName[]
  = FILE_PATH_LITERAL("./resources/icu/optimal/icudt64l.dat");

static const base::FilePath::CharType kTraceReportFileName[]
  = FILE_PATH_LITERAL("trace_report.json");

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

} // namespace

namespace flextool {

ScopedCmdEnvironment::ScopedCmdEnvironment()
  : appCmd(boostCmd)
{
}

ScopedCmdEnvironment::~ScopedCmdEnvironment()
{
}

/// \todo refactor long method
bool ScopedCmdEnvironment::init(int argc, char* argv[])
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

  // set application command-line arguments
  {
    boost::program_options::options_description_easy_init&
    options = boostCmd.options();
    options = appCmd.registerOptions(options);
  }

  {
    bool initOk = boostCmd.init(argc, argv);
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
