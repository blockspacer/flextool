#include "flextool/app_environment.hpp" // IWYU pragma: associated

#include "flextool/version.hpp"
#include "flextool/path_provider.hpp"

#include <basis/icu_util.hpp>
#include <basis/log_util.hpp>
#include <basis/thread_pool_util.hpp>
#include <basis/cmd_util.hpp>
#include <basis/tracing_util.hpp>
#include <basis/scoped_log_run_time.hpp>
#include <basis/i18n.hpp>

#include <base/files/file_util.h>
#include <base/path_service.h>
#include <base/sequenced_task_runner.h>
#include <base/bind.h>
#include <base/files/file_util.h>
#include <base/path_service.h>
#include <base/base64.h>
#include <base/base64url.h>
#include <base/command_line.h>
#include <base/lazy_instance.h>
#include <base/logging.h>
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
#include <base/files/file_path.h>
#include <base/strings/string_util.h>
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

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}

static const base::FilePath::CharType kIcuDataFileName[]
  = FILE_PATH_LITERAL("./resources/icu/optimal/icudt64l.dat");

/// \todo make use of it
//static std::vector<fs::path> find_ctp_scripts_dirs(
//  std::vector<fs::path> ctp_scripts_search_paths) {
//  std::vector<fs::path> res;
//  for(const fs::path& it: ctp_scripts_search_paths) {
//    const fs::path ctp_scripts_path = it / "ctp_scripts";
//    if(fs::exists(ctp_scripts_path)
//       && fs::is_directory(ctp_scripts_path))
//    {
//      res.push_back(ctp_scripts_path);
//    } else {
//      DLOG(WARNING)
//        << "can`t find folder: "
//        << ctp_scripts_path.string();
//    }
//  }
//  return res;
//}

/// \todo make use of it
//static void process_ctp_scripts_dir(
//  const std::string& ctp_scripts_path
//  , cling_utils::ClingInterpreter& clingInterpreter)
//{
//#if defined(CLING_IS_ON)
//#if defined(ALLOW_PER_PROJECT_CTP_SCRIPTS)
//    // Init InterpreterModule files
//    //if(const auto Interp_it
//    //   = InterpreterModule::moduleToSources.find("main_module")
//    //   ; Interp_it == InterpreterModule::moduleToSources.end())
//    {
//      std::vector<fs::path> ctp_scripts_paths(
//        fs::recursive_directory_iterator(ctp_scripts_path), fs::recursive_directory_iterator{}
//      );
//
//      /// \note we must be able to change loading order
//      /// by file naming (0_file, 1_file)
//      std::sort(ctp_scripts_paths.begin(), ctp_scripts_paths.end());
//
//      for (const auto & file_entry : ctp_scripts_paths) {
//        fs::path full_path = fs::absolute(file_entry);
//        if(full_path.extension() == ".cpp"
//           /// \note skip path containing `generated` as substr
//           && full_path.string().find("generated") == std::string::npos)
//        {
//          clingInterpreter.loadFile(full_path);
//          //InterpreterModule::moduleToSources["main_module"].push_back(full_path.string());
//          /// \todo use boost outcome for error reporting
//          VLOG(9)
//            << "added to InterpreterModule file "
//            << full_path.string();
//        }
//      }
//    }
//
//    //for(const auto& it : InterpreterModule::moduleToSources) {
//    //    reloadClingModule(it.first, it.second);
//    //    /// \todo use boost outcome for error reporting
//    //    XLOG(DBG9) << "reloaded module " << it.first;
//    //}
//#else
//      //InterpreterModule::interpMap["main_module"] = std::make_unique<InterpreterModule>(
//      //          "main_module",
//      //          std::vector<std::string>{});
//#endif // ALLOW_PER_PROJECT_CTP_SCRIPTS
//
//#endif // CLING_IS_ON
//}

} // namespace

