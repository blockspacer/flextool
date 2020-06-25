#pragma once

#include "app_cmd_options.hpp"

#include <base/at_exit.h>
#include <base/files/file_path.h>
#include <base/macros.h>
#include <base/memory/scoped_refptr.h>
#include <base/message_loop/message_loop.h>
#include <base/sequence_checker.h>

#include <basis/PluginManager.hpp>
#include <basis/base_environment.hpp>
#include <basis/scoped_log_run_time.hpp>
#include <basis/boost_command_line.hpp>

#include <string>
#include <vector>

namespace base { class SingleThreadTaskRunner; }

namespace entt { class dispatcher; }

namespace i18n { class I18n; }

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
