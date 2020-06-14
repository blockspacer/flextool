#include "flextool/command_line_constants.hpp" // IWYU pragma: associated

#include <boost/program_options/errors.hpp>

namespace po = boost::program_options;

namespace cmd {

// extern
const char kAutoStartTracer[] = "start_tracing";

// extern
const char kTracingCategories[] = "tracing_categories";

// extern
const char kHelp[] = "help";

// extern
const char kInDir[] = "indir";

// extern
const char kOutDir[] = "outdir";

// extern
const char kVersion[] = "version";

// extern
const char kPluginsDir[] = "plugins_dir";

// extern
const char kPluginsConfFile[] = "plugins_config";

// extern
//const char kClangIncludeDirs[] = "clang_includes_dir";

// extern
//const char kClingIncludeDirs[] = "cling_includes_dir";

// extern
const char kThreadNum[] = "threads,J";

/// \todo remove
// extern
//const char kScriptDirs[] = "ctp_scripts_paths,C";

// extern
const char kScriptFiles[] = "cling_scripts";

// extern
const char kExtraPluginFiles[] = "load_plugin";

} // namespace cmd
