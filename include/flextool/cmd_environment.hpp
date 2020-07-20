#pragma once

#include "app_cmd_options.hpp"

#include <base/files/file_path.h>
#include <base/macros.h>
#include <base/sequence_checker.h>

#include <basis/boost_command_line.hpp>

namespace flextool {

// setup command-line arguments
class ScopedCmdEnvironment {
public:
  ScopedCmdEnvironment();

  ~ScopedCmdEnvironment();

  // init with provided settings
  [[nodiscard]] /* do not ignore return value */
  bool init(int argc, char* argv[]);

public:
  cmd::BoostCmdParser boostCmdParser{};

  cmd::AppCmdOptions appCmd;

  base::FilePath dir_exe_{};

private:
  SEQUENCE_CHECKER(sequence_checker_);

  DISALLOW_COPY_AND_ASSIGN(ScopedCmdEnvironment);
};

} // flextool
