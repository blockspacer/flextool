include_guard( DIRECTORY )

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_clang_tidy
clang_tidy_enabler(
  PATHS
    #${CONAN_BIN_DIRS}
    ${CONAN_BIN_DIRS_LLVM_TOOLS}
  NO_SYSTEM_ENVIRONMENT_PATH
  NO_CMAKE_SYSTEM_PATH
  IS_ENABLED
    ${ENABLE_CLANG_TIDY}
  CHECK_TARGETS
    ${LIB_NAME}
  EXTRA_OPTIONS
    # TODO: use file .clang-tidy, similar to
    # https://github.com/mapbox/protozero/blob/ed6ba50979acf97b9c5300ae88a20d8b8b6c2fc4/.clang-tidy
    # https://fuchsia.googlesource.com/fuchsia/+/f7851796155f50b57d5ebd26a43ef49f5d5bdee4/.clang-tidy
    # https://github.com/jar-git/cmake-template/blob/master/.clang-tidy
    #
    # see clang-tidy --list-checks -checks='*' | grep "modernize"
    # see list of clang-tidy checks:
    # https://clang.llvm.org/extra/clang-tidy/checks/list.html
    #
    #  Disabled checks:
    #
    #  bugprone-signed-char-misuse
    #    Lots of warnings in varint.hpp otherwise.
    #
    #  cert-dcl21-cpp
    #    It is unclear whether this is still a good recommendation in modern C++.
    #
    #  cert-err58-cpp
    #    Due to the Catch2 test framework.
    #
    #  cert-err60-cpp
    #    Reports std::runtime_error as broken which we can't do anything about.
    #
    #  cppcoreguidelines-avoid-c-arrays
    #  hicpp-avoid-c-arrays
    #  modernize-avoid-c-arrays
    #    Makes sense for some array, but especially for char arrays using
    #    std::array isn't a good solution.
    #
    #  cppcoreguidelines-avoid-magic-numbers
    #  readability-magic-numbers
    #    Good idea, but it goes too far to force this everywhere.
    #
    #  cppcoreguidelines-macro-usage
    #    There are cases where macros are simply needed.
    #
    #  cppcoreguidelines-pro-bounds-array-to-pointer-decay
    #    Limited use and many false positives including for all asserts.
    #
    #  cppcoreguidelines-pro-bounds-pointer-arithmetic
    #    This is a low-level library, it needs to do pointer arithmetic.
    #
    #  cppcoreguidelines-pro-type-reinterpret-cast
    #    This is a low-level library, it needs to do reinterpret-casts.
    #
    #  fuchsia-*
    #    Much too strict.
    #
    #  google-runtime-references
    #    This is just a matter of preference, and we can't change the interfaces
    #    now anyways.
    #
    #  hicpp-no-array-decay
    #    Limited use and many false positives including for all asserts.
    #
    #  modernize-use-trailing-return-type
    #    We are not quite that modern.
    #    We do not want to rewrite code like so:
    #    int main(int argc, char* argv[])
    #    ~~~~
    #    auto                            -> int
    #
    #  readability-implicit-bool-conversion
    #    Not necessarily more readable.
    #
    -checks=*,-bugprone-signed-char-misuse,-cert-dcl21-cpp,-cert-err58-cpp,-cert-err60-cpp,-cppcoreguidelines-avoid-c-arrays,-cppcoreguidelines-avoid-magic-numbers,-cppcoreguidelines-macro-usage,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-pro-bounds-array-to-pointer-decay,-cppcoreguidelines-pro-type-reinterpret-cast,-fuchsia-*,-google-runtime-references,-hicpp-avoid-c-arrays,-hicpp-no-array-decay,-hicpp-vararg,-modernize-avoid-c-arrays,-modernize-use-trailing-return-type,-readability-implicit-bool-conversion,-readability-magic-numbers
    #-config="{CheckOptions: [ {key: readability-identifier-naming.ClassCase, value: CamelCase} ]}"
    -extra-arg=-std=c++17
    -extra-arg=-Qunused-arguments
    # To suppress compiler diagnostic messages
    # from third-party headers just use -isystem
    # instead of -I to include those headers.
    #-extra-arg=-nostdinc
    #-extra-arg=-nostdinc++
    -extra-arg=-DBOOST_SYSTEM_NO_DEPRECATED
    -extra-arg=-DBOOST_ERROR_CODE_HEADER_ONLY
    #List of files with line ranges to filter the
    #    warnings. Can be used together with
    #    -header-filter. The format of the list is a JSON
    #    array of objects:
    #      [
    #        {"name":"file1.cpp","lines":[[1,3],[5,7]]},
    #        {"name":"file2.h"}
    #      ]
    #-line-filter=\"[\
    #  {\"name\":\"path/to/file.cpp\"},\
    #  {\"name\":\"path/to/file.h\"}\
    #  ]\"
    # -header-filter is a whitelist not a blacklist.
    #-header-filter="^((?!/usr/|thirdparty|third_party|/cocos2d-x/external/|/cocos/scripting/).)*$"
    #-header-filter=flextool/*
    -header-filter=${CMAKE_CURRENT_SOURCE_DIR}
    -warnings-as-errors=cppcoreguidelines-avoid-goto
  VERBOSE
  REQUIRED
)
