#pragma once

#include <flexlib/ToolPlugin.hpp>

#include <base/sequence_checker.h>
#include <base/files/file_path.h>
#include <base/macros.h>

#include <basis/plugin_manager.h>

#include <vector>

namespace entt { class dispatcher; }

namespace flextool {

/// \note must store data related to base and basis libs
/// inits basic requirements, like thread pool, logging, etc.
class ScopedPluginEnvironment {
public:
  ScopedPluginEnvironment();

  ~ScopedPluginEnvironment();

  // init with provided settings
  [[nodiscard]] /* do not ignore return value */
  bool init(
    int argc
    , char* argv[]
    , const base::FilePath& pathToDirWithPlugins
    , const base::FilePath& pathToPluginsConfFile
    , const std::vector<base::FilePath>& pathsToExtraPluginFiles
    , const base::FilePath& outDir);

public:
  base::FilePath dir_exe_{};

  backend::PluginManager<::plugin::ToolPlugin> plug_mgr{};

  /// \note dispatcher must be thread-safe,
  /// so use entt after patch https://github.com/skypjack/entt/issues/449
  /// see https://github.com/skypjack/entt/commit/74f3df83dbc9fc4b43b8cfb9d71ba02234bd5c4a
  std::unique_ptr<entt::dispatcher> main_events_dispatcher;

private:
  SEQUENCE_CHECKER(sequence_checker_);

  DISALLOW_COPY_AND_ASSIGN(ScopedPluginEnvironment);
};

} // flextool
