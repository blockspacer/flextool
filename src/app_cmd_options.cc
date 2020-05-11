#include "flextool/app_cmd_options.hpp" // IWYU pragma: associated

#include <base/base_switches.h>

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

} // namespace

const char kDefaultPluginsDir[] = "plugins";

const char kDefaultPluginsConfig[] = "plugins.conf";

/// \todo
//const char kHelpArg[] = "help";

/// \todo
//const char kVersionArg[] = "version";

/// \todo
// static
//boost::optional<std::string>
//  AppCmdOptions::BoostCmdOptions::log_config{};

// static
boost::optional<std::string>
  AppCmdOptions::BoostCmdOptions::tracing_categories_arg{};

// static
boost::optional<std::string>
  AppCmdOptions::BoostCmdOptions::srcdir_arg{};

// static
boost::optional<std::string>
  AppCmdOptions::BoostCmdOptions::outdir_arg{};

// static
boost::optional<std::string>
  AppCmdOptions::BoostCmdOptions::plugin_dir_arg{};

// static
boost::optional<std::string>
  AppCmdOptions::BoostCmdOptions::plugin_conf_arg{};

// static
boost::optional<int>
  AppCmdOptions::BoostCmdOptions::thread_num_arg{};

// static
std::vector<std::string>
  AppCmdOptions::BoostCmdOptions::cling_scripts{};

// static
//std::vector<std::string>
//  AppCmdOptions::BoostCmdOptions::ctp_scripts_search_paths{};

// static
std::vector<std::string>
  AppCmdOptions::BoostCmdOptions::pathsToExtraPluginFiles{};

AppCmdOptions::AppCmdOptions(BoostCmd& boostCmd)
  : boostCmd_(boostCmd)
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

static std::vector<base::FilePath>
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
  return result;
}

std::vector<base::FilePath>
  AppCmdOptions::pathsToExtraPluginFiles()
{
  return toFilePaths(boostCmdOptions.pathsToExtraPluginFiles);
}
std::string AppCmdOptions::tracingCategories()
{
  return boostCmdOptions.tracing_categories_arg
    .value_or(DEFAULT_EVENT_CATEGORIES);
}

bool AppCmdOptions::hasAutoStartTracer()
{
  return boostCmd_.count(cmd::kAutoStartTracer);
}

bool AppCmdOptions::hasHelp()
{
  return boostCmd_.count(cmd::kHelp);
}

bool AppCmdOptions::hasVersion()
{
  return boostCmd_.count(cmd::kVersion);
}

std::vector<base::FilePath> AppCmdOptions::scriptFiles()
{
  return toFilePaths(boostCmdOptions.cling_scripts);
}

base::FilePath AppCmdOptions::inDir()
{
  base::FilePath dir
    = cmdKeyToDirectory(cmd::kInDir);
  return dir.empty()
    // default value
    ? dir_exe_
    : dir;
}

base::FilePath AppCmdOptions::outDir()
{
  base::FilePath dir
    = cmdKeyToDirectory(cmd::kOutDir);
  return dir.empty()
    // default value
    ? dir_exe_
    : dir;
}

base::FilePath AppCmdOptions::pluginsDir()
{
  base::FilePath dir
    = cmdKeyToDirectory(cmd::kPluginsDir);
  return dir.empty()
    // default value
    ? dir_exe_
      .AppendASCII(kDefaultPluginsDir)
    : dir;
}

base::FilePath AppCmdOptions::pluginsConfigFile()
{
  base::FilePath pluginsConfigFile
    = cmdKeyToFile(cmd::kPluginsConfFile);
  return pluginsConfigFile.empty()
    // default value
    ? dir_exe_
      .AppendASCII(kDefaultPluginsDir)
      .AppendASCII(kDefaultPluginsConfig)
    : pluginsConfigFile;
}

int AppCmdOptions::threadsNum()
{
  return std::clamp(
    boostCmdOptions.thread_num_arg.get_value_or(kDefaultThreadNum)
    , kMinThreadNum
    , kMaxThreadNum);
}

