#pragma once

#include "PluginManager.hpp"
#include "app_cmd_options.hpp"
#include "base_environment.hpp"
#include "boost_command_line.hpp"
#include "clang_util.hpp"

#include <base/at_exit.h>
#include <base/files/file_path.h>
#include <base/macros.h>
#include <base/memory/scoped_refptr.h>
#include <base/message_loop/message_loop.h>

#include <basis/scoped_log_run_time.hpp>

#include <flexlib/ClingInterpreterModule.hpp>

#include <string>
#include <vector>

namespace cling_utils { class ClingInterpreter; }

namespace flextool {

/// \note must store data related to cling C++ interpreter
class ScopedClingEnvironment {
public:
  ScopedClingEnvironment();

  ~ScopedClingEnvironment();

  // init with provided settings
  bool init(
    const std::vector<base::FilePath>& scriptFiles
    , const std::vector<std::string>& cling_extra_args);

public:
#if defined(CLING_IS_ON)
  std::unique_ptr<cling_utils::ClingInterpreter> clingInterpreter;

  std::vector<std::string> clingIncludePaths{".", "../"};

  std::vector<std::string> clingInterpreterArgs;
#endif // defined(CLING_IS_ON)

private:
  DISALLOW_COPY_AND_ASSIGN(ScopedClingEnvironment);
};

} // flextool
