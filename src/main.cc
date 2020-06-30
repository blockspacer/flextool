#include "flextool/cmd_environment.hpp"
#include "flextool/plugin_environment.hpp"
#include "flextool/clang_environment.hpp"
#include "flextool/cling_environment.hpp"
#include "flextool/clang_tool.hpp"
#include "flextool/version.hpp"
#include "flextool/command_line_constants.hpp"

#include <flexlib/ToolPlugin.hpp>
#include <flexlib/clangPipeline.hpp>

#include <base/logging.h>
#include <base/threading/thread_restrictions.h>
#include <base/memory/scoped_refptr.h>
#include <base/optional.h>

#include <basis/base_environment.hpp>

#include <stdlib.h>
#include <ostream>
#include <string>
#include <vector>
#include <thread>
#include <map>

namespace {

static const base::FilePath::CharType kIcuDataFileName[]
  = FILE_PATH_LITERAL(R"raw(./resources/icu/optimal/icudt64l.dat)raw");

static const base::FilePath::CharType kTraceReportFileName[]
  = FILE_PATH_LITERAL(R"raw(trace_report.json)raw");

const char kStringCommandDelim[]
  = "/";

void runClangLibTooling(
  flextool::ScopedClangEnvironment& clang_env)
{
  flextool::ClangTool tool;

  DCHECK(!clang_env.args_storage.empty());
  DCHECK(clang_env.annotationMatchOptions);
  DCHECK(!clang_env.annotationMatchOptions->annotateName.empty());
  DCHECK(clang_env.annotationMatchOptions->annotationMatchCallback);
  DCHECK(clang_env.annotationMatchOptions->endSourceFileAction);

  tool.run(
    clang_env.args_storage // command-line arguments
    , clang_env.annotationMatchOptions // custom settings
    );
}

// init application systems,
// initialization order matters!
[[nodiscard]] /* do not ignore return value */
base::Optional<int> initEnv(
  int argc
  , char* argv[]
  , flextool::ScopedCmdEnvironment& cmd_env
  , basis::ScopedBaseEnvironment& base_env
  , flextool::ScopedPluginEnvironment& plugin_env
  , flextool::ScopedClangEnvironment& clang_env
  , flextool::ScopedClingEnvironment& cling_env
  )
{
  // ScopedCmdEnvironment
  {
    const bool envCreated
      = cmd_env.init(argc, argv);
    if(!envCreated) {
      LOG(ERROR)
        << "Unable to create command-line environment";
      return
        EXIT_FAILURE;
    }
  }

  // ScopedBaseEnvironment
  {
    const bool envCreated
      = base_env.init(
          argc
          , argv
          , cmd_env.appCmd.count(cmd::kAutoStartTracer)
          , cmd_env.appCmd.tracingCategories()
          , cmd_env.appCmd.outDir()
          , kIcuDataFileName
          , kTraceReportFileName
          , cmd_env.appCmd.threadsNum()
          );
    if(!envCreated) {
      LOG(ERROR)
        << "Unable to create application environment";
      return
        EXIT_FAILURE;
    }
  }

  // ScopedPluginEnvironment
  {
    const bool envCreated
      = plugin_env.init(
          argc
          , argv
          , cmd_env.appCmd.pluginsDir()
          , cmd_env.appCmd.pluginsConfigFile()
          , cmd_env.appCmd.pathsToExtraPluginFiles()
          , cmd_env.appCmd.outDir());
    if(!envCreated) {
      LOG(ERROR)
        << "Unable to create application environment";
      return
        EXIT_FAILURE;
    }
  }

  // ScopedClangEnvironment
  {
    {
      const bool envCreated
        = clang_env.init(cmd_env);
      if(!envCreated) {
        LOG(ERROR)
          << "Unable to create clang environment";
        return
          EXIT_FAILURE;
      }
    }
  }

  // ScopedClingEnvironment
  {
    {
      const bool envCreated
        = cling_env.init(
            cmd_env.appCmd.scriptFiles()
            , clang_env.cling_extra_args);
      if(!envCreated) {
        LOG(ERROR)
          << "Unable to create clang environment";
        return
          EXIT_FAILURE;
      }
    }

    /// \note allow plugins to process commands before
    /// pre-built logic
    {
      VLOG(9)
        << "creating"
        " plugin::ToolPlugin::Events"
        "::RegisterClingInterpreter";
      DCHECK(cling_env.clingInterpreter);
      DCHECK(plugin_env.main_events_dispatcher);

      using RegisterClingInterpreter
        = plugin::ToolPlugin::Events::RegisterClingInterpreter;

      RegisterClingInterpreter eventData{
        cling_env.clingInterpreter.get()
      };

      plugin_env.main_events_dispatcher
        ->trigger<RegisterClingInterpreter>(
        std::move(eventData)
        );
    }
  }

  // continue setup of ScopedClingEnvironment,
  // depends on Cling
  {
    DCHECK(cling_env.clingInterpreter);

    /// \note allow plugins to process commands before
    /// pre-built logic
    {
      VLOG(9)
        << "sending"
        " plugin::ToolPlugin::Events"
        "::RegisterAnnotationMethods";
      DCHECK(clang_env.annotationMethods);
      DCHECK(clang_env.sourceTransformPipeline);

      using RegisterAnnotationMethods
        = plugin::ToolPlugin::Events::RegisterAnnotationMethods;

      RegisterAnnotationMethods eventData{
        clang_env.annotationMethods.get()
        , clang_env.sourceTransformPipeline.get()
      };

      plugin_env.main_events_dispatcher->trigger<
        RegisterAnnotationMethods>(
        std::move(eventData)
        );
    }
  }

  return base::nullopt;
}

// handle command-line argument: `--version`
[[nodiscard]] /* do not ignore return value */
base::Optional<int> handleVersionCmd(
  flextool::ScopedCmdEnvironment& cmd_env
  , flextool::ScopedPluginEnvironment& plugin_env)
{
  DCHECK(cmd_env.appCmd.count(cmd::kVersion));

  LOG(INFO)
    << "tool version: "
    << flextool_VERSION;

  /// \note allow plugins to process commands before
  /// pre-built logic
  {
    DCHECK(plugin_env.main_events_dispatcher);
    VLOG(9)
      << "sending"
      " plugin::ToolPlugin::Events"
      "::StringCommand";

    using StringCommand
      = plugin::ToolPlugin::Events::StringCommand;

    StringCommand eventData{
      // raw_cmd
      /*copy*/
      std::string(kStringCommandDelim) + cmd::kVersion
      , // split_parts
      /*copy*/
      std::vector<std::string>{cmd::kVersion}
    };

    plugin_env.main_events_dispatcher
      ->trigger<StringCommand>(
      std::move(eventData)
      );
  }

  // nothing after version printing
  // stop app execution with EXIT_SUCCESS
  return
    EXIT_SUCCESS;
}

// handle command-line argument: `--help`
[[nodiscard]] /* do not ignore return value */
base::Optional<int> handleHelpCmd(
  flextool::ScopedCmdEnvironment& cmd_env
  , flextool::ScopedPluginEnvironment& plugin_env
  , flextool::ScopedClangEnvironment& clang_env)
{
  LOG(INFO) << cmd_env.boostCmdParser.optionsToString();

  /// \note allow plugins to process commands before
  /// pre-built logic
  {
    DCHECK(plugin_env.main_events_dispatcher);
    VLOG(9)
      << "sending"
      " plugin::ServerPlugin::Events"
      "::StringCommand";

    using StringCommand
      = plugin::ToolPlugin::Events::StringCommand;

    StringCommand eventData{
      // raw_cmd
      /*copy*/
      std::string(kStringCommandDelim) + cmd::kVersion
      , // split_parts
      /*copy*/
      std::vector<std::string>{cmd::kVersion}
    };

    plugin_env.main_events_dispatcher
      ->trigger<StringCommand>(
      std::move(eventData)
      );

    /// \note continue to forward help to clang libtooling
    runClangLibTooling(clang_env);
  }

  // nothing after version printing
  // stop app execution with EXIT_SUCCESS
  return
    EXIT_SUCCESS;
}

// handle command-line arguments, such as `--version`
[[nodiscard]] /* do not ignore return value */
base::Optional<int> handleCmdOptions(
  flextool::ScopedCmdEnvironment& cmd_env
  , flextool::ScopedPluginEnvironment& plugin_env
  , flextool::ScopedClangEnvironment& clang_env
  )
{
  if (cmd_env.appCmd.count(cmd::kVersion))
  {
    DVLOG(9)
        << "processing command-line argument: "
        << cmd::kVersion;
    return handleVersionCmd(cmd_env,
      plugin_env);
  }

  if (cmd_env.appCmd.count(cmd::kHelp))
  {
    DVLOG(9)
        << "processing command-line argument: "
        << cmd::kHelp;
    return handleHelpCmd(cmd_env,
      plugin_env, clang_env);
  }

  return base::nullopt;
}

} // namespace