int AppCmdOptions::cmdKeyToInt(
  const base::StringPiece &key)
{
  CHECK(!key.empty());

  int result = std::numeric_limits<int>::max();

  CHECK(key.find_first_of(',')
        == base::StringPiece::npos);
  if(!boostCmd_.count(key)) {
    VLOG(9)
      << "Unable to find command-line argument: "
      << key;
    return result;
  }

  const boost::optional<int>& value
      = boostCmd_.getAs<
          boost::optional<int>
        >(key.as_string());

  if(value.is_initialized()) {
    result = value.value();
  } else {
    VLOG_NOT_INITIALIZED(9, key);
  }

  return result;
}

base::FilePath AppCmdOptions::getAsPath(
  const base::StringPiece &key)
{
  CHECK(!key.empty());

  base::FilePath result{};

  CHECK(key.find_first_of(',')
        == base::StringPiece::npos);
  if(!boostCmd_.count(key)) {
    VLOG(9)
      << "Unable to find command-line argument: "
      << key;
    return result;
  }

  const boost::optional<std::string>& value
      = boostCmd_.getAs<
          boost::optional<std::string>
        >(key.as_string());

  if(value.is_initialized() && !value.value().empty()) {
    result = base::FilePath{value.value()};
  } else {
    VLOG_NOT_INITIALIZED(9, key);
  }

  return result;
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
    return base::FilePath{};
  }

  /// \note On POSIX, |MakeAbsoluteFilePath| fails
  /// if the path does not exist
  dirPath = base::MakeAbsoluteFilePath(dirPath);
  DCHECK(!dirPath.empty())
    << "unable to find absolute path to "
    << dirPath;

  if (!base::PathExists(dirPath)) {
    LOG_PATH_NOT_EXIST(WARNING, dirPath);
    return base::FilePath{};
  }

  // we expect dir, NOT file
  if(!base::DirectoryExists(dirPath)) {
    LOG_PATH_NOT_DIRECTORY(WARNING, dirPath);
    return base::FilePath{};
  }

  return dirPath;
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
    return base::FilePath{};
  }

  /// \note On POSIX, |MakeAbsoluteFilePath| fails
  /// if the path does not exist
  filePath = base::MakeAbsoluteFilePath(filePath);
  DCHECK(!filePath.empty())
    << "unable to find absolute path to "
    << filePath;

  if (!base::PathExists(filePath)) {
    LOG_PATH_NOT_EXIST(WARNING, filePath);
    return base::FilePath{};
  }

  // we expect file, NOT dir
  if (base::DirectoryExists(filePath)) {
    LOG_PATH_MUST_BE_NOT_DIRECTORY(WARNING, filePath);
    return base::FilePath{};
  }

  base::File::Info fileInfo;
  bool hasInfo = base::GetFileInfo(filePath, &fileInfo);
  if(!hasInfo) {
    LOG(WARNING)
      << "unable to get source file information: "
      << filePath;
    return base::FilePath{};
  }

  return filePath;
}

