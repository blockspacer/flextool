#include <iostream>
#include <iterator>
#include <exception>
#include <string>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <vector>
#include <iomanip>
#include <sstream>

#include "flexlib/ToolPlugin.hpp"
#include "flextool/PluginManager.hpp"
#include "flexlib/parser_constants.hpp"

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <entt/entt.hpp>

// __has_include is currently supported by GCC and Clang. However GCC 4.9 may have issues and
// returns 1 for 'defined( __has_include )', while '__has_include' is actually not supported:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63662
#if __has_include(<filesystem>)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif // __has_include

#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/program_options.hpp>
#include <boost/utility/in_place_factory.hpp>

#include <basis/icu_util.hpp>

#include <base/bind.h>
#include <base/files/file_util.h>
#include <base/path_service.h>
#include <base/base64.h>
#include <base/base64url.h>
#include <base/command_line.h>
#include <base/lazy_instance.h>
#include <base/logging.h>
#include <base/trace_event/trace_event.h>
#include <base/cpu.h>
#include <base/optional.h>
#include <base/path_service.h>
#include <base/time/time.h>
#include <base/memory/ptr_util.h>
#include <base/macros.h>
#include <base/stl_util.h>
#include <base/bind.h>
#include <base/bind_helpers.h>
#include <base/memory/scoped_refptr.h>
#include <base/single_thread_task_runner.h>
#include <base/threading/thread_task_runner_handle.h>
#include <base/numerics/checked_math.h>
#include <base/numerics/clamped_math.h>
#include <base/numerics/safe_conversions.h>
#include <base/i18n/icu_string_conversions.h>
#include <base/i18n/string_compare.h>
#include <base/stl_util.h>
#include <base/base_switches.h>
#include <base/command_line.h>
#include <base/containers/small_map.h>
#include <base/i18n/icu_util.h>
#include <base/i18n/rtl.h>
#include <base/allocator/partition_allocator/page_allocator.h>
#include <base/allocator/allocator_shim.h>
#include <base/allocator/buildflags.h>
#include <base/allocator/partition_allocator/partition_alloc.h>
#include <base/memory/scoped_refptr.h>
#include <base/i18n/rtl.h>
#include <base/stl_util.h>
#include <base/memory/ref_counted_memory.h>
#include <base/memory/read_only_shared_memory_region.h>
#include <base/stl_util.h>
#include <base/bind.h>
#include <base/memory/weak_ptr.h>
#include <base/threading/thread.h>
#include <base/logging.h>
#include <base/system/sys_info.h>
#include <base/synchronization/waitable_event.h>
#include <base/observer_list.h>
#include <base/synchronization/lock.h>
#include <base/threading/thread.h>
#include "base/files/file_path.h"
#include "base/strings/string_util.h"
#include <base/timer/timer.h>
#include <base/callback.h>
#include <base/bind.h>
#include <base/callback.h>
#include <base/command_line.h>
#include <base/logging.h>
#include <base/memory/weak_ptr.h>
#include <base/message_loop/message_loop.h>
#include <base/optional.h>
#include <base/bind.h>
#include <base/callback.h>
#include <base/files/file_path.h>
#include <base/message_loop/message_loop.h>
#include <base/threading/thread.h>
#include <base/threading/thread_checker.h>
#include <base/feature_list.h>
#include <base/bind.h>
#include <base/files/file_util.h>
#include <base/path_service.h>
#include <base/at_exit.h>
#include <base/command_line.h>
#include <base/message_loop/message_loop.h>
#include <base/run_loop.h>
#include <base/trace_event/trace_event.h>
#include <base/trace_event/trace_buffer.h>
#include <base/trace_event/trace_log.h>
#include <base/trace_event/memory_dump_manager.h>
#include <base/trace_event/heap_profiler.h>
#include <base/trace_event/heap_profiler_allocation_context_tracker.h>
#include <base/trace_event/heap_profiler_event_filter.h>
#include <base/sampling_heap_profiler/sampling_heap_profiler.h>
#include <base/sampling_heap_profiler/poisson_allocation_sampler.h>
#include <base/trace_event/malloc_dump_provider.h>
#include <base/trace_event/memory_dump_provider.h>
#include <base/trace_event/memory_dump_scheduler.h>
#include <base/trace_event/memory_infra_background_whitelist.h>
#include <base/trace_event/process_memory_dump.h>
#include <base/trace_event/trace_event.h>
#include <base/allocator/allocator_check.h>
#include <base/base_switches.h>
#include <base/threading/sequence_local_storage_map.h>
#include <base/command_line.h>
#include <base/debug/alias.h>
#include <base/debug/stack_trace.h>
#include <base/memory/ptr_util.h>
#include <base/sequenced_task_runner.h>
#include <base/third_party/dynamic_annotations/dynamic_annotations.h>
#include <base/threading/thread.h>
#include <base/threading/thread_task_runner_handle.h>
#include <base/process/process_handle.h>
#include <base/single_thread_task_runner.h>
#include <base/task_runner.h>
#include <base/task_runner_util.h>
#include <base/task/thread_pool/thread_pool.h>
#include <base/task/thread_pool/thread_pool_impl.h>
#include <base/threading/thread.h>
#include <base/threading/thread_local.h>
#include <base/metrics/histogram.h>
#include <base/metrics/histogram_macros.h>
#include <base/metrics/statistics_recorder.h>
#include <base/metrics/user_metrics.h>
#include <base/metrics/user_metrics_action.h>
#include <base/threading/platform_thread.h>

#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

//#include "core/errors/errors.hpp"

#include "flexlib/utils.hpp"

#include "flexlib/funcParser.hpp"
//#include "flexlib/inputThread.hpp"

#include "flexlib/clangUtils.hpp"

#include "flexlib/clangPipeline.hpp"

#include "flexlib/annotation_parser.hpp"
#include "flexlib/annotation_match_handler.hpp"

#include "flexlib/matchers/annotation_matcher.hpp"

#include "flexlib/options/ctp/options.hpp"

#if defined(CLING_IS_ON)
#include "flexlib/ClingInterpreterModule.hpp"
#endif // CLING_IS_ON

#include "flexlib/ctp_registry.hpp"

#include "flextool/version.hpp"

namespace po = boost::program_options;

#if __has_include(<filesystem>)
namespace fs = std::filesystem;
#else
namespace fs = std::experimental::filesystem;
#endif // __has_include

static boost::optional<std::string> log_config;

static boost::optional<std::string> srcdir_arg;

static boost::optional<std::string> outdir_arg;

static boost::optional<std::string> plugin_dir_arg;

static boost::optional<std::string> plugin_conf_arg;

static boost::optional<std::string> clang_includes_dir_arg;

static boost::optional<std::string> cling_includes_dir_arg;

static boost::optional<int> thread_num_arg;

static std::vector<std::string> ctp_scripts_search_paths;

static std::vector<std::string> pathsToExtraPluginFiles;

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}

