#include "flextool/clang_tool.hpp" // IWYU pragma: associated

#include <clang/AST/ASTContext.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchersMacros.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Frontend/ASTConsumers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

#include <base/files/file_util.h>
#include <base/path_service.h>
#include <base/sequenced_task_runner.h>
#include <base/trace_event/trace_event.h>

// __has_include is currently supported by GCC and Clang. However GCC 4.9 may have issues and
// returns 1 for 'defined( __has_include )', while '__has_include' is actually not supported:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63662
#if __has_include(<filesystem>)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif // __has_include

#if __has_include(<filesystem>)
namespace fs = std::filesystem;
#else
namespace fs = std::experimental::filesystem;
#endif // __has_include

namespace flextool {

/// \todo long method
void ClangTool::run(
  const std::vector<std::string>& args
  , scoped_refptr<clang_utils::AnnotationMatchOptions>
      annotationMatchOptions
){
  CHECK(!args.empty())
    << "You must provide at least one argument"
       " to clang";

  // convert |std::vector<std::string>| to |std::vector<const char*>|
  std::vector<
      const char* /// \note must manage pointer lifetime
  > args_vec;
  std::transform(
    args.begin()
    , args.end()
    , std::back_inserter(args_vec)
    , [](const std::string& value)
    {
        VLOG(9)
          << "added command-line argument for clang: "
          << value;
        DCHECK(!value.empty());
        /// \note must manage pointer lifetime
        return value.c_str();
    });

  int args_arc = args_vec.size();
  DCHECK(args_vec.size() == args.size());

  CHECK(!args_vec.empty())
    << "You must provide at least one argument"
       " to clang";
  DCHECK(args_vec[0]);
  const char **args_argv = &(args_vec[0]);
  DCHECK(args_argv);

  // see http://llvm.org/docs/doxygen/html/classllvm_1_1cl_1_1OptionCategory.html
  llvm::cl::OptionCategory UseOverrideCategory("Use override options");

  // parse the command-line args passed to your code
  // see http://clang.llvm.org/doxygen/classclang_1_1tooling_1_1CommonOptionsParser.html
  clang::tooling::CommonOptionsParser op(args_arc, args_argv,
    UseOverrideCategory);

  // log information about files what will be processed
  for(const auto& it: op.getSourcePathList()) {
    VLOG(9)
      << "added source file = "
      << (it.empty()
         ? " (empty path)"
         : it);
    base::FilePath absolutePath
      /// \note On POSIX, |MakeAbsoluteFilePath| fails
      /// if the path does not exist
      = base::MakeAbsoluteFilePath(base::FilePath(it));
    DCHECK(!absolutePath.empty())
      << "unable to find absolute path to "
      << absolutePath;
    base::File::Info fileInfo;
    bool hasInfo = base::GetFileInfo(absolutePath, &fileInfo);
    VLOG(9)
      << "as absolute source file path = "
      << (absolutePath.empty()
         ? " (empty path)"
         : absolutePath.value());
    if(!base::PathExists(absolutePath)) {
      LOG(WARNING)
        << "source file does not exist: "
        << (absolutePath.empty()
           ? " (empty path)"
           : absolutePath.value());
    }
    else if(base::DirectoryExists(absolutePath)) {
      DCHECK(!absolutePath.empty());
      LOG(WARNING)
        << "source file must be NOT directory: "
        << absolutePath;
    }
    else if(!hasInfo) {
      DCHECK(!absolutePath.empty());
      LOG(WARNING)
        << "unable to get source file information: "
        << absolutePath;
    }
    else if(!fileInfo.size) {
      DCHECK(!absolutePath.empty());
      LOG(WARNING)
        << "source file is empty: "
        << absolutePath;
    }
  } // for

  // create a new Clang Tool instance (a LibTooling environment)
  // see http://clang.llvm.org/doxygen/classclang_1_1tooling_1_1ClangTool.html
  clang::tooling::ClangTool tool(
    op.getCompilations()
    , op.getSourcePathList()
  );

  VLOG(9)
    << "running clang tool...";

  // Run the Clang Tool, creating a new FrontendAction.
  // The benefit of using LibTooling is that you can do things
  // before and after the analysis, like printing out
  // the modified code or counting the number of functions
  // in several source files.
  tool.run(
    new clang_utils::AnnotationMatchFactory(
      annotationMatchOptions));
}

} // flextool
