#include "flextool/cling_environment.hpp" // IWYU pragma: associated

#include <entt/signal/dispatcher.hpp>

#include "flextool/app_cmd_options.hpp"
#include "flextool/command_line_constants.hpp"
#include "flextool/version.hpp"

#include <basis/PluginManager.hpp>
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

ScopedClingEnvironment::ScopedClingEnvironment()
{
  DETACH_FROM_SEQUENCE(sequence_checker_);

  DCHECK(base::MessageLoop::current()->task_runner());
}

ScopedClingEnvironment::~ScopedClingEnvironment()
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
}

bool ScopedClingEnvironment::init(
  const std::vector<base::FilePath>& scriptFiles
  , const std::vector<std::string>& cling_extra_args)
{
#if defined(CLING_IS_ON)
  // generate cling arguments based on command-line arguments
  {
    const bool ok
      = clang_util::add_default_cling_args(clingInterpreterArgs);
    if(!ok) {
      LOG(ERROR)
        << "unable to populate Cling arguments";
      return
        false;
    }
    DCHECK(!clingInterpreterArgs.empty());

    // pipeline clang args to cling
    {
      DCHECK(!cling_extra_args.empty());
      for(const std::string& arg: cling_extra_args) {

        // pipeline only args starting with "-"
        if(!base::StartsWith(
             base::StringPiece{arg}
             , base::StringPiece{clang_util::kClingArgPrefix}
             , base::CompareCase::INSENSITIVE_ASCII))
        {
          LOG(ERROR)
            << "unable to pipeline cling argument to cling: "
            << arg
            << " cause it does not have prefix: "
            << clang_util::kClingArgPrefix;
          NOTREACHED();
          continue;
        }

        clingInterpreterArgs.push_back(arg);
      }
    }

    LOG(INFO)
        << "populated Cling arguments: "
        << clingInterpreterArgs;
  }

  {
    VLOG(9)
      << "creating"
      " clingInterpreter";
    DCHECK(!clingInterpreterArgs.empty());
    DCHECK(!clingIncludePaths.empty());
    clingInterpreter
      = std::make_unique<
      cling_utils::ClingInterpreter>(
          "MainClingInterpreter_debug_id"
          , clingInterpreterArgs
          , clingIncludePaths
          );
  }

  // load files into C++ interpreter
  for(const base::FilePath& full_path
      : scriptFiles)
  {
    DCHECK(clingInterpreter);
    DCHECK(!full_path.empty());
    VLOG(9)
      << "calling"
      " clingInterpreter->loadFile";
    clingInterpreter->loadFile(full_path.value());
    LOG(INFO)
        << "loaded into Cling interpreter file "
        << full_path;
  }
#endif // defined(CLING_IS_ON)

  return
    true;
}

} // flextool