static bool startsWith(const std::string& in, const std::string& prefix) {
  return !in.compare(0, prefix.size(), prefix);
};

static std::string removePrefix(const std::string& from,
    const std::string& prefix) {
  return from.substr( prefix.size(), from.size() );
};


static inline void initCommandLine(int argc, char* argv[])
{
  base::PlatformThread::SetName("Main");

  // see https://stackoverflow.com/a/18981514/10904212
  std::locale::global(std::locale::classic());

  // see https://peter.sh/experiments/chromium-command-line-switches/
  DCHECK(!base::CommandLine::InitializedForCurrentProcess());
  base::CommandLine::Init(argc, argv);

  DCHECK(base::CommandLine::ForCurrentProcess());
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();

  // initialize |g_vlog_info| in debug mode
#if !defined(NDEBUG)
  /// \note usage
  /// ./app --vmodule=*main*=100 --enable-logging=stderr --log-level=100
  // Gives the default maximal active V-logging level; 0 is the default.
  // Normally positive values are used for V-logging levels.
  if(!command_line->HasSwitch(switches::kV)) {
    command_line->AppendSwitchASCII(switches::kV,
      "1");
  }
  // Gives the per-module maximal V-logging levels to override the value
  // given by --v.  E.g. "my_module=2,foo*=3" would change the logging
  // level for all code in source files "my_module.*" and "foo*.*"
  // ("-inl" suffixes are also disregarded for this matching).
  //
  // Any pattern containing a forward or backward slash will be tested
  // against the whole pathname and not just the module.  E.g.,
  // "*/foo/bar/*=2" would change the logging level for all code in
  // source files under a "foo/bar" directory.
  if(!command_line->HasSwitch(switches::kVModule)) {
    command_line->AppendSwitchASCII(switches::kVModule,
      "*main*=0"
      ",*webrtc*=2"
      ",*libjingle*=2");
  }

  // Indicates that crash reporting should be enabled. On platforms where helper
  // processes cannot access to files needed to make this decision, this flag is
  // generated internally.
  if(!command_line->HasSwitch(switches::kEnableCrashReporter)) {
    command_line->AppendSwitchASCII(switches::kEnableCrashReporter,
      "1");
  }

  // Generates full memory crash dump.
  if(!command_line->HasSwitch(switches::kFullMemoryCrashReport)) {
    command_line->AppendSwitchASCII(switches::kFullMemoryCrashReport,
      "1");
  }

#if defined(OS_LINUX)
    // Controls whether or not retired instruction counts are surfaced for threads
    // in trace events on Linux.
    //
    // This flag requires the BPF sandbox to be disabled.  if(!command_line->HasSwitch(switches::kVModule)) {
    if(!command_line->HasSwitch(switches::kEnableThreadInstructionCount)) {
      command_line->AppendSwitchASCII(switches::kEnableThreadInstructionCount,
        "1");
    }
#endif

  DCHECK(command_line->HasSwitch(switches::kV) ||
      command_line->HasSwitch(switches::kVModule));
#endif // NDEBUG

  // Must be done before hooking any functions that make stack traces.
  base::debug::EnableInProcessStackDumping();

  base::SamplingHeapProfiler::Get()->SetRecordThreadNames(true);

  /// \todo
  // init allocator https://github.com/aranajhonny/chromium/blob/caf5bcb822f79b8997720e589334266551a50a13/content/app/content_main_runner.cc#L512
  // base::EnableTerminationOnHeapCorruption();
  // base::EnableTerminationOnOutOfMemory();
  // mojo::embedder::Init();
  // mojo::ServiceManager::GetInstance();
//#if !defined(OFFICIAL_BUILD)
//  base::debug::EnableInProcessStackDumping();
//#if defined(OS_WIN)
//  base::RouteStdioToConsole(false);
//#endif
//#endif

  base::FeatureList::InitializeInstance(std::string(), std::string());

  /// \todo
  //base::FeatureList::InitializeInstance(
  //    command_line->GetSwitchValueASCII(switches::kEnableFeatures),
  //    command_line->GetSwitchValueASCII(switches::kDisableFeatures));

  // DCHECK(!base::TaskScheduler::GetInstance());
  // // A one-per-process task scheduler is needed for usage of APIs in
  // // base/post_task.h
  // base::TaskScheduler::CreateAndStartWithDefaultParams("MainThreadPool");
  // DCHECK(base::TaskScheduler::GetInstance());
}

static const base::FilePath::CharType kIcuDataFileName[]
  = FILE_PATH_LITERAL("./resources/icu/icudtl.dat");

/// \todo
//static inline void initPathService()
//{
//  base::PathService::RegisterProvider(
//    &application::paths::PathProvider
//    , application::paths::PATH_APP_START
//    , application::paths::PATH_APP_END);
//
//  base::FilePath log_directory;
//  base::PathService::Get(
//    application::paths::DIR_APP_DEBUG_OUT, &log_directory);
//  DLOG(INFO)
//    << "TODO: write logs to "
//    << log_directory.value();
//
//  base::FilePath test_root_directory;
//  base::PathService::Get(
//    application::paths::DIR_APP_TEST_OUT, &test_root_directory);
//  DLOG(INFO)
//    << "TODO: read test data from "
//    << test_root_directory.value();
//
//  base::FilePath web_root_directory;
//  base::PathService::Get(
//    application::paths::DIR_APP_WEB_ROOT, &web_root_directory);
//  DLOG(INFO)
//    << "TODO: serve web files from "
//    << web_root_directory.value();
//
//  DCHECK(base::MessageLoop::current());
//}

