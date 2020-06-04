#pragma once

#include "flexlib/ToolPlugin.hpp"
#include "flexlib/parser_constants.hpp"
#include "flexlib/utils.hpp"
#include "flexlib/funcParser.hpp"
#include "flexlib/clangUtils.hpp"
#include "flexlib/clangPipeline.hpp"
#include "flexlib/annotation_parser.hpp"
#include "flexlib/annotation_match_handler.hpp"
#include "flexlib/matchers/annotation_matcher.hpp"
#include "flexlib/options/ctp/options.hpp"
#if defined(CLING_IS_ON)
#include "flexlib/ClingInterpreterModule.hpp"
#endif // CLING_IS_ON

#include <base/macros.h>
#include <base/files/file_path.h>

#include <vector>
#include <string>

namespace flextool {

class ClangTool {
public:
  ClangTool() = default;

  // LibTooling is the C++ interface Clang provided.
  // It is very useful when you want to have full control over AST
  // (e.g. static analysis), or to implement a refactoring tool.
  // see https://kevinaboos.wordpress.com/2013/07/23/clang-tutorial-part-i-introduction/
  void run(
    // command-line arguments
    const std::vector<std::string>& args
    // custom settings
    , scoped_refptr<clang_utils::AnnotationMatchOptions>
        annotationMatchOptions);

private:
  DISALLOW_COPY_AND_ASSIGN(ClangTool);
};

} // flextool