///\todo allow per-plugin generation of cmake script with list of generated files
///\todo use cling to change file path to generated file
/// $exec(
///   generated(path = ./src/filename.cpp.gen);
/// )
///\todo support plugins that change default generator behavior (change .generated postfix in filename)
/// plugin must use flexlib file save pipeline (it is vector<pair<function>,datasource>)
/// other plugin can change flexlib file save pipeline (add .datetime postfix after .generated postfix)
///\todo support per-file metadata, pass to plugin load event
///\todo support in config file for ordered file path list with per-file rules (infile/outfile-may be same as infile or without prefix/json metadata)
///\todo allow plugins to hook into file parsing pipeline, have custom cmd args, change main loop to `watcher`
///\todo allow plugins to use `localstorage` to save arbitrary KV data. Remove $export
///\todo refactor typeclass
/// file parsing order must be set in conf file
/// plugin hooks in parsed event, filtered by pipeline rule "typeclass"
/// AFTER rule "typeclass" parsed, plugin can save libtooling output into member vars
/// AFTER rule "typeclass_instance" parsed, plugin can use libtooling output stored in member vars
/// if no data in member vars - error
///\todo allow plugins to use parser cache. Add plugin to cache libtooling matchresult with some KV storage
/// cache:
/// $(make_reflect;localCache("typeclassCache","key1;somedata;");after_make_reflect;localCache("typeclassCache","key2;somedata;"))
/// plugin may register cache observer with key "typeclassCache"
/// plugin may register cache observer for any key
boost::program_options::options_description_easy_init
  AppCmdOptions::registerOptions(
    boost::program_options::options_description_easy_init& options)
{
  return options
    (cmd::kHelp, "produce help message")
    (cmd::kVersion, "produce version message")
      /// \todo
    //(cmd::kClangIncludeDirs
    // , po::value(&boostCmdOptions.clang_includes_dir_arg)
    //     ->default_value(boost::none, "")
    // , "path to directory with clang includes")
      /// \todo
    //(cmd::kClingIncludeDirs
    // , po::value(&boostCmdOptions.cling_includes_dir_arg)
    //     ->default_value(boost::none, "")
    // , "path to directory with cling includes")
    (cmd::kPluginsDir
     , po::value(&boostCmdOptions.plugin_dir_arg)
         ->default_value(boost::none, "")
     , "path to directory with plugins")
    (cmd::kPluginsConfFile
     , po::value(&boostCmdOptions.plugin_conf_arg)
         ->default_value(boost::none, "")
     , "path to plugins configuration file")
    (cmd::kScriptFiles
     , po::value(&boostCmdOptions.cling_scripts)
         ->multitoken()
     , "list of C++ files what need to be loaded by Cling interpreter")
    /// \todo
    //(cmd::kScriptDirs
    // , po::value(&boostCmdOptions.ctp_scripts_search_paths)
    //     ->multitoken()
    // , "list of paths where toll will search for ctp_scripts subfolder")
    (cmd::kExtraPluginFiles
     , po::value(&boostCmdOptions.pathsToExtraPluginFiles)
         ->multitoken()
     , "paths to plugin files that "
       "must be loaded independently from plugins configuration file")
    (cmd::kOutDir
     , po::value(&boostCmdOptions.outdir_arg)
         ->default_value(boost::none, "")
     , "change output directory path (where to place generated files)")
    (cmd::kInDir
     , po::value(&boostCmdOptions.srcdir_arg)
         ->default_value(boost::none, "")
     , "change current working directory path (path to template files)")
    (cmd::kTracingCategories
     , po::value(&boostCmdOptions.tracing_categories_arg)
         ->default_value(boost::none, "")
     , "start tracing at application start"
       " you usually don't want to write big trace file,"
       " so trace only required period of time")
    (cmd::kThreadNum
     , po::value(&boostCmdOptions.thread_num_arg)
         ->default_value(boost::none, "")
     , "number of threads")
    /// \todo LOG arg
    //(log_arg_name, po::value(&log_config)->
    //  default_value(boost::none, ""), "log configuration")
    //
    // START BASE SWITCHES, see base/base_switches.h
    // redirect some switches from base lib to boost::program_options
    //
    (switches::kDisableBestEffortTasks
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kDisableBreakpad
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kDisableFeatures
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kDisableLowEndDeviceMode
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kEnableCrashReporter
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kEnableFeatures
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kEnableLowEndDeviceMode
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kForceFieldTrials
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kFullMemoryCrashReport
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kNoErrorDialogs
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kProfilingAtStart
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kProfilingFile
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kProfilingFlush
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kTestChildProcess
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kTestDoNotInitializeIcu
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kTraceToFile
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kTraceToFileName
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kV
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kVModule
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kWaitForDebugger
      , po::value<std::string>()
      , "switch provided by base lib")
#if defined(OS_WIN)
    (switches::kDisableUsbKeyboardDetect
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kDisableUsbKeyboardDetect
      , po::value<std::string>()
      , "switch provided by base lib")
#endif
#if defined(OS_LINUX) && !defined(OS_CHROMEOS)
    (switches::kDisableDevShmUsage
      , po::value<std::string>()
      , "switch provided by base lib")
#endif
#if defined(OS_POSIX)
    (switches::kEnableCrashReporterForTesting
      , po::value<std::string>()
      , "switch provided by base lib")
#endif
#if defined(OS_ANDROID)
    (switches::kEnableReachedCodeProfiler
      , po::value<std::string>()
      , "switch provided by base lib")
    (switches::kOrderfileMemoryOptimization
      , po::value<std::string>()
      , "switch provided by base lib")
#endif
#if defined(OS_LINUX)
    (switches::kEnableThreadInstructionCount
      , po::value<std::string>()
      , "switch provided by base lib")
#endif
    //
    // END BASE SWITCHES, see base/base_switches.h
    //
    ;
}

} // namespace cmd
