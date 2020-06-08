#pragma once

#include "flexlib/ToolPlugin.hpp"
#include "flexlib/annotation_match_handler.hpp"
#include "flexlib/annotation_parser.hpp"
#include "flexlib/clangPipeline.hpp"
#include "flexlib/clangUtils.hpp"
#include "flexlib/funcParser.hpp"
#include "flexlib/matchers/annotation_matcher.hpp"
#include "flexlib/options/ctp/options.hpp"
#include "flexlib/parser_constants.hpp"
#include "flexlib/utils.hpp"
#if defined(CLING_IS_ON)
#include "flexlib/ClingInterpreterModule.hpp"
#endif // CLING_IS_ON

#include <base/files/file_path.h>
#include <base/macros.h>

#include <string>
#include <vector>

namespace flextool {

class ClangTool {
public:
  using AnnotationMatchOptionsPtr
    = scoped_refptr<clang_utils::AnnotationMatchOptions>;

  ClangTool() = default;

  // LibTooling is the C++ interface Clang provided.
  // It is very useful when you want to have full control over AST
  // (e.g. static analysis), or to implement a refactoring tool.
  // see https://kevinaboos.wordpress.com/2013/07/23/clang-tutorial-part-i-introduction/
  void run(
    // command-line arguments
    const std::vector<std::string>& args
    // custom settings
    , AnnotationMatchOptionsPtr annotationMatchOptions);

private:
  DISALLOW_COPY_AND_ASSIGN(ClangTool);
};

} // flextool
