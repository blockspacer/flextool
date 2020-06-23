#include "flextool/app_cmd_options.hpp" // IWYU pragma: associated

#include "flextool/command_line_constants.hpp"

#include <flexlib/ToolPlugin.hpp>

#include <base/base_paths.h>
#include <base/base_switches.h>
#include <base/files/file.h>
#include <base/files/file_util.h>
#include <base/logging.h>
#include <base/path_service.h>
#include <base/strings/string_piece.h>

#include <build/build_config.h>

#include <basis/PluginManager.hpp>
#include <basis/boost_command_line.hpp>

#include <boost/none.hpp>
#include <boost/optional/optional.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/program_options/value_semantic.hpp>

#include <algorithm>
#include <limits>
#include <ostream>

#define LOG_PATH_NOT_EXIST(severity, path) \
  LOG(severity) \
    << "path must exist: " \
    << path;

#define LOG_PATH_NOT_DIRECTORY(severity, path) \
  LOG(severity) \
    << "path must be directory: " \
    << path;

#define LOG_PATH_MUST_BE_NOT_DIRECTORY(severity, path) \
  LOG(severity) \
    << "path must be NOT directory: " \
    << path;

#define VLOG_NOT_INITIALIZED(severity, key) \
  VLOG(severity) \
    << "command like argument " \
    << key \
    << " is not initialized";

namespace po = boost::program_options;

namespace cmd {

namespace {

static const char DEFAULT_EVENT_CATEGORIES[]
  = "-sequence_manager"
    ",-thread_pool"
    ",-base"
    ",-toplevel"
    ",profiler"
    ",user_timing"
    ",ui"
    ",browser"
    ",latency"
    ",latencyInfo"
    ",loading"
    ",skia"
    ",task_scheduler"
    ",native"
    ",benchmark"
    ",ipc"
    ",mojom"
    ",media"
    ",disabled-by-default-lifecycles"
    ",disabled-by-default-renderer.scheduler"
    ",disabled-by-default-v8.gc"
    ",disabled-by-default-blink_gc"
    ",disabled-by-default-system_stats"
    ",disabled-by-default-network"
    ",disabled-by-default-cpu_profiler"
    ",disabled-by-default-memory-infra";

[[nodiscard]] /* do not ignore return value */
static
std::vector<base::FilePath>
toFilePaths(const std::vector<std::string>& paths)
{
  std::vector<base::FilePath> result;
  for (const std::string& it: paths)
  {
    result.push_back(
      /// \note On POSIX, |MakeAbsoluteFilePath| fails
      /// if the path does not exist
      base::MakeAbsoluteFilePath(base::FilePath{it}));
  }
  return
    result;
}

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
    toFilePaths(cmdOptions_.pathsToExtraPluginFiles);
}

std::string AppCmdOptions::tracingCategories()
{
  return
    cmdOptions_.tracing_categories_arg
      .value_or(DEFAULT_EVENT_CATEGORIES);
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
    toFilePaths(cmdOptions_.cling_scripts);
}

base::FilePath AppCmdOptions::inDir()
{
  base::FilePath dir
    = cmdKeyToDirectory(cmd::kInDir);
  return
    dir.empty()
    // default value
    ? dir_exe_
    : dir;
}

base::FilePath AppCmdOptions::outDir()
{
  base::FilePath dir
    = cmdKeyToDirectory(cmd::kOutDir);
  return
    dir.empty()
    // default value
    ? dir_exe_
    : dir;
}

base::FilePath AppCmdOptions::pluginsDir()
{
  base::FilePath dir
    = cmdKeyToDirectory(cmd::kPluginsDir);
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
    = cmdKeyToFile(cmd::kPluginsConfFile);
  return
    pluginsConfigFile.empty()
    // default value
    ? dir_exe_
      .AppendASCII(backend::kDefaultPluginsDirName)
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

int AppCmdOptions::cmdKeyToInt(
  const base::StringPiece &key)
{
  CHECK(!key.empty());

  int result = std::numeric_limits<int>::max();

  CHECK(key.find_first_of(',')
        == base::StringPiece::npos);
  if(!boostCmdParser_.count(key)) {
    VLOG(9)
        << "Unable to find command-line argument: "
        << key;
    return
      result;
  }

  const boost::optional<int>& value
    = boostCmdParser_.getAs<
    boost::optional<int>
    >(key.as_string());

  if(value.is_initialized()) {
    result = value.value();
  } else {
    VLOG_NOT_INITIALIZED(9, key)
  }

  return
    result;
}

base::FilePath AppCmdOptions::getAsPath(
  const base::StringPiece &key)
{
  CHECK(!key.empty());

  base::FilePath result{};

  CHECK(key.find_first_of(',')
        == base::StringPiece::npos);
  if(!boostCmdParser_.count(key)) {
    VLOG(9)
        << "Unable to find command-line argument: "
        << key;
    return
      result;
  }

  const boost::optional<std::string>& value
    = boostCmdParser_.getAs<
    boost::optional<std::string>
    >(key.as_string());

  if(value.is_initialized()
     && !value.value().empty())
  {
    result = base::FilePath{value.value()};
  } else {
    VLOG_NOT_INITIALIZED(9, key)
  }

  return
    result;
}

base::FilePath AppCmdOptions::cmdKeyToDirectory(
  const char key[])
{
  base::FilePath dirPath
    = getAsPath(key);

  VLOG(9)
    << key
    << " equals to "
    << dirPath;

  if(dirPath.empty()) {
    return
      base::FilePath{};
  }

  /// \note On POSIX, |MakeAbsoluteFilePath| fails
  /// if the path does not exist
  dirPath
    = base::MakeAbsoluteFilePath(dirPath);
  DCHECK(!dirPath.empty())
    << "unable to find absolute path to "
    << dirPath;

  if (!base::PathExists(dirPath)) {
    LOG_PATH_NOT_EXIST(WARNING, dirPath)
    return
      base::FilePath{};
  }

  // we expect dir, NOT file
  if(!base::DirectoryExists(dirPath)) {
    LOG_PATH_NOT_DIRECTORY(WARNING, dirPath)
    return
      base::FilePath{};
  }

  return
    dirPath;
}

base::FilePath AppCmdOptions::cmdKeyToFile(
  const char key[])
{
  base::FilePath filePath
    = getAsPath(key);

  VLOG(9)
    << key
    << " equals to "
    << filePath;

  if(filePath.empty()) {
    return
      base::FilePath{};
  }

  /// \note On POSIX, |MakeAbsoluteFilePath| fails
  /// if the path does not exist
  filePath = base::MakeAbsoluteFilePath(filePath);
  DCHECK(!filePath.empty())
    << "unable to find absolute path to "
    << filePath;

  if (!base::PathExists(filePath)) {
    LOG_PATH_NOT_EXIST(WARNING, filePath)
    return
      base::FilePath{};
  }

  // we expect file, NOT dir
  if (base::DirectoryExists(filePath)) {
    LOG_PATH_MUST_BE_NOT_DIRECTORY(WARNING, filePath)
    return
      base::FilePath{};
  }

  base::File::Info fileInfo;
  bool hasInfo
    = base::GetFileInfo(filePath, &fileInfo);
  if(!hasInfo) {
    LOG(WARNING)
        << "unable to get source file information: "
        << filePath;
    return
      base::FilePath{};
  }

  return
    filePath;
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
