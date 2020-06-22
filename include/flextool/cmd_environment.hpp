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

// setup command-line arguments
class ScopedCmdEnvironment {
public:
  ScopedCmdEnvironment();

  ~ScopedCmdEnvironment();

  // init with provided settings
  bool init(int argc, char* argv[]);

public:
  cmd::BoostCmd boostCmd{};

  cmd::AppCmdOptions appCmd;

  base::FilePath dir_exe_{};

private:
  DISALLOW_COPY_AND_ASSIGN(ScopedCmdEnvironment);
};

} // flextool
