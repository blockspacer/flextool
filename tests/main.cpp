#ifdef __EMSCRIPTEN__
#define CATCH_INTERNAL_CONFIG_NO_POSIX_SIGNALS
#define CATCH_CONFIG_RUNNER
#else
// @see markfontenot.net/2016/09/13/c-and-catch-adding-your-own-main-method/
// #define CATCH_CONFIG_MAIN
// CATCH_CONFIG_RUNNER tells the catch library that this
// project will now explicitly call for the tests to be run.
#define CATCH_CONFIG_RUNNER
#endif

#include "basis/log/Logger.hpp"

#include <base/logging.h>
#include <base/i18n/icu_util.h>
#include <base/command_line.h>
#include <base/threading/platform_thread.h>
#include <base/base_switches.h>
#include <base/feature_list.h>
#include <base/at_exit.h>
#include <base/message_loop/message_loop.h>

#include <base/bind.h>
#include <base/test/launcher/unit_test_launcher.h>
#include <base/test/test_suite.h>
#include <build/build_config.h>

#include "testsCommon.h"

#if defined(USE_CATCH_TEST)
/*
 * runCatchTests will cause Catch to go ahead and
 * run your tests (that are contained in the tests.cpp file.
 * to do that, it needs access to the command line
 * args - argc and argv. It returns an integer that
 * ultimately gets passed back up to the operating system.
 * See the if statement at the top of main for
 * a better overview.
 */
int runCatchTests(int argc, char* const argv[]) {
  // This line of code causes the Catch library to
  // run the tests in the project.
  return Catch::Session().run(argc, argv);
}
#endif // USE_CATCH_TEST

#if defined(USE_CATCH_TEST) || defined(GTEST_NO_SUITE)
static inline void initI18n()
{
  /// \todo InitializeICUWithFileDescriptor
  bool icu_initialized = base::i18n::InitializeICU();
  //DCHECK(icu_initialized);
}

static inline void initCommandLine(int argc, char* argv[])
{
  base::PlatformThread::SetName("Main");

  // see https://stackoverflow.com/a/18981514/10904212
  std::locale::global(std::locale::classic());

  // see https://peter.sh/experiments/chromium-command-line-switches/
  base::CommandLine::Init(argc, argv);

  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();

  // initialize |g_vlog_info| in debug mode
#if !defined(NDEBUG)
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
#endif // USE_CATCH_TEST || defined(GTEST_NO_SUITE)

int main(int argc, char* argv[]) {
#if defined(USE_CATCH_TEST) || defined(GTEST_NO_SUITE)
  initCommandLine(argc, argv);

  // This object instance is required (for example,
  // LazyInstance, MessageLoop).
  base::AtExitManager at_exit;

  /// \note creates base::MessageLoop::current()
  base::MessageLoopForIO main_thread_message_loop;

  initI18n();

  gloer::log::Logger logger; // inits Logger
  LOG(INFO) << "created Logger...";

  // If the LogWorker is initialized then at scope exit the g3::shutDownLogging() will be called.
  // This is important since it protects from LOG calls from static or other entities that will go
  // out of scope at a later time.
  //
  // It can also be called manually:
  at_exit.RegisterTask(base::BindOnce(
    []
    (gloer::log::Logger& logger)
    {
      LOG(INFO) << "shutdown Logger...";
      logger.shutdown();
    }
    , std::ref(logger)
  ));
#endif // USE_CATCH_TEST || defined(GTEST_NO_SUITE)

#if defined(USE_CATCH_TEST)
  // If the TEST macro is defined to be true,
  // runCatchTests will be called and immediately
  // return causing the program to terminate. Change TEST
  // to false in the macro def at the top of this file
  // to skip tests and run the rest of your code.
  const int catch_res = runCatchTests(argc, argv);
  return catch_res;
#endif // USE_CATCH_TEST

#if defined(USE_GTEST_TEST)
#if defined(GTEST_NO_SUITE)
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  const int gtest_res = RUN_ALL_TESTS();
  return gtest_res;
#else
  base::TestSuite test_suite(argc, argv);
  return base::LaunchUnitTests(
      argc, argv,
      base::BindOnce(&base::TestSuite::Run, base::Unretained(&test_suite)));
#endif // GTEST_NO_SUITE
#endif // USE_GTEST_TEST

  // start working on other parts of your project here.
  return 0;
}
