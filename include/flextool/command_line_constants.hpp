#pragma once

namespace cmd {

// start tracing at application start
/// \note you usually don't want to write big trace file,
/// so trace only required period of time
extern const char kAutoStartTracer[];

extern const char kTracingCategories[];

extern const char kHelp[];

// filesystem path for input files
extern const char kInDir[];

// filesystem path for output files.
extern const char kOutDir[];

extern const char kVersion[];

extern const char kPluginsDir[];

extern const char kPluginsConfFile[];

/// \todo make use of it
extern const char kThreadNum[];

extern const char kScriptFiles[];

// paths to plugin files that
// must be loaded independently from plugins configuration file
extern const char kExtraPluginFiles[];

} // namespace cmd
