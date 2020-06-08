#include "flextool/app_environment.hpp"
#include "flextool/clang_tool.hpp"

int main(int argc, char* argv[])
{
  DCHECK(argc > 1);

  /// \note scoped environment must be destroyed at app exit
  flextool::ScopedAppEnvironment env;

  {
    const bool envCreated = env.init(argc, argv);
    if(!envCreated) {
      LOG(ERROR)
        << "Unable to create application environment";
      return
        EXIT_FAILURE;
    }
  }

  // run Clang LibTooling
  {
    flextool::ClangTool tool;

    DCHECK(!env.args_storage.empty());
    DCHECK(env.annotationMatchOptions);
    DCHECK(!env.annotationMatchOptions->annotateName.empty());
    DCHECK(env.annotationMatchOptions->annotationMatchCallback);
    DCHECK(env.annotationMatchOptions->endSourceFileAction);

    tool.run(
      env.args_storage   // command-line arguments
      , env.annotationMatchOptions   // custom settings
      );
  }

  return
    EXIT_SUCCESS;
}