int main(int argc, char* argv[])
{
  DCHECK(argc > 1);

  // setup command-line arguments
  flextool::ScopedCmdEnvironment cmd_env;

  // setup basic requirements, like thread pool, logging, etc.
  basis::ScopedBaseEnvironment base_env;

  // setup ::Corrade::PluginManager
  flextool::ScopedPluginEnvironment plugin_env;

  // setup clang libtooling
  flextool::ScopedClangEnvironment clang_env;

  // setup cling C++ interpreter
  flextool::ScopedClingEnvironment cling_env;

  // init application systems,
  // initialization order matters!
  {
    base::Optional<int> exit_code = initEnv(
      argc
      , argv
      , cmd_env
      , base_env
      , plugin_env
      , clang_env
      , cling_env);
    if(exit_code.has_value()) {
      LOG(WARNING)
        << "exited during environment creation";
      return exit_code.value();
    }
  }

  // handle command-line arguments, such as `--version`
  {
    base::Optional<int> exit_code = handleCmdOptions(
      cmd_env
      , plugin_env
      , clang_env);
    if(exit_code.has_value()) {
      DVLOG(9)
        << "exited after handling of"
        " command-line arguments";
      return exit_code.value();
    }
  }

  runClangLibTooling(clang_env);

  // during teardown we need to be able to perform IO.
  base::ThreadRestrictions::SetIOAllowed(true);

  /// \note it is safe to destroy
  /// ScopedBaseEnvironment, Scoped*Environment, etc.
  /// in any order after application finished
  LOG(INFO)
    << "performing shutdown...";

  return
    EXIT_SUCCESS;
}
