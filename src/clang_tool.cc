#include "flextool/clang_tool.hpp" // IWYU pragma: associated

#include <flexlib/matchers/annotation_matcher.hpp>

#include <base/files/file.h>
#include <base/files/file_path.h>
#include <base/files/file_util.h>
#include <base/logging.h>
#include <base/memory/scoped_refptr.h>

#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>

#include <algorithm>
#include <iterator>
#include <ostream>

namespace flextool {

static void logSourcePathList(
  const std::vector<std::string>& sourcePathList)
{
  if (sourcePathList.size() < 1) {
    LOG(WARNING)
      << "At least one input file expected.";
  }
  for(const auto& it: sourcePathList) {
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
}

void ClangTool::run(
  const std::vector<std::string>& args
  , scoped_refptr<clang_utils::AnnotationMatchOptions>
  annotationMatchOptions
  )
{
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
      return
      value.c_str();
    });

  DCHECK(args_vec.size() == args.size());

  CHECK(!args_vec.empty())
    << "You must provide at least one argument"
    " to clang";
  DCHECK(args_vec[0]);
  const char **args_argv = &(args_vec[0]);
  DCHECK(args_argv);

  // see http://llvm.org/docs/doxygen/html/classllvm_1_1cl_1_1OptionCategory.html
  llvm::cl::OptionCategory UseOverrideCategory(
    "Use override options");

  const int args_arc
    = static_cast<int>(args_vec.size());

  // parse the command-line args passed to your code
  // see http://clang.llvm.org/doxygen/classclang_1_1tooling_1_1CommonOptionsParser.html
  clang::tooling::CommonOptionsParser optionsParser(
    args_arc, args_argv, UseOverrideCategory);

  // log information about files what will be processed
  logSourcePathList(optionsParser.getSourcePathList());

  // create a new Clang Tool instance (a LibTooling environment)
  // see http://clang.llvm.org/doxygen/classclang_1_1tooling_1_1ClangTool.html
  clang::tooling::ClangTool tool(
    optionsParser.getCompilations()
    , optionsParser.getSourcePathList()
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
