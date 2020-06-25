#pragma once

#include <base/files/file_path.h>
#include <base/macros.h>
#include <base/sequence_checker.h>
#include <base/strings/string_piece_forward.h>

#include <boost/program_options/options_description.hpp>

#include <string>
#include <vector>

namespace boost {
template <class T> class optional;
} // namespace boost

namespace cmd {
class BoostCmdParser;
} // namespace cmd

namespace cmd {

constexpr int kDefaultThreadNum = 2;

constexpr int kMinThreadNum = 1;

constexpr int kMaxThreadNum = 100;

// used to convert unprocessed command-line arguments to
// internal data types (like std::string to base::FilePath)
// also can contain application-specific logic
// (like clamp command-line argument between some values)
class AppCmdOptions {
public:
  using options_init
    = boost::program_options::options_description_easy_init;

  // Unprocessed command-line arguments,
  // usually std::string.
  // Use |AppCmdOptions| to convert std::string
  // to base::FilePath etc.
  struct CmdOptions {
    static boost::optional<std::string> tracing_categories_arg;

    static boost::optional<std::string> srcdir_arg;

    static boost::optional<std::string> outdir_arg;

    static boost::optional<std::string> plugin_dir_arg;

    static boost::optional<std::string> plugin_conf_arg;

    /// \todo use multithreaded clang libtooling
    static boost::optional<int> thread_num_arg;

    static std::vector<std::string> cling_scripts;

    static std::vector<std::string> pathsToExtraPluginFiles;
  };

  AppCmdOptions(
    BoostCmdParser& BoostCmdParser);

  ~AppCmdOptions();

  [[nodiscard]] /* do not ignore return value */
  int threadsNum();

  [[nodiscard]] /* do not ignore return value */
  size_t count(
    const base::StringPiece& key);

  [[nodiscard]] /* do not ignore return value */
  base::FilePath inDir();

  // outdir can contain `compile_commands.json`.
  // To create compile_commands.json
  // set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  // or
  // set_target_properties(... ENABLE_EXPORTS 1)
  // see https://releases.llvm.org/9.0.0/tools/clang/docs/LibTooling.html
  [[nodiscard]] /* do not ignore return value */
  base::FilePath outDir();

  [[nodiscard]] /* do not ignore return value */
  base::FilePath pluginsDir();

  base::FilePath pluginsConfigFile();

  [[nodiscard]] /* do not ignore return value */
  std::vector<base::FilePath> scriptFiles();

  [[nodiscard]] /* do not ignore return value */
  std::vector<base::FilePath> pathsToExtraPluginFiles();

  [[nodiscard]] /* do not ignore return value */
  std::string tracingCategories();

  [[nodiscard]] /* do not ignore return value */
  options_init
  registerOptions(
    options_init& options);

private:
  BoostCmdParser& boostCmdParser_;

  CmdOptions cmdOptions_{};

  // we expect that |dir_exe_| will not be changed
  base::FilePath dir_exe_;

  SEQUENCE_CHECKER(sequence_checker_);

  DISALLOW_COPY_AND_ASSIGN(AppCmdOptions);
};

} // namespace cmd
