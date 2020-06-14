#pragma once

#include <base/macros.h>
#include <base/memory/scoped_refptr.h>

#include <string>
#include <vector>

namespace clang_utils {
class AnnotationMatchOptions;
} // namespace clang_utils

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
