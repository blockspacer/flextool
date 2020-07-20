#include "flextool/cmd_environment.hpp" // IWYU pragma: associated

#include "flextool/app_cmd_options.hpp"

#include <base/files/file_path.h>
#include <base/sequence_checker.h>
#include <base/base_paths.h>
#include <base/logging.h>
#include <base/path_service.h>

#include <basis/boost_command_line.hpp>

#include <boost/program_options/options_description.hpp>

#include <sstream>

namespace flextool {

ScopedCmdEnvironment::ScopedCmdEnvironment()
  : appCmd(boostCmdParser)
{
  DETACH_FROM_SEQUENCE(sequence_checker_);
}

ScopedCmdEnvironment::~ScopedCmdEnvironment()
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
}

/// \todo refactor long method
bool ScopedCmdEnvironment::init(
  int argc, char* argv[])
{
  DCHECK(argc > 0);

  if (!base::PathService::Get(base::DIR_EXE, &dir_exe_)) {
    NOTREACHED();
    // stop app execution with EXIT_FAILURE
    return
      false;
  }

  /// \note log all command-line arguments before
  /// parsing them as program options
  {
    VLOG(9)
        << "started "
        << dir_exe_
        << " with arguments:";
    for(int i = 0; i < argc; ++i) {
      VLOG(9)
        << " "
        << argv[i]
        << " ";
    }
  }

  // set description for command-line arguments
  {
    using boost::program_options::options_description_easy_init;

    options_description_easy_init& options
      = boostCmdParser.options();
    options = appCmd.registerOptions(options);
  }

  // set parser for command-line arguments
  {
    bool initOk = boostCmdParser.init(argc, argv);
    if(!initOk)
    {
      LOG(ERROR)
        << "Unable to parse command-line arguments";
      DCHECK(false);
      // stop app execution with EXIT_FAILURE
      return
        false;
    }
  }

  return
    true;
}

} // flextool
