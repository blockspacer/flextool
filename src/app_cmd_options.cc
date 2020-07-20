// IWYU pragma: no_include "boost/program_options/detail/value_semantic.hpp"

#include "flextool/app_cmd_options.hpp" // IWYU pragma: associated

#include "flextool/command_line_constants.hpp"

#include <build/build_config.h>

#include <base/base_paths.h>
#include <base/base_switches.h>
#include <base/logging.h>
#include <base/path_service.h>
#include <base/files/file_path.h>
#include <base/sequence_checker.h>
#include <base/strings/string_piece_forward.h>

#include <basis/PluginManager.hpp>
#include <basis/boost_command_line.hpp>
#include <basis/cmd_util.hpp>

#include <boost/none.hpp>
#include <boost/optional/optional.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/lexical_cast/bad_lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/type_index/type_index_facade.hpp>

#include <algorithm>
#include <stddef.h>

namespace po = boost::program_options;

namespace cmd {

namespace {

const char kPluginsConfigFilesDir[]
  = "resources/configuration_files";

[[nodiscard]] /* do not ignore return value */
static
AppCmdOptions::options_init&
registerCommonOptions(
  AppCmdOptions::options_init& options)
{
  return
    options
    // -- option --
    (
    cmd::kHelp, "produce help message"
    )
    // -- option --
    (
    cmd::kVersion, "produce version message"
    );
}

[[nodiscard]] /* do not ignore return value */
static
AppCmdOptions::options_init&
registerPluginsOptions(
  AppCmdOptions::options_init& options
  , boost::optional<std::string>& plugin_dir_arg
  , boost::optional<std::string>& plugin_conf_arg
  , std::vector<std::string>& pathsToExtraPluginFiles)
{
  return
    options
    // -- option --
    (
    cmd::kPluginsDir
    , po::value(&plugin_dir_arg)
      ->default_value(boost::none, "")
    , "path to directory with plugins"
    )
    // -- option --
    (
    cmd::kPluginsConfFile
    , po::value(&plugin_conf_arg)
      ->default_value(boost::none, "")
    , "path to plugins configuration file"
    )
    // -- option --
    (
    cmd::kExtraPluginFiles
    , po::value(&pathsToExtraPluginFiles)
      ->multitoken()
    , "paths to plugin files that "
    "must be loaded independently from"
    " plugins configuration file"
    );
}

[[nodiscard]] /* do not ignore return value */
static
AppCmdOptions::options_init&
registerClingOptions(
  AppCmdOptions::options_init& options
  , std::vector<std::string>& cling_scripts)
{
  return
    options
    // -- option --
    (
    cmd::kScriptFiles
    , po::value(&cling_scripts)
      ->multitoken()
    ,
    "list of C++ files what need to be"
    " loaded by Cling interpreter"
    );
}

[[nodiscard]] /* do not ignore return value */
static
AppCmdOptions::options_init&
registerTracingOptions(
  AppCmdOptions::options_init& options
  , boost::optional<std::string>& tracing_categories_arg)
{
  return
    options
    // -- option --
    (
    cmd::kTracingCategories
    , po::value(&tracing_categories_arg)
      ->default_value(boost::none, "")
    , "start tracing at application start"
    " you usually don't want to write big trace file,"
    " so trace only required period of time"
    );
}

[[nodiscard]] /* do not ignore return value */
static
AppCmdOptions::options_init&
registerToolOptions(
  AppCmdOptions::options_init& options
  , boost::optional<std::string>& outdir_arg
  , boost::optional<std::string>& srcdir_arg
  , boost::optional<int>& thread_num_arg)
{
  return
    options
    // -- option --
    (
    cmd::kOutDir
    , po::value(&outdir_arg)
      ->default_value(boost::none, "")
    ,
    "change output directory path"
    " (where to place generated files)"
    )
    // -- option --
    (
    cmd::kInDir
    , po::value(&srcdir_arg)
      ->default_value(boost::none, "")
    ,
    "change current working directory path"
    " (path to template files)"
    )
    // -- option --
    (
    cmd::kThreadNum
    , po::value(&thread_num_arg)
      ->default_value(boost::none, "")
    , "number of threads"
    );
}

[[nodiscard]] /* do not ignore return value */
static
AppCmdOptions::options_init&
registerBaseSwitches(
  AppCmdOptions::options_init& options)
{
  /// \note we need to re-register base switches
  /// cause boost do not allow unknown options
  // BASE SWITCHES, see base/base_switches.h
  // redirect some switches from base lib
  // to boost::program_options
  return
    options
    // -- option --
    (
    switches::kDisableBestEffortTasks
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kDisableBreakpad
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kDisableFeatures
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kDisableLowEndDeviceMode
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kEnableCrashReporter
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kEnableFeatures
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kEnableLowEndDeviceMode
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kForceFieldTrials
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kFullMemoryCrashReport
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kNoErrorDialogs
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kProfilingAtStart
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kProfilingFile
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kProfilingFlush
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kTestChildProcess
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kTestDoNotInitializeIcu
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kTraceToFile
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kTraceToFileName
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kV
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kVModule
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kWaitForDebugger
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
#if defined(OS_WIN)
    (
    switches::kDisableUsbKeyboardDetect
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kDisableUsbKeyboardDetect
    , po::value<std::string>()
    , "switch provided by base lib"
    )
#endif
    // -- option --
#if defined(OS_LINUX) && !defined(OS_CHROMEOS)
    (
    switches::kDisableDevShmUsage
    , po::value<std::string>()
    , "switch provided by base lib"
    )
#endif
    // -- option --
#if defined(OS_POSIX)
    (
    switches::kEnableCrashReporterForTesting
    , po::value<std::string>()
    , "switch provided by base lib"
    )
#endif
    // -- option --
#if defined(OS_ANDROID)
    (
    switches::kEnableReachedCodeProfiler
    , po::value<std::string>()
    , "switch provided by base lib"
    )
    // -- option --
    (
    switches::kOrderfileMemoryOptimization
    , po::value<std::string>()
    , "switch provided by base lib"
    )
#endif
    // -- option --
#if defined(OS_LINUX)
    (
    switches::kEnableThreadInstructionCount
    , po::value<std::string>()
    , "switch provided by base lib"
    )
#endif
  ;
}

} // namespace

// static
boost::optional<std::string>
AppCmdOptions::CmdOptions::tracing_categories_arg{};

// static
boost::optional<std::string>
AppCmdOptions::CmdOptions::srcdir_arg{};

// static
boost::optional<std::string>
AppCmdOptions::CmdOptions::outdir_arg{};

// static
boost::optional<std::string>
AppCmdOptions::CmdOptions::plugin_dir_arg{};

// static
boost::optional<std::string>
AppCmdOptions::CmdOptions::plugin_conf_arg{};

// static
boost::optional<int>
AppCmdOptions::CmdOptions::thread_num_arg{};

// static
std::vector<std::string>
AppCmdOptions::CmdOptions::cling_scripts{};

// static
std::vector<std::string>
AppCmdOptions::CmdOptions::pathsToExtraPluginFiles{};

AppCmdOptions::AppCmdOptions(BoostCmdParser& boostCmdParser)
  : boostCmdParser_(boostCmdParser)
{
  DETACH_FROM_SEQUENCE(sequence_checker_);

  if (!base::PathService::Get(base::DIR_EXE, &dir_exe_)) {
    NOTREACHED();
  }
}

AppCmdOptions::~AppCmdOptions()
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
}

std::vector<base::FilePath>
AppCmdOptions::pathsToExtraPluginFiles()
{
  return
    basis::toFilePaths(cmdOptions_.pathsToExtraPluginFiles);
}

std::string AppCmdOptions::tracingCategories()
{
  return
    cmdOptions_.tracing_categories_arg
      .value_or(basis::DEFAULT_EVENT_CATEGORIES);
}

size_t AppCmdOptions::count(
  const base::StringPiece& key)
{
  return
    boostCmdParser_.count(key);
}

std::vector<base::FilePath> AppCmdOptions::scriptFiles()
{
  return
    basis::toFilePaths(cmdOptions_.cling_scripts);
}

base::FilePath AppCmdOptions::inDir()
{
  base::FilePath dir
    = basis::cmdKeyToDirectory(cmd::kInDir, boostCmdParser_);
  return
    dir.empty()
    // default value
    ? dir_exe_
    : dir;
}

base::FilePath AppCmdOptions::outDir()
{
  base::FilePath dir
    = basis::cmdKeyToDirectory(cmd::kOutDir, boostCmdParser_);
  return
    dir.empty()
    // default value
    ? dir_exe_
    : dir;
}

base::FilePath AppCmdOptions::pluginsDir()
{
  base::FilePath dir
    = basis::cmdKeyToDirectory(cmd::kPluginsDir, boostCmdParser_);
  return
    dir.empty()
    // default value
    ? dir_exe_
      .AppendASCII(backend::kDefaultPluginsDirName)
    : dir;
}

base::FilePath AppCmdOptions::pluginsConfigFile()
{
  base::FilePath pluginsConfigFile
    = basis::cmdKeyToFile(cmd::kPluginsConfFile, boostCmdParser_);
  return
    pluginsConfigFile.empty()
    // default value
    ? dir_exe_
      .AppendASCII(kPluginsConfigFilesDir)
      .AppendASCII(backend::kPluginsConfigFileName)
    : pluginsConfigFile;
}

int AppCmdOptions::threadsNum()
{
  return
    std::clamp(
    cmdOptions_.thread_num_arg
      .get_value_or(kDefaultThreadNum)
    , kMinThreadNum
    , kMaxThreadNum
    );
}

AppCmdOptions::options_init
AppCmdOptions::registerOptions(
  AppCmdOptions::options_init& options)
{
  options
    = registerCommonOptions(options);

  options
    = registerPluginsOptions(options
                             , cmdOptions_.plugin_dir_arg
                             , cmdOptions_.plugin_conf_arg
                             , cmdOptions_.pathsToExtraPluginFiles);

  options
    = registerClingOptions(options
                           , cmdOptions_.cling_scripts);

  options
    = registerTracingOptions(options
                             , cmdOptions_.tracing_categories_arg);

  options
    = registerToolOptions(options
                          , cmdOptions_.outdir_arg
                          , cmdOptions_.srcdir_arg
                          , cmdOptions_.thread_num_arg);

  options
    = registerBaseSwitches(options);

  return
    options;
}

} // namespace cmd
