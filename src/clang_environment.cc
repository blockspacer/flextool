#include "flextool/clang_environment.hpp" // IWYU pragma: associated

#include "flextool/app_cmd_options.hpp"
#include "flextool/clang_util.hpp"
#include "flextool/cmd_environment.hpp"
#include "flextool/command_line_constants.hpp"

#include <base/files/file_path.h>
#include <base/sequence_checker.h>
#include <base/bind.h>
#include <base/logging.h>
#include <base/memory/scoped_refptr.h>
#include <base/strings/string_piece.h>
#include <base/strings/string_piece_forward.h>
#include <base/threading/thread_task_runner_handle.h>
#include <base/check.h>

#include <flexlib/annotation_match_handler.hpp>
#include <flexlib/annotation_parser.hpp>
#include <flexlib/clangPipeline.hpp>
#include <flexlib/matchers/annotation_matcher.hpp>
#include <flexlib/parser_constants.hpp>

#include <algorithm>
#include <iterator>
#include <sstream>

namespace std {

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

} // namespace std

namespace {

static const char kClangHelpFlag[] = "-help";

} // namespace

namespace flextool {

ScopedClangEnvironment::ScopedClangEnvironment()
{
  DETACH_FROM_SEQUENCE(sequence_checker_);

  DCHECK(base::ThreadTaskRunnerHandle::Get());
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
