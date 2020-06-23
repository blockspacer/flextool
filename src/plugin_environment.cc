#include "flextool/plugin_environment.hpp" // IWYU pragma: associated

#include <flexlib/ToolPlugin.hpp>

#include <basis/PluginManager.hpp>
#include <basis/cmd_util.hpp>
#include <basis/i18n.hpp>
#include <basis/icu_util.hpp>
#include <basis/log_util.hpp>
#include <basis/thread_pool_util.hpp>
#include <basis/tracing_util.hpp>

#include <base/process/memory.h>
#include <base/base_paths.h>
#include <base/bind.h>
#include <base/callback_forward.h>
#include <base/debug/stack_trace.h>
#include <base/feature_list.h>
#include <base/files/file.h>
#include <base/files/file_util.h>
#include <base/logging.h>
#include <base/memory/ref_counted_memory.h>
#include <base/memory/scoped_refptr.h>
#include <base/message_loop/message_loop.h>
#include <base/message_loop/message_loop_current.h>
#include <base/metrics/statistics_recorder.h>
#include <base/path_service.h>
#include <base/run_loop.h>
#include <base/sampling_heap_profiler/sampling_heap_profiler.h>
#include <base/single_thread_task_runner.h>
#include <base/strings/string_piece.h>
#include <base/strings/string_piece_forward.h>
#include <base/strings/string_util.h>
#include <base/system/sys_info.h>
#include <base/task/thread_pool/thread_pool.h>
#include <base/threading/platform_thread.h>
#include <base/trace_event/memory_dump_manager.h>
#include <base/trace_event/trace_buffer.h>
#include <base/trace_event/trace_event.h>
#include <base/trace_event/trace_log.h>

#include <entt/signal/dispatcher.hpp>

#include <algorithm>
#include <functional>
#include <iterator>
#include <locale>
#include <sstream>
#include <string.h>

// __has_include is currently supported by GCC and Clang. However GCC 4.9 may have issues and
// returns 1 for 'defined( __has_include )', while '__has_include' is actually not supported:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63662
#if __has_include(<filesystem>)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif // __has_include

#if __has_include(<filesystem>)
namespace fs = std::filesystem;
#else
namespace fs = std::experimental::filesystem;
#endif // __has_include

namespace flextool {

ScopedPluginEnvironment::ScopedPluginEnvironment()
  : main_events_dispatcher(std::make_unique<entt::dispatcher>())
{
  DCHECK(base::MessageLoop::current()->task_runner());
}

/// \note destroy dispatcher before plugin manager
/// see https://github.com/skypjack/entt/issues/103
ScopedPluginEnvironment::~ScopedPluginEnvironment()
{
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
