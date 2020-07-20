#include "flextool/plugin_environment.hpp" // IWYU pragma: associated

#include <base/files/file_path.h>
#include <base/base_paths.h>
#include <base/files/file_util.h>
#include <base/logging.h>
#include <base/message_loop/message_loop.h>
#include <base/message_loop/message_loop_current.h>
#include <base/path_service.h>
#include <base/sequence_checker.h>

#include <entt/signal/dispatcher.hpp>

#include <Corrade/PluginManager/AbstractManager.h>

#include <flexlib/ToolPlugin.hpp>

#include <basis/PluginManager.hpp>

#include <sstream>
#include <algorithm>

namespace flextool {

ScopedPluginEnvironment::ScopedPluginEnvironment()
  : main_events_dispatcher(std::make_unique<entt::dispatcher>())
{
  DCHECK(base::MessageLoop::current()->task_runner());

  DETACH_FROM_SEQUENCE(sequence_checker_);
}

/// \note destroy dispatcher before plugin manager
/// see https://github.com/skypjack/entt/issues/103
ScopedPluginEnvironment::~ScopedPluginEnvironment()
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  DCHECK(main_events_dispatcher);
  VLOG(9)
    << "PluginManager shutdown...";

  using Shutdown
    = backend::PluginManagerEvents::Shutdown;

  main_events_dispatcher->trigger<Shutdown>();
}

/// \todo refactor long method
bool ScopedPluginEnvironment::init(
  int argc
  , char* argv[]
  , const base::FilePath& pathToDirWithPlugins
  , const base::FilePath& pathToPluginsConfFile
  , const std::vector<base::FilePath>& pathsToExtraPluginFiles
  , const base::FilePath& outDir)
{
  DCHECK(argc > 0);

  if (!base::PathService::Get(base::DIR_EXE, &dir_exe_)) {
    NOTREACHED();
    // stop app execution with EXIT_FAILURE
    return
      false;
  }

  {
    DCHECK(main_events_dispatcher);
    plug_mgr.connect_to_dispatcher(*main_events_dispatcher);
  }

  {
    VLOG(9)
      << "creating"
      " plugin::ToolPlugin::Events::Init";
    DCHECK(main_events_dispatcher);

    using Init
      = plugin::ToolPlugin::Events::Init;

    Init eventData{
      argc
      , argv
    };

    main_events_dispatcher->trigger<Init>(
      std::move(eventData)
      );
  }

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
    DCHECK(!pathToDirWithPlugins.empty());
    // must have default value
    DCHECK(!pathToPluginsConfFile.empty());

    using Startup
      = backend::PluginManagerEvents::Startup;

    Startup eventData{
      pathToDirWithPlugins
      , pathToPluginsConfFile
      , pathsToExtraPluginFiles
    };

    main_events_dispatcher->trigger<
      Startup
      >(std::move(eventData));
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
        << pathToDirWithPlugins;
  }

  {
    VLOG(9)
      << "running"
      " plug_mgr.connect_plugins_to_dispatcher";
    DCHECK(main_events_dispatcher);
    plug_mgr.connect_plugins_to_dispatcher(
      *main_events_dispatcher);
  }

  return
    true;
}

} // flextool
