#pragma once

#include <base/macros.h>
#include <base/sequenced_task_runner.h>
#include <base/trace_event/trace_event.h>
#include <base/files/file_path.h>

#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/program_options.hpp>
#include <boost/utility/in_place_factory.hpp>

#include <vector>
#include <string>

namespace cmd {

// start tracing at application start
/// \note you usually don't want to write big trace file,
/// so trace only required period of time
extern const char kAutoStartTracer[];

extern const char kTracingCategories[];

extern const char kHelp[];

/// \todo make use of it
//extern const char log_arg_name = "log,L";
// --srcdir - filesystem path for input files
extern const char kInDir[];

// --outdir - filesystem path for output files.
extern const char kOutDir[];

extern const char kVersion[];

extern const char kPluginsDir[];

extern const char kPluginsConfFile[];

/// \todo remove
//extern const char kClangIncludeDirs[];

/// \todo remove
//extern const char kClingIncludeDirs[];

/// \todo make use of it
extern const char kThreadNum[];

/// \todo remove
//extern const char kScriptDirs[];

extern const char kScriptFiles[];

// paths to plugin files that
// must be loaded independently from plugins configuration file
extern const char kExtraPluginFiles[];

} // namespace cmd
