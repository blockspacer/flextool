#pragma once

#include "flexlib/annotation_parser.hpp"
#include "flexlib/annotation_match_handler.hpp"
#include "flexlib/clangPipeline.hpp"

#include <string>
#include <vector>

#include <Corrade/PluginManager/AbstractPlugin.h>
#include <Corrade/PluginManager/AbstractManager.h>
#include <Corrade/PluginManager/PluginManager.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/PluginManager/PluginMetadata.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Corrade/Utility/Debug.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Containers/Array.h>

#include <entt/entt.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/helper.hpp>
#include <entt/entity/group.hpp>

#include <base/logging.h>
#include <base/cpu.h>
#include <base/command_line.h>
#include <base/debug/alias.h>
#include <base/debug/stack_trace.h>
#include <base/memory/ptr_util.h>
#include <base/sequenced_task_runner.h>
#include <base/strings/string_util.h>
#include <base/trace_event/trace_event.h>

#if defined(CLING_IS_ON)
#include "flexlib/ClingInterpreterModule.hpp"
#endif // CLING_IS_ON

#define REGISTER_PLUGIN(name, className, interface) \
  CORRADE_PLUGIN_REGISTER(name, className, interface)

namespace plugin {

typedef ::Corrade::PluginManager::AbstractManager AbstractManager;
typedef ::Corrade::PluginManager::AbstractPlugin AbstractPlugin;

class ToolPlugin
  : public ::plugin::AbstractPlugin
{
 public:
  struct Events {
    struct StringCommand {
      // unprocessed command
      std::string raw_cmd;

      // |raw_cmd| split by space
      std::vector<std::string> split_parts;
    };

    struct RegisterAnnotationMethods {
      mutable ::cxxctp::AnnotationMethods*
        annotationMethods;
      mutable ::clang_utils::SourceTransformPipeline*
        sourceTransformPipeline;
    };

#if defined(CLING_IS_ON)
    struct RegisterClingInterpreter {
      mutable ::cling_utils::ClingInterpreter* clingInterpreter;
    };
#endif // CLING_IS_ON

  };

  explicit ToolPlugin(
    ::plugin::AbstractManager& manager
    , const std::string& plugin)
    : AbstractPlugin{manager, plugin}
  {
    DETACH_FROM_SEQUENCE(sequence_checker_);
  }

  static std::string pluginInterface() {
    // plugin interface version checks to avoid unexpected behavior
    return "backend.ToolPlugin/1.0";
  }

  static std::vector<std::string> pluginSearchPaths() {
    return {""};
  }

  virtual void connect_to_dispatcher(
    entt::dispatcher& event_dispatcher) = 0;

  virtual void disconnect_dispatcher(
    entt::dispatcher& event_dispatcher) = 0;

  virtual std::string author() const = 0;

  virtual std::string title() const = 0;

  virtual std::string description() const = 0;

  virtual bool load() = 0;

  virtual bool unload() = 0;

protected:
  SEQUENCE_CHECKER(sequence_checker_);

  DISALLOW_COPY_AND_ASSIGN(ToolPlugin);
};

} // namespace plugin
