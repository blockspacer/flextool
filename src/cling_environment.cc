#include "flextool/cling_environment.hpp" // IWYU pragma: associated

#include "flextool/clang_util.hpp"

#include <flexlib/ClingInterpreterModule.hpp>

#include <base/files/file_path.h>
#include <base/sequence_checker.h>
#include <base/logging.h>
#include <base/message_loop/message_loop.h>
#include <base/message_loop/message_loop_current.h>
#include <base/strings/string_piece.h>
#include <base/strings/string_piece_forward.h>
#include <base/strings/string_util.h>

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

namespace flextool {

ScopedClingEnvironment::ScopedClingEnvironment()
{
  DETACH_FROM_SEQUENCE(sequence_checker_);

  DCHECK(base::MessageLoop::current()->task_runner());
}

ScopedClingEnvironment::~ScopedClingEnvironment()
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
}

bool ScopedClingEnvironment::init(
  const std::vector<base::FilePath>& scriptFiles
  , const std::vector<std::string>& cling_extra_args)
{
#if defined(CLING_IS_ON)
  // generate cling arguments based on command-line arguments
  {
    const bool ok
      = clang_util::add_default_cling_args(clingInterpreterArgs);
    if(!ok) {
      LOG(ERROR)
        << "unable to populate Cling arguments";
      return
        false;
    }
    DCHECK(!clingInterpreterArgs.empty());

    // pipeline clang args to cling
    {
      DCHECK(!cling_extra_args.empty());
      for(const std::string& arg: cling_extra_args) {

        // pipeline only args starting with "-"
        if(!base::StartsWith(
             base::StringPiece{arg}
             , base::StringPiece{clang_util::kClingArgPrefix}
             , base::CompareCase::INSENSITIVE_ASCII))
        {
          LOG(ERROR)
            << "unable to pipeline cling argument to cling: "
            << arg
            << " cause it does not have prefix: "
            << clang_util::kClingArgPrefix;
          NOTREACHED();
          continue;
        }

        clingInterpreterArgs.push_back(arg);
      }
    }

    LOG(INFO)
        << "populated Cling arguments: "
        << clingInterpreterArgs;
  }

  {
    VLOG(9)
      << "creating"
      " clingInterpreter";
    DCHECK(!clingInterpreterArgs.empty());
    DCHECK(!clingIncludePaths.empty());
    clingInterpreter
      = std::make_unique<
      cling_utils::ClingInterpreter>(
          "MainClingInterpreter_debug_id"
          , clingInterpreterArgs
          , clingIncludePaths
          );
  }

  // load files into C++ interpreter
  for(const base::FilePath& full_path
      : scriptFiles)
  {
    DCHECK(clingInterpreter);
    DCHECK(!full_path.empty());
    VLOG(9)
      << "calling"
      " clingInterpreter->loadFile";
    clingInterpreter->loadFile(full_path.value());
    LOG(INFO)
        << "loaded into Cling interpreter file "
        << full_path;
  }
#endif // defined(CLING_IS_ON)

  return
    true;
}

} // flextool
