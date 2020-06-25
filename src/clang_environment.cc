#include "flextool/clang_environment.hpp" // IWYU pragma: associated

#include <entt/signal/dispatcher.hpp>

#include "flextool/app_cmd_options.hpp"
#include "flextool/clang_util.hpp"
#include "flextool/command_line_constants.hpp"
#include "flextool/version.hpp"

#include <basis/boost_command_line.hpp>
#include <basis/cmd_util.hpp>
#include <basis/i18n.hpp>
#include <basis/icu_util.hpp>
#include <basis/log_util.hpp>
#include <basis/thread_pool_util.hpp>
#include <basis/tracing_util.hpp>
#include <basis/PluginManager.hpp>

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

namespace {

static const char kClangHelpFlag[] = "-help";

/// \todo code repeat
template<class T>
std::ostream& operator<<(
  std::ostream& stream, const std::vector<T>& data)
{
  std::copy(data.begin(), data.end(),
            std::ostream_iterator<T>(stream, " "));
  return
    stream;
}

} // namespace

namespace flextool {

ScopedClangEnvironment::ScopedClangEnvironment()
{
  DETACH_FROM_SEQUENCE(sequence_checker_);

  DCHECK(base::MessageLoop::current()->task_runner());
}

ScopedClangEnvironment::~ScopedClangEnvironment()
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
}

/// \todo refactor long method
bool ScopedClangEnvironment::init(
  flextool::ScopedCmdEnvironment& cmd_env)
{
  // generate clang arguments based on command-line arguments
  {
    const base::FilePath clangBuildPath = cmd_env.appCmd.outDir();
    DCHECK(!clangBuildPath.empty());
    const bool ok = clang_util::populateClangArguments(
      clangBuildPath
      , args_storage
      , cling_extra_args
      , cmd_env.boostCmdParser);
    if(!ok) {
      LOG(ERROR)
        << "unable to populate Clang arguments";
      return
        false;
    }
    DCHECK(!args_storage.empty());

    LOG(INFO)
        << "populated Clang arguments: "
        << args_storage;
  }

  if (cmd_env.appCmd.count(cmd::kHelp)) {
    args_storage.push_back(kClangHelpFlag);
    /// \note continue to forward help to clang libtooling
  }

  {
    VLOG(9)
      << "creating"
      " flexlib::AnnotationMethods";
    annotationMethods
      = std::make_unique<
      ::flexlib::AnnotationMethods
      >();
  }

  {
    VLOG(9)
      << "creating"
      " flexlib::AnnotationParser";
    DCHECK(annotationMethods);
    annotationParser
      = std::make_unique<
      ::flexlib::AnnotationParser
      >(annotationMethods.get());
  }

  const base::FilePath outDir = cmd_env.appCmd.outDir();

  {
    VLOG(9)
      << "creating"
      " flexlib::AnnotationMatchHandler";
    CHECK(!outDir.empty())
      << "you must provide output directory";
    DCHECK(annotationParser);
    DCHECK(annotationMethods);
    anotationMatchHandler
      = std::make_unique<
      ::flexlib::AnnotationMatchHandler
      >(annotationParser.get()
        , annotationMethods.get()
        , base::BindRepeating(
          &clang_util::FileSaveHandler::saveFile
          , base::Unretained(&fileSaveHandler) // must control lifetime
          , outDir
          , true // shouldFlushFile
          , false // shouldPrintConsole
          ));
  }

  {
    VLOG(9)
      << "creating"
      " clang_utils::AnnotationMatchOptions";
    DCHECK(!base::StringPiece(::flexlib::kAnnotateAttrName).empty());
    DCHECK(anotationMatchHandler);
    annotationMatchOptions =
      new clang_utils::AnnotationMatchOptions(
        ::flexlib::kAnnotateAttrName
        , base::BindRepeating(
          &::flexlib::AnnotationMatchHandler::matchHandler
          , base::Unretained(anotationMatchHandler.get()))
        , base::BindRepeating(
          &::flexlib::AnnotationMatchHandler::endSourceFileHandler
          , base::Unretained(anotationMatchHandler.get()))
        );
  }

  sourceTransformPipeline
    = std::make_unique<
    ::clang_utils::SourceTransformPipeline
    >();

  return
    true;
}

} // flextool