/// \todo move to TraceManager
/// https://github.com/blockspacer/cobalt-clone-28052019/blob/89664d116629734759176d820e9923257717e09c/src/cobalt/browser/trace_manager.h
/// \todo use ScopedTraceToFile
/// https://github.com/blockspacer/cobalt-clone-cab7770533804d582eaa66c713a1582f361182d3/blob/0b72f93b07285f3af3c8452ae2ceaf5860ca7c72/src/cobalt/trace_event/scoped_trace_to_file.cc
static inline void initTracing(
  const bool auto_start_tracer
  , const std::string event_categories)
{
  base::trace_event::MallocDumpProvider::GetInstance()
    ->DisableMetrics();

#if defined(OS_MACOSX)
  // On macOS, this call is necessary to shim malloc zones that were created
  // after startup. This cannot be done during shim initialization because the
  // task scheduler has not yet been initialized.
  base::allocator::PeriodicallyShimNewMallocZones();
#endif

  // see https://github.com/mbbill/libTraceEvent
  // see https://habr.com/ru/post/256907/
  // see http://dev.chromium.org/developers/how-tos/trace-event-profiling-tool
  // see https://www.chromium.org/developers/how-tos/trace-event-profiling-tool/tracing-event-instrumentation
  base::trace_event::TraceConfig trace_config{
    // |category_filter_string| is a comma-delimited list of category wildcards.
    // A category can have an optional '-' prefix to make it an excluded category.
    // All the same rules apply above, so for example, having both included and
    // excluded categories in the same list would not be supported.
    // Example: "test_MyTest*"
    // Example: "test_MyTest*,test_OtherStuff"
    // Example: "-excluded_category1,-excluded_category2"
    event_categories
    //, base::trace_event::RECORD_CONTINUOUSLY
    , base::trace_event::RECORD_UNTIL_FULL
    };

  /// \todo https://www.chromium.org/developers/how-tos/trace-event-profiling-tool/memory/howto-adding-memory-infra-tracing-to-a-component
  //base::tracing::ProcessMetricsMemoryDumpProvider::RegisterForProcess(
  //    base::kNullProcessId);

  /// \todo use base::trace_event::MemoryDumpManager::GetInstance()->RegisterDumpProvider
  // Create trace config with heap profiling filter.
  /*std::string filter_string = base::JoinString(
      {"*", TRACE_DISABLED_BY_DEFAULT("net"), TRACE_DISABLED_BY_DEFAULT("cc"),
       base::trace_event::MemoryDumpManager::kTraceCategory},
      ",");
  base::trace_event::TraceConfigCategoryFilter category_filter;
  category_filter.InitializeFromString(filter_string);

  base::trace_event::TraceConfig::EventFilterConfig heap_profiler_filter_config(
      base::HeapProfilerEventFilter::kName);
  heap_profiler_filter_config.SetCategoryFilter(category_filter);

  base::trace_event::TraceConfig::EventFilters enabled_event_filters_;
  enabled_event_filters_.push_back(heap_profiler_filter_config);
  trace_config.SetEventFilters(enabled_event_filters_);*/

  //base::trace_event::TraceLog::GetInstance()->SetEnabled(
  //  base::trace_event::TraceConfig(
  //    base::trace_event::MemoryDumpManager::kTraceCategory, ""),
  //  base::trace_event::TraceLog::RECORDING_MODE);

  base::trace_event::TraceLog::GetInstance()->SetEnabled(
    trace_config
    , base::trace_event::TraceLog::RECORDING_MODE);

  if(!auto_start_tracer) {
    base::trace_event::TraceLog::GetInstance()->SetDisabled();
    LOG(INFO)
      << "Tracing disabled."
      << " You can control tracing by cmd flags:"
      << " --start_tracing"
      /// \todo
      //<< " You can control tracing by commands:"
      //<< " '\\start_tracing' and '\\stop_tracing'"
      ;
  } else {
    LOG(INFO)
      << "Tracing started with config: "
      << trace_config.ToString();
    LOG(INFO)
      << "Tracing file may require a lot of space, so"
      << " consider enabling tracing on-demand"
      //<< " by commands";
      //<< " '\\start_tracing' and '\\stop_tracing'"
      ;
  }

  base::trace_event::TraceLog::GetInstance()
    ->set_process_name("Main Process");

  {
    using base::trace_event::AllocationContextTracker;
    using CaptureMode
      = base::trace_event::AllocationContextTracker::CaptureMode;

    DCHECK(AllocationContextTracker
      ::GetInstanceForCurrentThread());
    AllocationContextTracker::SetCaptureMode(
      CaptureMode::DISABLED);
  }

  base::SamplingHeapProfiler::Init();

  //base::PoissonAllocationSampler::Get()
  //  ->SuppressRandomnessForTest(true);
  auto* h_profiler = base::SamplingHeapProfiler::Get();
  DCHECK(h_profiler);
  const uint32_t profile_id = h_profiler->Start();

  base::trace_event::MemoryDumpManager* mdump_instance
    = base::trace_event::MemoryDumpManager::GetInstance();
  DCHECK(mdump_instance);
  mdump_instance->Initialize(
   base::BindRepeating(
     [
     ](
       base::trace_event::MemoryDumpType dump_type
       , base::trace_event::MemoryDumpLevelOfDetail level_of_detail
     ){
       NOTIMPLEMENTED();
     }
   )
   , false // Only coordinator process triggers periodic memory dumps.
  );

  mdump_instance->SetupForTracing(
    base::trace_event::TraceConfig::MemoryDumpConfig());

  //void* volatile p = base::allocator::UncheckedAlloc(400); /// \todo
  //free(p);

  std::vector<base::SamplingHeapProfiler::Sample> h_samples =
      h_profiler->GetSamples(
        //profile_id
        0 // To retrieve all set to 0.
        );
  h_profiler->Stop();
  //DCHECK(h_samples.size() > 0); /// \todo
  std::map<size_t, size_t> h_buckets;
  std::map<size_t, size_t> h_sums;
  for (auto& sample : h_samples) {
    h_buckets[sample.size] += sample.total;
  }
  for (auto& it : h_buckets) {
    //if (it.first != 400 && it.first != 700 && it.first != 20480)
    //  continue;
    h_sums[it.first] += it.second;
    LOG(INFO)
        << "h_sums: " << it.second;
  }

  static uint64_t dump_guid = 0;
  base::trace_event::MemoryDumpType dump_type
    = base::trace_event::MemoryDumpType::EXPLICITLY_TRIGGERED;
  base::trace_event::MemoryDumpLevelOfDetail level_of_detail
    = base::trace_event::MemoryDumpLevelOfDetail::DETAILED;
  base::trace_event::MemoryDumpRequestArgs request_args{
    dump_guid, dump_type, level_of_detail};
  mdump_instance->CreateProcessDump(request_args
    , base::BindRepeating(
      [](
        bool success
        , uint64_t dump_guid
        , std::unique_ptr<base::trace_event::ProcessMemoryDump> pmd
      ){
        using base::trace_event::TracedValue;

        LOG(INFO)
          << "MemoryDumpRequest done"
          << (success ? " fine" : " with errors");
        DCHECK(success || !pmd);
        if(!success || !pmd) {
          DCHECK(false); /// \todo
          return;
        }

        //const auto& allocator_dumps = pmd->allocator_dumps();
        //for (const auto& expected_dump_name : allocator_dumps.) {
        //  LOG(INFO)
        //    << "expected_dump_name: "
        //    << expected_dump_name;
        //}

        std::unique_ptr<TracedValue> traced_value
          = std::make_unique<TracedValue>();

        pmd->SerializeAllocatorDumpsInto(traced_value.get());
        {
          bool enabled;
          TRACE_EVENT_CATEGORY_GROUP_ENABLED(
              base::trace_event::MemoryDumpManager::kTraceCategory, &enabled);
          //DCHECK(enabled);
        }
        traced_value->SetString(
            "level_of_detail",
            base::trace_event::MemoryDumpLevelOfDetailToString(
              /// \todo
              base::trace_event::MemoryDumpLevelOfDetail::DETAILED
              ));

        base::trace_event::TraceArguments trace_args(
          "dumps"
          , std::move(traced_value));

        // don't forget to enable tracing category
        // and call `TeardownForTracing`
        TRACE_EVENT_API_ADD_TRACE_EVENT(
            TRACE_EVENT_PHASE_MEMORY_DUMP,
            base::trace_event::TraceLog::GetCategoryGroupEnabled(
                base::trace_event::MemoryDumpManager::kTraceCategory)
            , base::trace_event::MemoryDumpTypeToString(
               /// \todo
               base::trace_event::MemoryDumpType::EXPLICITLY_TRIGGERED
            )
            , ::trace_event_internal::kGlobalScope
            , dump_guid
            //, 0//pid ///\todo
            , &trace_args
            , TRACE_EVENT_FLAG_HAS_ID);
        TRACE_EVENT_NESTABLE_ASYNC_BEGIN2(
            base::trace_event::MemoryDumpManager::kTraceCategory
            , "GlobalMemoryDump"
            , TRACE_ID_LOCAL(dump_guid)
            , "dump_type"
            , base::trace_event::MemoryDumpTypeToString(
                /// \todo
                base::trace_event::MemoryDumpType::EXPLICITLY_TRIGGERED
              )
            , "level_of_detail",
            base::trace_event::MemoryDumpLevelOfDetailToString(
                /// \todo
                base::trace_event::MemoryDumpLevelOfDetail::DETAILED
              )
            );
        TRACE_EVENT_NESTABLE_ASYNC_END2(
            base::trace_event::MemoryDumpManager::kTraceCategory
            , "GlobalMemoryDump"
            , TRACE_ID_LOCAL(dump_guid)
            , "dump_type"
            , base::trace_event::MemoryDumpTypeToString(
                /// \todo
                base::trace_event::MemoryDumpType::EXPLICITLY_TRIGGERED
              )
            , "level_of_detail",
            base::trace_event::MemoryDumpLevelOfDetailToString(
                /// \todo
                base::trace_event::MemoryDumpLevelOfDetail::DETAILED
              )
            );
        LOG(INFO)
          << "MemoryDumpRequest finished";
      }
  ));
}

