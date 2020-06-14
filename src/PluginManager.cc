#include "flextool/PluginManager.hpp" // IWYU pragma: associated

#include "flexlib/ToolPlugin.hpp"

#include <Corrade/PluginManager/AbstractManager.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Configuration.h>
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Corrade/Utility/Directory.h>

#include <base/files/file_path.h>
#include <base/files/file_util.h>
#include <base/logging.h>
#include <base/trace_event/trace_event.h>

#include <entt/signal/dispatcher.hpp>
#include <entt/signal/sigh.hpp>

#include <algorithm>
#include <initializer_list>
#include <ostream>
#include <utility>

#ifdef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
#error \
  "no CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT" \
  " for that platform"
#endif

namespace backend {

void PluginManager::connect_to_dispatcher(
  entt::dispatcher& events_dispatcher)
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  TRACE_EVENT0("toplevel", "PluginManager::connect_to_dispatcher()")

  events_dispatcher.sink<Events::Startup>()
    .connect<&PluginManager::startup>(this);
  events_dispatcher.sink<Events::Shutdown>()
    .connect<&PluginManager::shutdown>(this);
}

void PluginManager::connect_plugins_to_dispatcher(
  entt::dispatcher& events_dispatcher)
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  TRACE_EVENT0("toplevel", "PluginManager::connect_to_dispatcher()")

  for(PluginPtr& loaded_plugin : loaded_plugins_) {
    DCHECK(loaded_plugin);
    loaded_plugin->connect_to_dispatcher(events_dispatcher);
  }
}

