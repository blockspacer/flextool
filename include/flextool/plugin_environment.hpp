#pragma once

#include "PluginManager.hpp"
#include "app_cmd_options.hpp"
#include "boost_command_line.hpp"
#include "clang_util.hpp"

#include <base/at_exit.h>
#include <base/files/file_path.h>
#include <base/macros.h>
#include <base/memory/scoped_refptr.h>
#include <base/message_loop/message_loop.h>

#include <basis/scoped_log_run_time.hpp>

#include <string>
#include <vector>

namespace base { class SingleThreadTaskRunner; }

namespace entt { class dispatcher; }

namespace i18n { class I18n; }

namespace flextool {

/// \note must store data related to base and basis libs
/// inits basic requirements, like thread pool, logging, etc.
class ScopedPluginEnvironment {
public:
  ScopedPluginEnvironment();

  ~ScopedPluginEnvironment();

  // init with provided settings
  bool init(
    int argc
    , char* argv[]
    , const base::FilePath& pathToDirWithPlugins
    , const base::FilePath& pathToPluginsConfFile
    , const std::vector<base::FilePath>& pathsToExtraPluginFiles
    , const base::FilePath& outDir);

public:
  base::FilePath dir_exe_{};

  backend::PluginManager plug_mgr{};

  /// \note dispatcher must be thread-safe,
  /// so use entt after patch https://github.com/skypjack/entt/issues/449
  /// see https://github.com/skypjack/entt/commit/74f3df83dbc9fc4b43b8cfb9d71ba02234bd5c4a
  std::unique_ptr<entt::dispatcher> main_events_dispatcher;

private:
  DISALLOW_COPY_AND_ASSIGN(ScopedPluginEnvironment);
};

} // flextool
