#include "flextool/base_environment.hpp"
#include "flextool/cmd_environment.hpp"
#include "flextool/plugin_environment.hpp"
#include "flextool/clang_environment.hpp"
#include "flextool/cling_environment.hpp"
#include "flextool/clang_tool.hpp"
#include "flextool/version.hpp"

#include <flexlib/ToolPlugin.hpp>
#include <flexlib/clangPipeline.hpp>

#include <base/logging.h>
#include <base/memory/scoped_refptr.h>
#include <base/optional.h>

#include <stdlib.h>
#include <ostream>
#include <string>
#include <vector>
#include <thread>
#include <map>

namespace {

const char kStringCommandDelim[]
  = "/";

const char kVersionStringCommand[]
  = "version";

// init application systems,
// initialization order matters!
base::Optional<int> initEnv(
  int argc
  , char* argv[]
  , flextool::ScopedCmdEnvironment& cmd_env
  , flextool::ScopedBaseEnvironment& base_env
  , flextool::ScopedPluginEnvironment& plugin_env
  , flextool::ScopedClangEnvironment& clang_env
  , flextool::ScopedClingEnvironment& cling_env
  )
{
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

  {
    const bool envCreated
      = base_env.init(
          argc
          , argv
          , cmd_env.appCmd.hasAutoStartTracer()
          , cmd_env.appCmd.tracingCategories()
          , cmd_env.appCmd.outDir()
          , cmd_env.appCmd.threadsNum()
          );
    if(!envCreated) {
      LOG(ERROR)
        << "Unable to create application environment";
      return
        EXIT_FAILURE;
    }
  }

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

  // clang env
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

    /// \note allow plugins to process commands before pre-built logic
    {
      VLOG(9)
        << "sending"
        " plugin::ToolPlugin::Events::RegisterAnnotationMethods";
      DCHECK(clang_env.annotationMethods);
      DCHECK(clang_env.sourceTransformPipeline);
      plugin_env.main_events_dispatcher->trigger<
        plugin::ToolPlugin::Events::RegisterAnnotationMethods>(
        plugin::ToolPlugin::Events::RegisterAnnotationMethods{
          .annotationMethods
            = clang_env.annotationMethods.get()
            , .sourceTransformPipeline
            = clang_env.sourceTransformPipeline.get()
        }
        );
    }
  }

  // cling env
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

    /// \note allow plugins to process commands before pre-built logic
    {
      VLOG(9)
        << "creating"
        " plugin::ToolPlugin::Events::RegisterClingInterpreter";
      DCHECK(cling_env.clingInterpreter);
      DCHECK(plugin_env.main_events_dispatcher);
      plugin_env.main_events_dispatcher->trigger<
        plugin::ToolPlugin::Events::RegisterClingInterpreter>(
        plugin::ToolPlugin::Events::RegisterClingInterpreter{
          .clingInterpreter
            = cling_env.clingInterpreter.get()
        }
        );
    }
  }

  return base::nullopt;
}

// handle command-line arguments, such as `--version`
base::Optional<int> handleCmd(
  int argc
  , char* argv[]
  , flextool::ScopedCmdEnvironment& cmd_env
  , flextool::ScopedBaseEnvironment& base_env
  , flextool::ScopedPluginEnvironment& plugin_env
  )
{
  if (cmd_env.appCmd.hasVersion())
  {
    LOG(INFO)
        << "tool version: "
        << flextool_VERSION;

    /// \note allow plugins to process commands before pre-built logic
    {
      DCHECK(plugin_env.main_events_dispatcher);
      /// \todo remove plugin::ToolPlugin::Events::StringCommand
      plugin_env.main_events_dispatcher->trigger<
        plugin::ToolPlugin::Events::StringCommand>(
        plugin::ToolPlugin::Events::StringCommand{
          .raw_cmd =
            /*copy*/
            std::string(kStringCommandDelim) + kVersionStringCommand
          , .split_parts =
            /*copy*/
            std::vector<std::string>{kVersionStringCommand}
        }
        );
    }

    // nothing after version printing
    // stop app execution with EXIT_SUCCESS
    return
      EXIT_SUCCESS;
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
  flextool::ScopedBaseEnvironment base_env;

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
      return exit_code.value();
    }
  }

  // handle command-line arguments, such as `--version`
  {
    base::Optional<int> exit_code = handleCmd(
      argc
      , argv
      , cmd_env
      , base_env
      , plugin_env);
    if(exit_code.has_value()) {
      return exit_code.value();
    }
  }

  // run Clang LibTooling
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

  /// \note it is safe to destroy
  /// ScopedBaseEnvironment, ScopedClingEnvironment, etc.
  /// in any order after |ClangTool.run| finished
  return
    EXIT_SUCCESS;
}
