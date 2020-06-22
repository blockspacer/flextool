#include "flextool/base_environment.hpp" // IWYU pragma: associated

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

} // namespace

namespace flextool {

ScopedBaseEnvironment::ScopedBaseEnvironment()
  : main_loop_task_runner(base::MessageLoop::current()->task_runner())
{
  DCHECK(base::MessageLoop::current()->task_runner());
}

ScopedBaseEnvironment::~ScopedBaseEnvironment()
{
  {
    DCHECK(base::trace_event::MemoryDumpManager::GetInstance());
    base::trace_event::MemoryDumpManager::GetInstance()
      ->TeardownForTracing();
  }

  // save tracing report to file, if needed
  {
    const bool need_write_tracing_report
      = base::trace_event::TraceLog::GetInstance()->IsEnabled();
    if(need_write_tracing_report) {
      basis::writeTraceReport(
        dir_exe_.Append(kTraceReportFileName));
    } else {
      LOG(INFO)
        << "tracing disabled";
    }
  }

  {
    LOG(INFO)
      << "ThreadPool Shutdown...";
    TRACE_EVENT0("shutdown", "MainLoop:ThreadPool");
    DCHECK(base::ThreadPool::GetInstance());
    base::ThreadPool::GetInstance()->Shutdown();
    // Tasks posted with TaskShutdownBehavior::BLOCK_SHUTDOWN and
    // tasks posted with TaskShutdownBehavior::SKIP_ON_SHUTDOWN that
    // have started to run before the Shutdown() call
    // have now completed their execution.
    // Tasks posted with TaskShutdownBehavior::CONTINUE_ON_SHUTDOWN
    // may still be running.
  }
}

/// \todo refactor long method
bool ScopedBaseEnvironment::init(
  int argc
  , char* argv[]
  , const bool auto_start_tracer
  , const std::string event_categories
  , const base::FilePath& outDir
  , const int threadsNum)
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

  base::PlatformThread::SetName("Main");

  // see https://stackoverflow.com/a/18981514/10904212
  std::locale::global(std::locale::classic());

  basis::initCommandLine(argc, argv);

#if DCHECK_IS_ON()
  // Must be done before hooking any functions that make stack traces.
  base::debug::EnableInProcessStackDumping();
#endif // DCHECK_IS_ON()

  base::SamplingHeapProfiler::Get()->SetRecordThreadNames(true);

  /// \todo
  // init allocator https://github.com/aranajhonny/chromium/blob/caf5bcb822f79b8997720e589334266551a50a13/content/app/content_main_runner.cc#L512

  base::EnableTerminationOnHeapCorruption();

  base::FeatureList::InitializeInstance(
    std::string(), std::string());

  basis::initLogging(
    "" // logFile
    );

  if(!base::PathExists(dir_exe_.Append(kIcuDataFileName))) {
    LOG(ERROR)
        << "unable to load icu i18n data file: "
        << dir_exe_.Append(kIcuDataFileName);
    // stop app execution with EXIT_FAILURE
    return
      false;
  }

  icu_util::initICUi18n(kIcuDataFileName);

  /// \note you must init ICU before i18n
  i18n = std::make_unique<i18n::I18n>(
    nullptr // locale
    );

  {
    const int num_cores
      = base::SysInfo::NumberOfProcessors();
    const int kBackgroundMaxThreads
    /// \note based on command-line paramater
      = 1 + threadsNum;
    const int maxByDemandWorkerThreadsInPool
    /// \note based on command-line paramater
      = 1 + threadsNum;
    const int kForegroundMaxThreads
      = std::max(
          kBackgroundMaxThreads + maxByDemandWorkerThreadsInPool
          , num_cores - 1);
    CHECK(kForegroundMaxThreads >= 1)
      << "Unable to register foreground threads."
      " Make sure you have at leat one cpu core";

    basis::initThreadPool(
      kBackgroundMaxThreads
      , kForegroundMaxThreads);
  }

  // register flextool::ApplicationPathKeys
  flextool::AddPathProvider();

  // see http://dev.chromium.org/developers/how-tos/trace-event-profiling-tool
  basis::initTracing(
    auto_start_tracer
    , event_categories
    );

  /// \todo Disable MemoryPressureListener when memory coordinator is enabled.
  //base::MemoryPressureListener::SetNotificationsSuppressed(false);

  base::StatisticsRecorder::InitLogOnShutdown();

  // set current path
  {
    CHECK(!outDir.empty());
    base::SetCurrentDirectory(outDir);
    DCHECK(base::FilePath{fs::current_path().string()} == outDir);
    VLOG(9)
        << "Current path is "
        << fs::current_path();
  }

  return
    true;
}

} // flextool
