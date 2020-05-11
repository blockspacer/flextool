#include "flextool/command_line_constants.hpp" // IWYU pragma: associated

namespace po = boost::program_options;

namespace cmd {

const char kAutoStartTracer[] = "start_tracing";

const char kTracingCategories[] = "tracing_categories";

const char kHelp[] = "help";

const char kInDir[] = "indir";

const char kOutDir[] = "outdir";

const char kVersion[] = "version";

const char kPluginsDir[] = "plugins_dir";

const char kPluginsConfFile[] = "plugins_config";

//const char kClangIncludeDirs[] = "clang_includes_dir";

//const char kClingIncludeDirs[] = "cling_includes_dir";

const char kThreadNum[] = "threads,J";

/// \todo remove
//const char kScriptDirs[] = "ctp_scripts_paths,C";

const char kScriptFiles[] = "cling_scripts";

const char kExtraPluginFiles[] = "load_plugin";

} // namespace cmd
