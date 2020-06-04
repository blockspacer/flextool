#pragma once

#include "flextool/command_line_constants.hpp"
#include "flextool/boost_command_line.hpp"

#include <base/macros.h>
#include <base/sequenced_task_runner.h>
#include <base/trace_event/trace_event.h>
#include <base/files/file_path.h>
#include <base/files/file_util.h>
#include <base/stl_util.h>
#include <base/bind.h>
#include <base/path_service.h>

#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/program_options.hpp>
#include <boost/utility/in_place_factory.hpp>

#include <vector>
#include <string>

namespace cmd {

constexpr int kDefaultThreadNum = 2;

constexpr int kMinThreadNum = 1;

constexpr int kMaxThreadNum = 100;

extern const char kDefaultPluginsDir[];

extern const char kDefaultPluginsConfig[];

//extern const char kHelpArg[];

//extern const char kVersionArg[];

// used to convert unprocessed command-line arguments to
// internal data types (like std::string to base::FilePath)
// also can contain application-specific logic
// (like clamp command-line argument between some values)
class AppCmdOptions {
public:
  // Unprocessed command-line arguments, usually std::string.
  // Use |AppCmdOptions| to convert std::string to base::FilePath etc.
  struct BoostCmdOptions {
    /// \todo use it
    //static boost::optional<std::string> log_config;

    static boost::optional<std::string> tracing_categories_arg;

    static boost::optional<std::string> srcdir_arg;

    static boost::optional<std::string> outdir_arg;

    static boost::optional<std::string> plugin_dir_arg;

    static boost::optional<std::string> plugin_conf_arg;

    /// \todo use multithreaded clang libtooling
    static boost::optional<int> thread_num_arg;

    static std::vector<std::string> cling_scripts;

    /// \todo use it
    //static std::vector<std::string> ctp_scripts_search_paths;

    static std::vector<std::string> pathsToExtraPluginFiles;
  };

  AppCmdOptions(BoostCmd& boostCmd);

  ~AppCmdOptions();

  int threadsNum();

  base::FilePath inDir();

  // outdir can contain `compile_commands.json`.
  // To create compile_commands.json
  // set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  // or
  // set_target_properties(... ENABLE_EXPORTS 1)
  // see https://releases.llvm.org/9.0.0/tools/clang/docs/LibTooling.html
  base::FilePath outDir();

  base::FilePath pluginsDir();

  base::FilePath pluginsConfigFile();

  std::vector<base::FilePath> scriptFiles();

  std::vector<base::FilePath> pathsToExtraPluginFiles();

  std::string tracingCategories();

  bool hasAutoStartTracer();

  bool hasHelp();

  bool hasVersion();

  // converts command-line argument to ABSOLUTE path
  /// \note returns empty base::FilePath{} if path
  /// is NOT valid directory
  base::FilePath cmdKeyToDirectory(
    const char key[]);

  // converts command-line argument to ABSOLUTE path
  /// \note returns empty base::FilePath{} if path
  /// is NOT valid file
  base::FilePath cmdKeyToFile(
    const char key[]);

  /// \note returns std::numeric_limits<int>::max() if
  /// command-line argument is NOT specified or NOT convertable
  /// to int
  int cmdKeyToInt(
    const base::StringPiece& key);

  // converts command-line argument to path
  // path may be NOT absolute
  /// \note returns empty base::FilePath{} if command-line argument
  /// is NOT specified or NOT valid
  base::FilePath getAsPath(
    const base::StringPiece& key);

  boost::program_options::options_description_easy_init
    registerOptions(
      boost::program_options::options_description_easy_init& options);

private:
  BoostCmd& boostCmd_;

  BoostCmdOptions boostCmdOptions{};

  // we expect that |dir_exe_| will not be changed
  base::FilePath dir_exe_;

  SEQUENCE_CHECKER(sequence_checker_);

  DISALLOW_COPY_AND_ASSIGN(AppCmdOptions);
};

} // namespace cmd