namespace flextool {

ScopedAppEnvironment::ScopedAppEnvironment()
  : appCmd(boostCmd)
  , main_events_dispatcher(std::make_unique<entt::dispatcher>())
  , main_loop_task_runner(base::MessageLoop::current()->task_runner())
{
  DCHECK(base::MessageLoop::current()->task_runner());
}

ScopedAppEnvironment::~ScopedAppEnvironment()
{
  {
    DCHECK(main_events_dispatcher);
    VLOG(9)
      << "PluginManager shutdown...";
    main_events_dispatcher->trigger<
      backend::PluginManager::Events::Shutdown>();
  }

  {
    DCHECK(base::trace_event::MemoryDumpManager::GetInstance());
    base::trace_event::MemoryDumpManager::GetInstance()
      ->TeardownForTracing();
  }

  // save tracing report to file, if needed
  {
    DCHECK(base::trace_event::TraceLog::GetInstance());

    // TODO: refactor tracing https://github.com/blockspacer/cobalt-clone-cab7770533804d582eaa66c713a1582f361182d3/blob/master/src/cobalt/trace_event/json_file_outputter.cc
    const bool need_write_tracing_report
      = base::trace_event::TraceLog::GetInstance()->IsEnabled();
  #if defined(NDEBUG)
    CHECK(!need_write_tracing_report);
  #endif // NDEBUG

    base::trace_event::TraceLog::GetInstance()->SetDisabled();

    if(need_write_tracing_report)
    {
      base::trace_event::TraceResultBuffer trace_buffer;

      static const char tracingJsonPath[] = "trace_report.json";
      base::File output_file(
        base::FilePath::FromUTF8Unsafe(tracingJsonPath)
        , base::File::FLAG_CREATE_ALWAYS | base::File::FLAG_WRITE);

      static const char kStart[] = "{\"traceEvents\":[";
      output_file.WriteAtCurrentPos(kStart, strlen(kStart));

      trace_buffer.SetOutputCallback(base::Bind(
          []
          (base::File& output_file
           , const std::string& data)
          {
            output_file.WriteAtCurrentPos(
              data.c_str()
              , data.length());
          }
          , std::ref(output_file)
        )
      );

      base::trace_event::MemoryDumpManager::GetInstance()
        ->TeardownForTracing();

      base::RunLoop trace_run_loop{};

      // you can open resulting file in chrome://tracing
      base::trace_event::TraceLog::GetInstance()->Flush(
      base::Bind(
        []
        (base::File& output_file
         , base::Closure quit_closure
         , base::trace_event::TraceResultBuffer* trace_buffer
         , const scoped_refptr<base::RefCountedString>& json_events_str
         , bool has_more_events)
        {
          DCHECK(trace_buffer);
          trace_buffer->AddFragment(json_events_str->data());
          if (!has_more_events) {
              quit_closure.Run();
          } else {
            output_file.WriteAtCurrentPos(",", 1);
          }
        },
        std::ref(output_file)
        , trace_run_loop.QuitClosure()
        , base::Unretained(&trace_buffer)));

      trace_run_loop.Run();

      CHECK(output_file.IsValid());

      static const char kEnd[] = "]}";
      output_file.WriteAtCurrentPos(kEnd, strlen(kEnd));
      output_file.Close();

      LOG(INFO)
        << "tracing report written into file: "
        << tracingJsonPath
        << ". You can open resulting file in chrome://tracing";
    } else {
      VLOG(9)
        << "tracing to file disabled,"
        << " nothing to do";
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
    // have started to run before the Shutdown() call have now completed their
    // execution. Tasks posted with
    // TaskShutdownBehavior::CONTINUE_ON_SHUTDOWN may still be
    // running.
  }
}

bool ScopedAppEnvironment::init(int argc, char* argv[])
{
  DCHECK(argc > 0);

  if (!base::PathService::Get(base::DIR_EXE, &dir_exe_)) {
    NOTREACHED();
    // stop app execution with EXIT_FAILURE
    return false;
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
      return false;
    }
  }

  base::PlatformThread::SetName("Main");

  // see https://stackoverflow.com/a/18981514/10904212
  std::locale::global(std::locale::classic());

  basis::initCommandLine(argc, argv);

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

  base::FeatureList::InitializeInstance(
    std::string(), std::string());

  /// \todo
  //base::FeatureList::InitializeInstance(
  //    command_line->GetSwitchValueASCII(switches::kEnableFeatures),
  //    command_line->GetSwitchValueASCII(switches::kDisableFeatures));

  // DCHECK(!base::TaskScheduler::GetInstance());
  // // A one-per-process task scheduler is needed for usage of APIs in
  // // base/post_task.h
  // base::TaskScheduler::CreateAndStartWithDefaultParams("MainThreadPool");
  // DCHECK(base::TaskScheduler::GetInstance());

  basis::initLogging(
    "" // logFile
  );

  if(!base::PathExists(dir_exe_.Append(kIcuDataFileName))) {
    LOG(ERROR)
      << "unable to load icu i18n data file: "
      << dir_exe_.Append(kIcuDataFileName);
    // stop app execution with EXIT_FAILURE
    return false;
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
      = 1 + appCmd.threadsNum();
    const int maxByDemandWorkerThreadsInPool
      = 1 + appCmd.threadsNum();
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

  flextool::AddPathProvider();

  basis::initTracing(
    appCmd.hasAutoStartTracer()
    , appCmd.tracingCategories()
  );

  /// \todo Disable MemoryPressureListener when memory coordinator is enabled.
  //base::MemoryPressureListener::SetNotificationsSuppressed(false);

  base::StatisticsRecorder::InitLogOnShutdown();

  {
    DCHECK(main_events_dispatcher);
    plug_mgr.connect_to_dispatcher(*main_events_dispatcher);
  }

  {
    VLOG(9)
      << "creating"
         " plugin::ToolPlugin::Events::Init";
    DCHECK(main_events_dispatcher);
    main_events_dispatcher->trigger<
      plugin::ToolPlugin::Events::Init>(
        plugin::ToolPlugin::Events::Init{
          argc
          , argv
        }
      );
  }

  {
    const base::FilePath clangBuildPath = appCmd.outDir();
    DCHECK(!clangBuildPath.empty());
    const bool ok = clang_util::populateClangArguments(
      clangBuildPath
      , args_storage
      , cling_extra_args
      , boostCmd);
    if(!ok) {
      LOG(ERROR)
        << "unable to populate Clang arguments";
      return false;
    }
    DCHECK(!args_storage.empty());

    LOG(INFO)
      << "populated Clang arguments: "
      << args_storage;
  }

  if (appCmd.hasHelp()) {
    LOG(INFO) << boostCmd.optionsToString();

    const char kClangHelpFlag[] = "-help";

    args_storage.push_back(kClangHelpFlag);
    /// \note continue to forward help to clang libtooling
  }

  if (appCmd.hasVersion())
  {
    LOG(INFO)
      << "tool version: "
      << flextool_VERSION;

    /// \todo remove plugin::ToolPlugin::Events::StringCommand
    /// \note allow plugins to process commands before pre-built logic
    {
      const char StringCommandDelim[] = "/";
      const char kVersionStringCommand[] = "version";

      DCHECK(main_events_dispatcher);
      main_events_dispatcher->trigger<
        plugin::ToolPlugin::Events::StringCommand>(
          plugin::ToolPlugin::Events::StringCommand{
            .raw_cmd = /*copy*/
                std::string(StringCommandDelim) + kVersionStringCommand
            , .split_parts = /*copy*/
                std::vector<std::string>{kVersionStringCommand}
          }
        );
    }

    // stop app execution with EXIT_SUCCESS
    return true;
  }

  /// \note that function may be usefull if Cling is turned off
  /// because functions may be called by name in Cling
  //ctp_utils::add_modulecallbacks();

  /// \note must be after add_modulecallbacks()
  //if (vm.count("plugins") || vm.count("P")) {
  //  LOG(INFO) << "loaded plugins (ctp scripts): ";
  //  for(const auto& it : clang_utils::get_flexlib_callbacks()) {
  //    LOG(INFO) << "  + " << it.first;
  //  }
  //  return false; // stop app execution
  //}

//#if defined(CLING_IS_ON)
//  //if (ctp_scripts_search_paths.empty()) {
//  //  if (clang_utils::get_flexlib_callbacks().empty()) {
//  //    LOG(ERROR) << "ERROR: ctp_scripts_paths not set "
//  //                 "and no built-in scripts/plugins available.";
//  //    return false; // stop app execution
//  //  }
//  //  LOG(WARNING) << "WARNING: ctp_scripts_paths not set.";
//  //}
//
//  auto fix_search_paths = [](std::vector<std::string>& inout) {
//    std::vector<std::string> v;
//    for(const auto& it: inout) {
//      DLOG(INFO) << "before: " << it;
//      // split a string by blank spaces unless it is in quotes
//      std::istringstream iss(it);
//        std::string s;
//        while (iss >> std::quoted(s)) {
//            if(!s.empty()) {
//              /// \note made path absolute
//              v.push_back(fs::absolute(s));
//              DLOG(INFO) << "after: " << s;
//            }
//        }
//    }
//    inout = v;
//  };
//
//  fix_search_paths(ctp_scripts_search_paths);
//#else
//  if(ctp_scripts_search_paths.size() > 0) {
//    LOG(ERROR) << "ERROR: `ctp_scripts_paths` option "
//                 "require cling support! "
//                 "Build tool with DENABLE_CLING=TRUE";
//    return false; // stop app execution
//  }
//#endif // CLING_IS_ON

  //ctp::Options::src_path = fs::absolute(fs::current_path());
  //if(srcdir_arg.is_initialized() && !srcdir_arg.value().empty()) {
  //  DLOG(INFO) << "srcdir_arg " << srcdir_arg.value();
  //  ctp::Options::src_path = fs::absolute(fs::path(srcdir_arg.value()));
  //  if(!fs::is_directory(ctp::Options::src_path)) {
  //    LOG(ERROR) << srcdir_arg.value() << " must be directory";
  //    return false; // stop app execution
  //  }
  //  fs::current_path(srcdir_arg.value());
  //}

  const base::FilePath outDir = appCmd.outDir();
  // set current path
  {
    CHECK(!outDir.empty());
    base::SetCurrentDirectory(outDir);
    DCHECK(base::FilePath{fs::current_path().string()} == outDir);
    VLOG(9)
      << "Current path is "
      << fs::current_path();
  }

  //ctp::Options::res_path = fs::absolute(fs::current_path());
  //
  //if(outdir_arg.is_initialized() && !outdir_arg.value().empty()) {
  //  DLOG(INFO) << "outdir_arg " << outdir_arg.value();
  //  ctp::Options::res_path = fs::absolute(fs::path(outdir_arg.value()));
  //  if(!fs::is_directory(ctp::Options::res_path)) {
  //    LOG(WARNING) << "created directory " << outdir_arg.value();
  //    fs::create_directories(ctp::Options::res_path);
  //  }
  //}

//#if defined(CLING_IS_ON)
//  VLOG(9) << "ctp_scripts_search_paths (" << ctp_scripts_search_paths.size() << "): ";
//  for(const std::string& it: ctp_scripts_search_paths) {
//    ctp::Options::ctp_scripts_search_paths.push_back(it);
//    VLOG(9)
//      << "added path to cling interpreter files: "
//      << it;
//  }
//#endif // CLING_IS_ON

  //for(const std::string& it: pathsToExtraPluginFiles) {
  //  ctp::Options::pathsToExtraPluginFiles.push_back(base::FilePath{it});
  //  VLOG(9)
  //    << "added path to plugin file: "
  //    << it;
  //}

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

#if defined(CLING_IS_ON)
  std::vector<std::string> clingIncludePaths{".", "../"};

  std::vector<std::string> clingInterpreterArgs;
  {
    const bool ok
      = clang_util::add_default_cling_args(clingInterpreterArgs);
    if(!ok) {
      LOG(ERROR)
        << "unable to populate Cling arguments";
      return false;
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

  for(const base::FilePath& full_path: appCmd.scriptFiles()) {
    DCHECK(clingInterpreter);
    DCHECK(!full_path.empty());
    VLOG(9)
      << "calling"
         " clingInterpreter->loadFile";
    clingInterpreter->loadFile(full_path.value());
    LOG(INFO)
      << "added to InterpreterModule file "
      << full_path;
  }

  //if(ctp::Options::ctp_scripts_search_paths.empty()) {
  //  LOG(WARNING)
  //    << "command-line argument ctp_scripts_paths not provided. "
  //    << "it must contain C++ files what will be loaded by cling";
  //} else {
  //  std::vector<fs::path> ctp_scripts_paths =
  //    find_ctp_scripts_dirs(ctp::Options::ctp_scripts_search_paths);
  //  if(ctp_scripts_paths.empty()) {
  //    LOG(WARNING)
  //      << "can`t find ctp_scripts folder in: "
  //      << ctp::Options::ctp_scripts_search_paths;
  //  }
  //  for(const fs::path& it: ctp_scripts_paths) {
  //    process_ctp_scripts_dir(it, clingInterpreter);
  //  }
  //}
#endif // defined(CLING_IS_ON)

  {
    // sanity check
    {
      base::FilePath curDir;
      base::GetCurrentDirectory(&curDir);
      CHECK(curDir == outDir);
    }

    {
      VLOG(9)
        << "running"
           " backend::PluginManager::Events::Startup";
      DCHECK(main_events_dispatcher);
      // must have default value
      DCHECK(!appCmd.pluginsDir().empty());
      // must have default value
      DCHECK(!appCmd.pluginsConfigFile().empty());
      main_events_dispatcher->trigger<
          backend::PluginManager::Events::Startup
        >(backend::PluginManager::Events::Startup{
          .pathToDirWithPlugins
            = appCmd.pluginsDir()
          , .pathToPluginsConfFile
            = appCmd.pluginsConfigFile()
          , .pathsToExtraPluginFiles
            =  appCmd.pathsToExtraPluginFiles()
        }
      );
    }

    /// \todo allow plugins to notify other plugins about
    /// their startup with custom data
    /// all you need is to store main_events_dispatcher in plugin
    /// and send startup event via that main_events_dispatcher
    /// other plugins must load special library or headers
    /// to process that startup event with same main_events_dispatcher
    // each loaded plugin must receive |connect_plugins_to_dispatcher|
    if(!plug_mgr.countLoadedPlugins()) {
      LOG(WARNING)
        << "unable to find plugins in plugins directory: "
        << dir_exe_.AppendASCII(cmd::kPluginsDir);
    }

    {
      VLOG(9)
        << "running"
           " plug_mgr.connect_plugins_to_dispatcher";
      DCHECK(main_events_dispatcher);
      plug_mgr.connect_plugins_to_dispatcher(*main_events_dispatcher);
    }
  }

#if defined(CLING_IS_ON)
  {
    VLOG(9)
      << "creating"
         " plugin::ToolPlugin::Events::RegisterClingInterpreter";
    DCHECK(clingInterpreter);
    DCHECK(main_events_dispatcher);
    main_events_dispatcher->trigger<
      plugin::ToolPlugin::Events::RegisterClingInterpreter>(
        plugin::ToolPlugin::Events::RegisterClingInterpreter{
          .clingInterpreter = clingInterpreter.get()
        }
      );
  }
#endif // defined(CLING_IS_ON)

  {
    VLOG(9)
      << "creating"
         " flexlib::AnnotationMethods";
    annotationMethods
      = std::make_unique<
          ::flexlib::AnnotationMethods
        >();
  }

  {
    VLOG(9)
      << "creating"
         " flexlib::AnnotationParser";
    DCHECK(annotationMethods);
    annotationParser
      = std::make_unique<
          ::flexlib::AnnotationParser
        >(annotationMethods.get());
  }

  {
    VLOG(9)
      << "creating"
         " flexlib::AnnotationMatchHandler";
    CHECK(!outDir.empty())
      << "you must provide output directory";
    DCHECK(annotationParser);
    DCHECK(annotationMethods);
    anotationMatchHandler
      = std::make_unique<
          ::flexlib::AnnotationMatchHandler
        >(annotationParser.get()
          , annotationMethods.get()
          , base::BindRepeating(
              &clang_util::FileSaveHandler::saveFile
              , base::Unretained(&fileSaveHandler) // must control lifetime
              , outDir
              , true // shouldFlushFile
              , false // shouldPrintConsole
          ));
  }

  {
    VLOG(9)
      << "creating"
         " clang_utils::AnnotationMatchOptions";
    DCHECK(!base::StringPiece(::flexlib::kAnnotateAttrName).empty());
    DCHECK(anotationMatchHandler);
    annotationMatchOptions =
      new clang_utils::AnnotationMatchOptions(
        ::flexlib::kAnnotateAttrName
        , base::BindRepeating(
            &::flexlib::AnnotationMatchHandler::matchHandler
            , base::Unretained(anotationMatchHandler.get()))
        , base::BindRepeating(
            &::flexlib::AnnotationMatchHandler::endSourceFileHandler
            , base::Unretained(anotationMatchHandler.get()))
      );
  }

  sourceTransformPipeline
    = std::make_unique<
        ::clang_utils::SourceTransformPipeline
      >();

  /// \note allow plugins to process commands before pre-built logic
  {
    VLOG(9)
      << "sending"
         " plugin::ToolPlugin::Events::RegisterAnnotationMethods";
    DCHECK(annotationMethods);
    DCHECK(sourceTransformPipeline);
    main_events_dispatcher->trigger<
      plugin::ToolPlugin::Events::RegisterAnnotationMethods>(
        plugin::ToolPlugin::Events::RegisterAnnotationMethods{
          .annotationMethods = annotationMethods.get()
          , .sourceTransformPipeline = sourceTransformPipeline.get()
        }
      );
  }

  return true;
}

} // flextool
