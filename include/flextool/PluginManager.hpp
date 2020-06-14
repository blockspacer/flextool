#pragma once

// 'plugin::ToolPlugin' must be complete type
// or may cause undefined behavior with
// Corrade/Containers/Pointer
#include "flexlib/ToolPlugin.hpp"

#include <base/macros.h>
#include <base/files/file_path.h>
#include <base/logging.h>
#include <base/sequence_checker.h>

#include <Corrade/Containers/Pointer.h>
#include <Corrade/PluginManager/Manager.h>

#include <stddef.h>
#include <string>
#include <vector>

namespace entt {
  class dispatcher;
} // namespace entt

/// \todo hide impl details
//namespace Corrade {
//
//namespace PluginManager {
//
//template<class T>
//class Manager;
//
//} // namespace Corrade
//
//} // namespace Corrade

namespace backend {

class PluginManager {
public:
  using PluginType
    = ::plugin::ToolPlugin;

  using PluginPtr
    = Corrade::Containers::Pointer<PluginType>;

  struct Events {
    struct Startup {
      base::FilePath pathToDirWithPlugins;
      base::FilePath pathToPluginsConfFile;
      std::vector<base::FilePath> pathsToExtraPluginFiles;
    };
    struct Shutdown {
      // event parameters
    };
    struct StringCommand {
      std::string cmd;
    };
  };

public:
  PluginManager()
  {
    DETACH_FROM_SEQUENCE(sequence_checker_);
  }

  ~PluginManager() {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

    /// \note destoy |entt::dispatcher| before plugin manager
  }

  // |dispatcher| to handle per-plugin events
  void connect_plugins_to_dispatcher(
    entt::dispatcher& events_dispatcher);

  // |dispatcher| to handle |PluginManager|
  // events like |Startup| or |Shutdown|
  void connect_to_dispatcher(
    entt::dispatcher& events_dispatcher);

  void startup(
    const Events::Startup& event);

  void shutdown(
    const Events::Shutdown& event);

  size_t countLoadedPlugins()
  const noexcept
  {
    return
      loaded_plugins_.size();
  }

private:
  bool is_initialized_ = false;

  std::unique_ptr<
    ::Corrade::PluginManager::Manager<PluginType>
    > manager_;

  std::vector<PluginPtr> loaded_plugins_;

  SEQUENCE_CHECKER(sequence_checker_);

  DISALLOW_COPY_AND_ASSIGN(PluginManager);
};

} // namespace backend