static void inline initThreadPool(const int network_threads_num)
{
  DCHECK(!base::ThreadPool::GetInstance());

  DCHECK(network_threads_num > 1);

  // Values were chosen so that:
  // * There are few background threads.
  // * Background threads never outnumber foreground threads.
  // * The system is utilized maximally by foreground threads.
  // * The main thread is assumed to be busy, cap foreground workers at
  //   |num_cores - 1|.
  const int num_cores
    = base::SysInfo::NumberOfProcessors();

  const int kBackgroundMaxThreads
    = network_threads_num;
  DCHECK(kBackgroundMaxThreads < num_cores);

  constexpr int maxByDemandWorkerThreadsInPool = 3;
  const int kForegroundMaxThreads
    = std::max(network_threads_num + maxByDemandWorkerThreadsInPool, num_cores - 1);

  if(num_cores < kForegroundMaxThreads)
  {
    LOG(WARNING)
      << "(low grade CPU or bad config)"
      << " num_cores < foreground max threads."
      << " Where"
      << " foreground max threads = " << kForegroundMaxThreads
      << " num_cores = " << num_cores;
  }

  //constexpr int kMaxNumBackgroundThreads = 2;
  //constexpr int kMaxNumForegroundThreads = 4;

  std::unique_ptr<base::ThreadPool> thread_pool_
    = std::make_unique<base::internal::ThreadPoolImpl>("Test");

  //thread_pool_->SetCanRun(true);

  // when to reclaim idle threads
  base::TimeDelta kSuggestedReclaimTime
    = base::TimeDelta::FromSeconds(30);

  //thread_pool_->Start(
  //  base::internal::ThreadPoolImpl::InitParams{
  //    {kMaxNumBackgroundThreads, reclaim_time}
  //    , {kMaxNumForegroundThreads, reclaim_time}
  //  }
  //  , /*worker_thread_observer_*/nullptr);

  base::ThreadPool::SetInstance(std::move(thread_pool_));
  base::ThreadPool::GetInstance()->Start(
    base::internal::ThreadPoolImpl::InitParams{
      {kBackgroundMaxThreads, kSuggestedReclaimTime}
      , {kForegroundMaxThreads, kSuggestedReclaimTime}
    }
    , /*worker_thread_observer_*/nullptr);
}

static void initLogging(const std::string& log_file) {
  /// \note make sure you init-ed CommandLine like so `base::CommandLine::Init(0, nullptr);`
  logging::LoggingSettings logging_settings;
  logging_settings.logging_dest
    = log_file.empty() ?
      logging::LOG_TO_SYSTEM_DEBUG_LOG : logging::LOG_TO_FILE;
  logging_settings.log_file = log_file.c_str();
  // Indicates that the log file should be locked when being written to.
  // Unless there is only one single-threaded process that is logging to
  // the log file, the file should be locked during writes to make each
  // log output atomic. Other writers will block.
  logging_settings.lock_log = logging::LOCK_LOG_FILE;
  CHECK(logging::InitLogging(logging_settings));

  // To view log output with IDs and timestamps use "adb logcat -v threadtime".
  logging::SetLogItems(true,   // Process ID
                       true,   // Thread ID
                       true,   // Timestamp
                       true);  // Tick count
  VLOG(1)
    << "Log file "
    << logging_settings.log_file;
}

static std::vector<fs::path> find_ctp_scripts_dirs(
  std::vector<fs::path> ctp_scripts_search_paths) {
  std::vector<fs::path> res;
  for(const fs::path& it: ctp_scripts_search_paths) {
    const fs::path ctp_scripts_path = it / "ctp_scripts";
    if(fs::exists(ctp_scripts_path)
       && fs::is_directory(ctp_scripts_path))
    {
      res.push_back(ctp_scripts_path);
    } else {
      DLOG(WARNING)
        << "can`t find folder: "
        << ctp_scripts_path.string();
    }
  }
  return res;
}

/// \todo make use of it
static void process_ctp_scripts_dir(
  const std::string& ctp_scripts_path
  , cling_utils::ClingInterpreter& clingInterpreter)
{
#if defined(CLING_IS_ON)
#if defined(ALLOW_PER_PROJECT_CTP_SCRIPTS)
    // Init InterpreterModule files
    //if(const auto Interp_it
    //   = InterpreterModule::moduleToSources.find("main_module")
    //   ; Interp_it == InterpreterModule::moduleToSources.end())
    {
      std::vector<fs::path> ctp_scripts_paths(
        fs::recursive_directory_iterator(ctp_scripts_path), fs::recursive_directory_iterator{}
      );

      /// \note we must be able to change loading order
      /// by file naming (0_file, 1_file)
      std::sort(ctp_scripts_paths.begin(), ctp_scripts_paths.end());

      for (const auto & file_entry : ctp_scripts_paths) {
        fs::path full_path = fs::absolute(file_entry);
        if(full_path.extension() == ".cpp"
           /// \note skip path containing `generated` as substr
           && full_path.string().find("generated") == std::string::npos)
        {
          clingInterpreter.loadFile(full_path);
          //InterpreterModule::moduleToSources["main_module"].push_back(full_path.string());
          /// \todo use boost outcome for error reporting
          DVLOG(9)
            << "added to InterpreterModule file "
            << full_path.string();
        }
      }
    }

    //for(const auto& it : InterpreterModule::moduleToSources) {
    //    reloadClingModule(it.first, it.second);
    //    /// \todo use boost outcome for error reporting
    //    XLOG(DBG9) << "reloaded module " << it.first;
    //}
#else
      //InterpreterModule::interpMap["main_module"] = std::make_unique<InterpreterModule>(
      //          "main_module",
      //          std::vector<std::string>{});
#endif // ALLOW_PER_PROJECT_CTP_SCRIPTS

#endif // CLING_IS_ON
}