/// \todo refactor long method
void PluginManager::startup(const Events::Startup& event)
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  TRACE_EVENT0("toplevel", "PluginManager::startup()")

  VLOG(9) << "(PluginManager) startup";

  const std::string executable_path
    = ::Corrade::Utility::Directory::path(
        ::Corrade::Utility::Directory::executableLocation());
  CHECK(!executable_path.empty())
    << "invalid executable path";

  const base::FilePath pathToDirWithPlugins
    = event.pathToDirWithPlugins.empty()
      // default value
        ? base::FilePath{executable_path}
        : event.pathToDirWithPlugins;
  CHECK(!pathToDirWithPlugins.empty())
    << "invalid path to directory with plugins";

  const std::string pluginsConfFile
    = ::Corrade::Utility::Directory::join(
        std::initializer_list<std::string>{
          pathToDirWithPlugins.value()
          , "plugins.conf"}
        );

  const base::FilePath pathToPluginsConfFile
    = event.pathToPluginsConfFile.empty()
      // default value
      ? base::FilePath{pluginsConfFile}
      : event.pathToPluginsConfFile;
  CHECK(!pathToPluginsConfFile.empty())
    << "invalid path to plugins configuration file";

  VLOG(9)
    << "using plugins configuration file: "
    << pathToPluginsConfFile;

  Corrade::Utility::Configuration conf{pathToPluginsConfFile.value()};
  std::vector<Corrade::Utility::ConfigurationGroup*> plugin_groups;
  bool plugin_filtering_enabled = false;
  // parse plugins configuration file
  {
    if(conf.hasGroups()) {
      if(conf.hasGroup("plugins")) {
        plugin_filtering_enabled = true;
        Corrade::Utility::ConfigurationGroup* configurationGroup
          = conf.group("plugins");
        if(configurationGroup) {
          plugin_groups = configurationGroup->groups("plugin");
        }
      }
    }
  }

  manager_
    = std::make_unique<
    ::Corrade::PluginManager::Manager<PluginType>
    >();

  /**
   * Set the plugin manager directory to load plugins
   *
   * Keeps loaded plugins untouched, removes unloaded plugins which are
   * not existing anymore and adds newly found plugins. The directory is
   * expected to be in UTF-8.
   * @partialsupport Not available on platforms without
   *      @ref CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT "dynamic plugin support".
   */
  manager_->setPluginDirectory(
    event.pathToDirWithPlugins.value());

  VLOG(9)
    << "Using plugin directory: "
    << manager_->pluginDirectory();

  std::vector<std::string> all_plugins
    = manager_->pluginList();

  std::vector<std::string> filtered_plugins;

  // parse list of plugin sections from plugins configuration file
  if(plugin_filtering_enabled)
  {
    for(const Corrade::Utility::ConfigurationGroup* plugin_group
        : plugin_groups)
    {
      if(!plugin_group->hasValue("title"))
      {
        LOG(WARNING)
            << "invalid plugin configuration: "
            << "title not provided";
        DCHECK(false);
        continue;
      }

      const std::string plugin_conf_name
        = plugin_group->value("title");
      auto find_result
        = std::find(all_plugins.begin()
                    , all_plugins.end()
                    , plugin_conf_name);
      if(find_result == all_plugins.end())
      {
        LOG(WARNING)
            << "plugin not found: "
            << plugin_conf_name;
        NOTREACHED();
      } else {
        filtered_plugins.push_back(plugin_conf_name);
      }
    }
  }

  // append path to plugins that
  // must be loaded independently of configuration file
  for(const base::FilePath& pluginPath
      : event.pathsToExtraPluginFiles)
  {
    VLOG(9)
        << "added plugin: "
        << pluginPath;
    if(pluginPath.empty() || !base::PathExists(pluginPath)) {
      LOG(ERROR)
        << "invalid path to plugin file: "
        << pluginPath;
      CHECK(false)
        << "path does not exist: "
        << pluginPath;
    }
    filtered_plugins.push_back(
      /*
      * @note If passing a file path, the implementation expects forward
      *      slashes as directory separators.
      * Use @ref Utility::Directory::fromNativeSeparators()
      *      to convert from platform-specific format.
      */
      Corrade::Utility::Directory::fromNativeSeparators(
        pluginPath.value()));
  }

  for(std::vector<std::string>::const_iterator it =
        filtered_plugins.begin()
      ; it != filtered_plugins.end(); ++it)
  {
    const std::string& pluginNameOrPath = *it;

    // The implementation expects forward slashes as directory separators.
    DCHECK(Corrade::Utility::Directory::fromNativeSeparators(
             pluginNameOrPath) == pluginNameOrPath);

    VLOG(9)
        << "plugin enabled: "
        << pluginNameOrPath;

    /**
     * @brief Load a plugin
     *
     * Returns @ref LoadState::Loaded if the plugin is already loaded or if
     * loading succeeded. For static plugins returns always
     * @ref LoadState::Static. On failure returns @ref LoadState::NotFound,
     * @ref LoadState::WrongPluginVersion,
     * @ref LoadState::WrongInterfaceVersion,
     * @ref LoadState::UnresolvedDependency or @ref LoadState::LoadFailed.
     *
     * If the plugin has any dependencies, they are recursively processed
     * before loading given plugin.
     *
     * If @p plugin is a plugin file path (i.e., ending with a
     * platform-specific extension such as `.so` or `.dll`), it's loaded
     * from given location and, if the loading succeeds, its basename
     * (without extension) is exposed as an available plugin name. It's
     * expected that a plugin with the same name is not already loaded. The
     * plugin will reside in the plugin list as long as it's loaded or,
     * after calling @ref unload() on it, until next call to
     * @ref setPluginDirectory() or @ref reloadPluginDirectory().
     *
     * @note If passing a file path, the implementation expects forward
     *      slashes as directory separators. Use @ref Utility::Directory::fromNativeSeparators()
     *      to convert from platform-specific format.
     *
     * @see @ref unload(), @ref loadState(), @ref Manager::instantiate(),
     *      @ref Manager::loadAndInstantiate()
     * @partialsupport On platforms without
     *      @ref CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT "dynamic plugin support"
     *      returns always either @ref LoadState::Static or
     *      @ref LoadState::NotFound.
    **/
    const bool is_loaded
      = static_cast<bool>(
          manager_->load(pluginNameOrPath)
          & (Corrade::PluginManager::LoadState::Loaded
             | Corrade::PluginManager::LoadState::
               Static)
          );
    if(!is_loaded) {
      LOG(ERROR)
        << "The requested plugin "
        << pluginNameOrPath
        << " cannot be loaded.";
      DCHECK(false);
      continue;
    }

    /**
    @brief Extract filename (without path) from filename

    If the filename doesn't contain any slash, returns whole string, otherwise
    returns everything after last slash.
    @attention The implementation expects forward slashes as directory separators.
        Use @ref fromNativeSeparators() to convert from a platform-specific format.
    @see @ref path(), @ref splitExtension()
    */
    const std::string pluginNameOrFilenameWithExt
      = Corrade::Utility::Directory::filename(pluginNameOrPath);

    DCHECK(base::FilePath{pluginNameOrPath}
             .BaseName().value()
           == pluginNameOrFilenameWithExt);

    /**
    @brief Split basename and extension
    @m_since{2019,10}

    Returns a pair `(root, ext)` where @cpp root + ext == path @ce, and ext is
    empty or begins with a period and contains at most one period. Leading periods
    on the filename are ignored, @cpp splitExtension("/home/.bashrc") @ce returns
    @cpp ("/home/.bashrc", "") @ce. Behavior equivalent to Python's
    @cb{.py} os.path.splitext() @ce.
    @attention The implementation expects forward slashes as directory separators.
        Use @ref fromNativeSeparators() to convert from a platform-specific format.
    @see @ref path(), @ref filename(), @ref String::partition()
    */
    const std::string pluginName
      = Corrade::Utility::Directory::splitExtension(
          pluginNameOrFilenameWithExt).first;

    DCHECK(base::FilePath{pluginNameOrFilenameWithExt}
             .BaseName().RemoveExtension().value()
           == pluginName);

    DCHECK(static_cast<bool>(
             manager_->loadState(pluginName)
             & (Corrade::PluginManager::LoadState::
                  Loaded
                | Corrade::PluginManager::LoadState::
                  Static)
             ));

    /// Returns new instance of given plugin.
    /// \note The plugin must be already
    /// successfully loaded by this manager.
    /// \note The returned value is never |nullptr|
    PluginPtr plugin
    /// \note must be plugin name, not path to file
      = manager_->instantiate(pluginName);
    if(!plugin) {
      LOG(ERROR)
        << "The requested plugin "
        << pluginNameOrPath
        << " cannot be instantiated.";
      DCHECK(false);
      continue;
    }

    VLOG(9)
      << "=== loading plugin ==";
    VLOG(9)
        << "plugin title:       "
        << plugin->title();
    VLOG(9)
        << "plugin description: "
        << plugin->description().substr(0, 100)
        << "...";

    plugin->load();

    loaded_plugins_.push_back(std::move(plugin));
    VLOG(9)
      << "=== plugin loaded ==";
  }

  DCHECK(!is_initialized_)
    << "Plugin manager must be initialized once."
    << "You can unload or reload plugins at runtime.";
  is_initialized_ = true;
}

void PluginManager::shutdown(const Events::Shutdown&)
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  TRACE_EVENT0("toplevel", "PluginManager::shutdown()");

  VLOG(9) << "(PluginManager) shutdown";

  /// \note destructor of ::Corrade::PluginManager::Manager
  /// also unloads all plugins
  for(PluginPtr& loaded_plugin : loaded_plugins_) {
    DCHECK(loaded_plugin);
    loaded_plugin->unload();
    //loaded_plugin.reset(nullptr);
  }
}

} // namespace backend