static void tryAppendHeadersPathArg(
  const base::FilePath& path
  , std::vector<std::string>& out_args
  , const std::string& includeFlag)
{
  out_args.push_back(includeFlag
    + path.value());
  if(!base::PathExists(path)) {
    LOG(WARNING)
      << "Unable to find path to headers: "
      << path;
  }
}

/**
 * Clang provides some options out-of-the-box:
 *
 * Generic Options:
 *
 *   -help
 *     - Display available options (-help-hidden for more)
 *   -help-list
 *     - Display list of available options (-help-list-hidden for more)
 *   -version
 *     - Display the version of this program
 *
 * Use override options:
 *
 *   -extra-arg=<string>
 *     - Additional argument to append to the compiler command line
 *   -extra-arg-before=<string>
 *     - Additional argument to prepend to the compiler command line
 *   -p=<string>
 *     - Build path
 **/
static void add_default_clang_args(
    const base::FilePath clangHeadersPath
    , std::vector<std::string> &args)
{
  const char kIncludeFlag[] = "-extra-arg=-I";

  base::FilePath dir_exe;
  if (!base::PathService::Get(base::DIR_EXE, &dir_exe)) {
    NOTREACHED();
    return;
  }

  args.push_back("clang_app"); // app name

  //args.push_back("-extra-arg=-nostdinc");

  args.push_back("-extra-arg=-DCLANG_ENABLED=1");
  args.push_back("-extra-arg=-DCLANG_IS_ON=1");

  // https://stackoverflow.com/a/30877725
  args.push_back("-extra-arg=-DBOOST_SYSTEM_NO_DEPRECATED");
  args.push_back("-extra-arg=-DBOOST_ERROR_CODE_HEADER_ONLY");

  // include paths
  {
    args.push_back(kIncludeFlag
      + dir_exe.value());

    if(!clangHeadersPath.empty()) {
      tryAppendHeadersPathArg(
        clangHeadersPath
        , args
        , kIncludeFlag);
    }

    //tryAppendHeadersPathArg(
    //  clangHeadersPath.Append("./lib/clang/5.0.0/include")
    //  , args
    //  , kIncludeFlag);

    //tryAppendHeadersPathArg(
    //  clangHeadersPath.Append("./src/include")
    //  , args
    //  , kIncludeFlag);
    //
    //tryAppendHeadersPathArg(
    //  clangHeadersPath.Append("./build/include")
    //  , args
    //  , kIncludeFlag);
    //
    //tryAppendHeadersPathArg(
    //  clangHeadersPath.Append("./src/tools/clang/include")
    //  , args
    //  , kIncludeFlag);
    //
    //tryAppendHeadersPathArg(
    //  clangHeadersPath.Append("./build/tools/clang/include")
    //  , args
    //  , kIncludeFlag);
    //
    //tryAppendHeadersPathArg(
    //  clangHeadersPath.Append("./src/tools/cling/include")
    //  , args
    //  , kIncludeFlag);

    args.push_back(kIncludeFlag
      + dir_exe.Append("./resources").value());
  }

  args.push_back("-extra-arg=-std=c++17");
}

static void add_default_cling_args(
    const base::FilePath clingHeadersPath
    , std::vector<std::string> &clingInterpreterArgs)
{
  const char kIncludeFlag[] = "-I";

  base::FilePath dir_exe;
  if (!base::PathService::Get(base::DIR_EXE, &dir_exe)) {
    NOTREACHED();
    return;
  }

  //clingInterpreterArgs.push_back("-nostdinc");

  clingInterpreterArgs.push_back("-DCLING_ENABLED=1");
  clingInterpreterArgs.push_back("-DCLING_IS_ON=1");

  // https://stackoverflow.com/a/30877725
  clingInterpreterArgs.push_back("-DBOOST_SYSTEM_NO_DEPRECATED");
  clingInterpreterArgs.push_back("-DBOOST_ERROR_CODE_HEADER_ONLY");

  clingInterpreterArgs.push_back("EmbedCling"); // app name
  clingInterpreterArgs.push_back("--std=c++17");

  // include paths
  {
    clingInterpreterArgs.push_back(kIncludeFlag
      + dir_exe.value());

    if(!clingInterpreterArgs.empty()) {
      tryAppendHeadersPathArg(
        clingHeadersPath
        , clingInterpreterArgs
        , kIncludeFlag);
    }

    //tryAppendHeadersPathArg(
    //  clingHeadersPath.Append("./lib/clang/5.0.0/include")
    //  , clingInterpreterArgs
    //  , kIncludeFlag);

    //tryAppendHeadersPathArg(
    //  clingHeadersPath.Append("./src/include")
    //  , clingInterpreterArgs
    //  , kIncludeFlag);
    //
    //tryAppendHeadersPathArg(
    //  clingHeadersPath.Append("./build/include")
    //  , clingInterpreterArgs
    //  , kIncludeFlag);
    //
    //tryAppendHeadersPathArg(
    //  clingHeadersPath.Append("./src/tools/clang/include")
    //  , clingInterpreterArgs
    //  , kIncludeFlag);
    //
    //tryAppendHeadersPathArg(
    //  clingHeadersPath.Append("./build/tools/clang/include")
    //  , clingInterpreterArgs
    //  , kIncludeFlag);
    //
    //tryAppendHeadersPathArg(
    //  clingHeadersPath.Append("./src/tools/cling/include")
    //  , clingInterpreterArgs
    //  , kIncludeFlag);

    clingInterpreterArgs.push_back(kIncludeFlag
      + dir_exe.Append(
          "./resources").value());
  }
}

/// \todo make use of it
static const char* help_arg_name = "help";
/// \todo make use of it
//static const char* log_arg_name = "log,L";
// --srcdir - filesystem path for input files
static const char* srcdir_arg_name = "srcdir,S";
// --outdir - filesystem path for output files.
static const char* outdir_arg_name = "outdir,R";
/// \todo make use of it
static const char* version_arg_name = "version,V";
static const char* plugin_dir_arg_name = "plugins_dir";
static const char* plugin_conf_arg_name = "plugins_config";
/// \todo remove
static const char* clang_includes_dir_arg_name = "clang_includes_dir";
/// \todo remove
static const char* cling_includes_dir_arg_name = "cling_includes_dir";
/// \todo make use of it
static const char* threads_arg_name = "threads,J";
/// \todo remove
static const char* ctp_scripts_paths_arg_name = "ctp_scripts_paths,C";
// paths to plugin files that
// must be loaded independently from plugins configuration file
static const char* pathsToExtraPluginFilesArg = "load_plugin";

int main(int argc, char* argv[]) {
  po::variables_map vm;
  std::unique_ptr<po::basic_parsed_options<char>> parsed_options;
  po::options_description desc("Allowed options");
  try {
    desc.add_options()
      (help_arg_name, "produce help message")
      (version_arg_name, "produce version message")
      (clang_includes_dir_arg_name
       , po::value(&clang_includes_dir_arg)->default_value(boost::none, "")
       , "path to directory with clang includes")
      (cling_includes_dir_arg_name
       , po::value(&cling_includes_dir_arg)->default_value(boost::none, "")
       , "path to directory with cling includes")
      (plugin_dir_arg_name
       , po::value(&plugin_dir_arg)->default_value(boost::none, "")
       , "path to directory with plugins")
      (plugin_conf_arg_name
       , po::value(&plugin_conf_arg)->default_value(boost::none, "")
       , "path to plugins configuration file")
      (ctp_scripts_paths_arg_name
       , po::value(&ctp_scripts_search_paths)->multitoken()
       , "list of paths where toll will search for ctp_scripts subfolder")
      (pathsToExtraPluginFilesArg
       , po::value(&pathsToExtraPluginFiles)->multitoken()
       , "paths to plugin files that "
         "must be loaded independently from plugins configuration file")
      (outdir_arg_name
       , po::value(&outdir_arg)->default_value(boost::none, "")
       , "change output directory path (where to place generated files)")
      (srcdir_arg_name
       , po::value(&srcdir_arg)->default_value(boost::none, "")
       , "change current working directory path (path to template files)")
      (threads_arg_name
       , po::value(&thread_num_arg)->default_value(boost::none, "")
       , "number of threads")
      /// \todo LOG arg
      //(log_arg_name, po::value(&log_config)->
      //  default_value(boost::none, ""), "log configuration")
      ;

    parsed_options = std::make_unique<po::basic_parsed_options<char>>(po::command_line_parser(argc, argv).
      options(desc).allow_unregistered().run());
    po::store(*parsed_options, vm);

    po::notify(vm);
  }
  catch(std::exception& e) {
    LOG(ERROR) << "ERROR: " << e.what();
    return EXIT_FAILURE;
  }
  catch(...) {
    LOG(ERROR) << "ERROR: Exception of unknown type!";
    return EXIT_FAILURE;
  }

  initCommandLine(argc, argv);

  initLogging("");

  // This object instance is required (for example,
  // LazyInstance, MessageLoop).
  base::AtExitManager at_exit;

  /// \note creates base::MessageLoop::current()
  base::MessageLoopForIO main_thread_message_loop;

  icu_util::initICUi18n(kIcuDataFileName);

  /// \todo
  //initPathService();

  // allows to schedule arbitrary tasks on main loop
  scoped_refptr<base::SingleThreadTaskRunner> main_loop_task_runner
    = base::MessageLoop::current()->task_runner();

  /// \todo use multithreaded clang libtooling
  initThreadPool(
    /*thread num*/ std::clamp(thread_num_arg.get_value_or(2), 0, 100));

  initTracing(false //auto_start_tracer
    , "*" //tracing_categories
  );

  /// \todo Disable MemoryPressureListener when memory coordinator is enabled.
  //base::MemoryPressureListener::SetNotificationsSuppressed(false);

  base::StatisticsRecorder::InitLogOnShutdown();

  base::FilePath dir_exe;
  if (!base::PathService::Get(base::DIR_EXE, &dir_exe)) {
    NOTREACHED();
    return EXIT_FAILURE;
  }

  if(plugin_dir_arg.is_initialized() && !plugin_dir_arg.value().empty()) {
    DVLOG(9) << "plugin_dir_arg " << plugin_dir_arg.value();
    ctp::Options::pathToDirWithPlugins
      = base::MakeAbsoluteFilePath(
          base::FilePath(plugin_dir_arg.value()));
    if (!base::PathExists(ctp::Options::pathToDirWithPlugins)) {
      LOG(WARNING) << "path must exist: " << plugin_conf_arg.value();
    }
    else if(!base::DirectoryExists(ctp::Options::pathToDirWithPlugins)) {
      LOG(WARNING) << "path must be directory: " << plugin_dir_arg.value();
    }
    else {
      ctp::Options::pathToDirWithPlugins
        = base::FilePath{plugin_dir_arg.value()};
    }
  }

  ctp::Options::pathToDirWithPluginsConfigFile
    // default value
    = dir_exe.AppendASCII("plugins").AppendASCII("plugins.conf");

  if(plugin_conf_arg.is_initialized() && !plugin_conf_arg.value().empty()) {
    DVLOG(9) << "plugin_conf_arg " << plugin_conf_arg.value();
    ctp::Options::pathToDirWithPluginsConfigFile
      = base::MakeAbsoluteFilePath(
          base::FilePath(plugin_conf_arg.value()));
    if (!base::PathExists(ctp::Options::pathToDirWithPluginsConfigFile)) {
      LOG(WARNING) << "path must exist: " << plugin_conf_arg.value();
    }
    else if(base::DirectoryExists(ctp::Options::pathToDirWithPluginsConfigFile)) {
      LOG(WARNING) << "path be NOT directory: " << plugin_conf_arg.value();
    }
    else {
      ctp::Options::pathToDirWithPluginsConfigFile
        = base::FilePath{plugin_conf_arg.value()};
    }
  }

  base::FilePath clangHeadersPath{};

  if(clang_includes_dir_arg.is_initialized() && !clang_includes_dir_arg.value().empty()) {
    DLOG(INFO) << "clang_includes_dir_arg " << clang_includes_dir_arg.value();
    ctp::Options::res_path = fs::absolute(fs::path(clang_includes_dir_arg.value()));
    if(!fs::is_directory(ctp::Options::res_path)) {
      LOG(WARNING) << "not directory " << clang_includes_dir_arg.value();
    } else {
      clangHeadersPath = base::FilePath{clang_includes_dir_arg.value()};
    }
  }

  base::FilePath clingHeadersPath{};

  if(cling_includes_dir_arg.is_initialized() && !cling_includes_dir_arg.value().empty()) {
    DLOG(INFO) << "cling_includes_dir_arg " << cling_includes_dir_arg.value();
    ctp::Options::res_path = fs::absolute(fs::path(cling_includes_dir_arg.value()));
    if(!fs::is_directory(ctp::Options::res_path)) {
      LOG(WARNING) << "not directory " << cling_includes_dir_arg.value();
    } else {
      clingHeadersPath = base::FilePath{cling_includes_dir_arg.value()};
    }
  }

  backend::PluginManager plug_mgr;

  /// \note dispatcher must be thread-safe,
  /// so use entt after patch https://github.com/skypjack/entt/issues/449
  /// see https://github.com/skypjack/entt/commit/74f3df83dbc9fc4b43b8cfb9d71ba02234bd5c4a
  std::unique_ptr<entt::dispatcher> main_events_dispatcher
    = std::make_unique<entt::dispatcher>();
  plug_mgr.connect_to_dispatcher(*main_events_dispatcher);

  std::vector<std::string> args_storage;
  std::vector<std::string> cling_extra_args;

  /// \note must be before all custom arguments
  add_default_clang_args(clangHeadersPath, args_storage);

  /// \note forward unregistered options to clang libtooling
  for (const auto& o : (*parsed_options).options) {
    if (vm.find(o.string_key) == vm.end()) {
      // an unknown option
      for(size_t i = 0; i < o.value.size(); i++) {
        const std::string combined_for_clang
          = o.string_key + o.value[i];
        DVLOG(9) << "forwarded unregistered option "
                      "to clang libtooling: "
                      << combined_for_clang;
        if(combined_for_clang.empty()) {
          continue;
        }
        args_storage.push_back(combined_for_clang);
        /// \note only arguments starting with `-extra-arg=`
        /// will be forwarded
        if(startsWith(combined_for_clang, "-extra-arg=")) {
          const std::string combined_for_cling
            = removePrefix(combined_for_clang, "-extra-arg=");
          if(combined_for_cling.empty()) {
            continue;
          }
#if defined(CLING_IS_ON)
          // pipeline clang args to cling
          cling_extra_args.push_back(combined_for_cling);
#endif // CLING_IS_ON
        }
      }
    }
  }

  if (vm.count(help_arg_name)) {
    LOG(INFO) << desc;
    args_storage.push_back("-help");
    /// \note continue to forward help to clang libtooling
  }

  if (vm.count("version") || vm.count("V")) {
    LOG(INFO)
      << "tool version: "
      << flextool_VERSION;

    /// \note allow plugins to process commands before pre-built logic
    main_events_dispatcher->trigger<
      plugin::ToolPlugin::Events::StringCommand>(
        plugin::ToolPlugin::Events::StringCommand{
          .raw_cmd = /*copy*/ "/version"
          , .split_parts = /*copy*/ std::vector<std::string>{"version"}
        }
      );

    return EXIT_SUCCESS;
  }

  /// \note that function may be usefull if Cling is turned off
  /// because functions may be called by name in Cling
  //ctp_utils::add_modulecallbacks();

  /// \note must be after add_modulecallbacks()
  //if (vm.count("plugins") || vm.count("P")) {
  //  LOG(INFO) << "loaded plugins (ctp scripts): ";
  //  for(const auto& it : clang_utils::get_cxxctp_callbacks()) {
  //    LOG(INFO) << "  + " << it.first;
  //  }
  //  return EXIT_SUCCESS;
  //}

#if defined(CLING_IS_ON)
  //if (ctp_scripts_search_paths.empty()) {
  //  if (clang_utils::get_cxxctp_callbacks().empty()) {
  //    LOG(ERROR) << "ERROR: ctp_scripts_paths not set "
  //                 "and no built-in scripts/plugins available.";
  //    return EXIT_FAILURE;
  //  }
  //  LOG(WARNING) << "WARNING: ctp_scripts_paths not set.";
  //}

  auto fix_search_paths = [](std::vector<std::string>& inout) {
    std::vector<std::string> v;
    for(const auto& it: inout) {
      DLOG(INFO) << "before: " << it;
      // split a string by blank spaces unless it is in quotes
      std::istringstream iss(it);
        std::string s;
        while (iss >> std::quoted(s)) {
            if(!s.empty()) {
              /// \note made path absolute
              v.push_back(fs::absolute(s));
              DLOG(INFO) << "after: " << s;
            }
        }
    }
    inout = v;
  };

  fix_search_paths(ctp_scripts_search_paths);
#else
  if(ctp_scripts_search_paths.size() > 0) {
    LOG(ERROR) << "ERROR: `ctp_scripts_paths` option "
                 "require cling support! "
                 "Build tool with DENABLE_CLING=TRUE";
    return EXIT_FAILURE;
  }
#endif // CLING_IS_ON

  ctp::Options::src_path = fs::absolute(fs::current_path());
  if(srcdir_arg.is_initialized() && !srcdir_arg.value().empty()) {
    DLOG(INFO) << "srcdir_arg " << srcdir_arg.value();
    ctp::Options::src_path = fs::absolute(fs::path(srcdir_arg.value()));
    if(!fs::is_directory(ctp::Options::src_path)) {
      LOG(ERROR) << srcdir_arg.value() << " must be directory";
      return EXIT_FAILURE;
    }
    fs::current_path(srcdir_arg.value());
  }

  ctp::Options::res_path = fs::absolute(fs::current_path());

  if(outdir_arg.is_initialized() && !outdir_arg.value().empty()) {
    DLOG(INFO) << "outdir_arg " << outdir_arg.value();
    ctp::Options::res_path = fs::absolute(fs::path(outdir_arg.value()));
    if(!fs::is_directory(ctp::Options::res_path)) {
      LOG(WARNING) << "created directory " << outdir_arg.value();
      fs::create_directories(ctp::Options::res_path);
    }
  }

#if defined(CLING_IS_ON)
  DVLOG(9) << "ctp_scripts_search_paths (" << ctp_scripts_search_paths.size() << "): ";
  for(const std::string& it: ctp_scripts_search_paths) {
    ctp::Options::ctp_scripts_search_paths.push_back(it);
    DVLOG(9)
      << "added path to cling interpreter files: "
      << it;
  }
#endif // CLING_IS_ON

  for(const std::string& it: pathsToExtraPluginFiles) {
    ctp::Options::pathsToExtraPluginFiles.push_back(base::FilePath{it});
    DVLOG(9)
      << "added path to plugin file: "
      << it;
  }

  DVLOG(9) << "Current path is " << fs::current_path();
  DVLOG(9) << "srcdir: Local path is " << srcdir_arg.get_value_or("");
  DVLOG(9) << "srcdir: Absolute path is " << ctp::Options::src_path;
  DVLOG(9) << "outdir: Local path is " << outdir_arg.get_value_or("");
  DVLOG(9) << "outdir: Absolute path is " << ctp::Options::res_path;

  auto chrono_then = std::chrono::steady_clock::now();

//#if defined(CLING_IS_ON)
//    // cling thread used as C++ interpreter
//    std::thread /// \todo use base::Thread
//        cling_thread(cling_utils::cling_func);
//    cling_thread.detach();
//
//    // Wait until main() sends data
//    {
//      std::unique_lock<std::mutex> lk(cling_utils::InterpreterModule::clingReadyMutex);
//      cling_utils::InterpreterModule::clingReadyCV.wait(lk, []{return cling_utils::InterpreterModule::isClingReady;});
//    }
//#endif // CLING_IS_ON

  // https://stackoverflow.com/questions/53525502/compiling-c-on-the-fly-clang-libtooling-fails-to-set-triple-for-llvm-ir
  // https://stackoverflow.com/questions/27092593/how-to-use-standard-library-with-clang-and-libtooling
  std::vector< const char* > args_vec;
  {
      std::vector< std::string >::const_iterator iarg;
      for( iarg = args_storage.begin() ; iarg != args_storage.end() ; ++iarg ) {
          args_vec.push_back(iarg->c_str());
      }
  }

  ::cxxctp::AnnotationMethods annotationMethods;
  ::clang_utils::SourceTransformPipeline sourceTransformPipeline;

#if defined(CLING_IS_ON)
  std::vector<std::string> clingIncludePaths{".", "../"};

  std::vector<std::string> clingInterpreterArgs;
  {
    add_default_cling_args(clingHeadersPath, clingInterpreterArgs);

    // pipeline clang args to cling
    for(const auto& it: cling_extra_args) {
      clingInterpreterArgs.push_back(it);
    }
  }

  cling_utils::ClingInterpreter clingInterpreter(
        "MainClingInterpreter_debug_id"
        , clingInterpreterArgs
        , clingIncludePaths
  );
  if(ctp::Options::ctp_scripts_search_paths.empty()) {
    LOG(WARNING)
      << "command-line argument ctp_scripts_paths not provided. "
      << "it must contain C++ files what will be loaded by cling";
  } else {
    std::vector<fs::path> ctp_scripts_paths =
      find_ctp_scripts_dirs(ctp::Options::ctp_scripts_search_paths);
    if(ctp_scripts_paths.empty()) {
      LOG(WARNING)
        << "can`t find ctp_scripts folder in: "
        << ctp::Options::ctp_scripts_search_paths;
    }
    for(const fs::path& it: ctp_scripts_paths) {
      process_ctp_scripts_dir(it, clingInterpreter);
    }
  }
#endif // defined(CLING_IS_ON)

  {
    main_events_dispatcher->trigger<
        backend::PluginManager::Events::Startup
      >(backend::PluginManager::Events::Startup{
        .pathToDirWithPlugins = ctp::Options::pathToDirWithPlugins
        , .pathToPluginsConfFile = ctp::Options::pathToDirWithPluginsConfigFile
        , .pathsToExtraPluginFiles =  ctp::Options::pathsToExtraPluginFiles
      }
    );
    // each loaded plugin must receive |connect_plugins_to_dispatcher|
    if(!plug_mgr.countLoadedPlugins()) {
      LOG(WARNING)
        << "unable to find plugins in plugins directory: "
        << dir_exe.AppendASCII("plugins").MaybeAsASCII();
    }
    plug_mgr.connect_plugins_to_dispatcher(*main_events_dispatcher);
  }

#if defined(CLING_IS_ON)
  main_events_dispatcher->trigger<
    plugin::ToolPlugin::Events::RegisterClingInterpreter>(
      plugin::ToolPlugin::Events::RegisterClingInterpreter{
        .clingInterpreter = &clingInterpreter
      }
    );
#endif // defined(CLING_IS_ON)

  /// \note allow plugins to process commands before pre-built logic
  main_events_dispatcher->trigger<
    plugin::ToolPlugin::Events::RegisterAnnotationMethods>(
      plugin::ToolPlugin::Events::RegisterAnnotationMethods{
        .annotationMethods = &annotationMethods
        , .sourceTransformPipeline = &sourceTransformPipeline
      }
    );

  ::cxxctp::AnnotationParser annotationParser(&annotationMethods);

  ::cxxctp::AnnotationMatchHandler anotationMatchHandler(
    &annotationParser, &annotationMethods);
  scoped_refptr<clang_utils::AnnotationMatchOptions>
    annotationMatchOptions
      = new clang_utils::AnnotationMatchOptions(
          ::cxxctp::kAnnotateAttrName
          , base::BindRepeating(
              &::cxxctp::AnnotationMatchHandler::matchHandler
              , base::Unretained(&anotationMatchHandler))
          , base::BindRepeating(
              &::cxxctp::AnnotationMatchHandler::endSourceFileHandler
              , base::Unretained(&anotationMatchHandler))
        );

  // LibTooling is the C++ interface Clang provided.
  // It is very useful when you want to have full control over AST
  // (e.g. static analysis), or to implement a refactoring tool.
  // see https://kevinaboos.wordpress.com/2013/07/23/clang-tutorial-part-i-introduction/
  {
    int args_arc = args_vec.size();

    const char **args_argv = &(args_vec[0]);

    // see http://llvm.org/docs/doxygen/html/classllvm_1_1cl_1_1OptionCategory.html
    llvm::cl::OptionCategory UseOverrideCategory("Use override options");

    // parse the command-line args passed to your code
    // see http://clang.llvm.org/doxygen/classclang_1_1tooling_1_1CommonOptionsParser.html
    clang::tooling::CommonOptionsParser op(args_arc, args_argv,
      UseOverrideCategory);

    // TODO: support for clang::tooling::FixedCompilationDatabase
    // https://github.com/mlomb/MetaCPP/blob/8eddde5e1bf4809ad70a68a385b9cbcc8e237370/MetaCPP-CLI/ScraperTool.cpp#L19

    for(const auto& it: op.getSourcePathList()) {
      base::FilePath absolutePath
        = base::MakeAbsoluteFilePath(base::FilePath(it));
      DLOG(INFO)
        << "added source path = "
        << absolutePath;
    }

    // create a new Clang Tool instance (a LibTooling environment)
    // see http://clang.llvm.org/doxygen/classclang_1_1tooling_1_1ClangTool.html
    clang::tooling::ClangTool Tool(
      op.getCompilations(),
      op.getSourcePathList());

    // Run the Clang Tool, creating a new FrontendAction.
    // The benefit of using LibTooling is that you can do things
    // before and after the analysis, like printing out
    // the modified code or counting the number of functions
    // in several source files.
    Tool.run(
      new clang_utils::AnnotationMatchFactory(
        annotationMatchOptions));
  }

//#if defined(CLING_IS_ON)
//    bool quit = false;
//    while(!quit)
//    {
//        /// \note must wait for Cling & ClangTool threads
//        quit = cling_utils::InterpreterModule::receivedMessagesQueue_->isEmpty();
//    }
//#endif // CLING_IS_ON

  LOG(INFO)
    << "PluginManager shutdown...";
  main_events_dispatcher->trigger<
    backend::PluginManager::Events::Shutdown>();

  long int diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now() - chrono_then)
                 .count();
  long int diff_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                 std::chrono::steady_clock::now() - chrono_then)
                 .count();
  DLOG(INFO)
    << "Done in : "
    << diff_ms << " milliseconds"
    <<" (" << diff_ns << " nanoseconds)";
  return EXIT_SUCCESS;
}
