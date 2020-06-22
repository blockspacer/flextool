include_guard( DIRECTORY )

# Hack to depend NOT on CONAN_PKG::libname, but on libname directly
# See for details
# https://docs.conan.io/en/latest/developing_packages/workspaces.html
set(${PROJECT_NAME}_LOCAL_BUILD FALSE CACHE BOOL "${PROJECT_NAME}_LOCAL_BUILD")
message(STATUS "${PROJECT_NAME}_LOCAL_BUILD=${${PROJECT_NAME}_LOCAL_BUILD}")

option(BUILD_DOXY_DOC
  "build doxygen documentation" OFF)

option(ENABLE_LWYU
  "Enable CMAKE_LINK_WHAT_YOU_USE" OFF)

option(COMPILE_WITH_LLVM_TOOLS
  "Enable clang from llvm_tools (conan package)" OFF)

option(ENABLE_TESTS
  "Enable tests" OFF)

# see https://gitlab.kitware.com/cmake/community/wikis/FAQ#can-i-do-make-uninstall-with-cmake
option(ENABLE_UNINSTALL
  "Enable uninstall (using install_manifest.txt)" OFF)

option(ENABLE_CPPCHECK
  "Enable cppcheck" OFF)

# TODO: Dr. Memory https://github.com/DynamoRIO/drmemory

option(ENABLE_VALGRIND
  "Enable valgrind" OFF)

option(ENABLE_CLANG_TIDY
  "Enable clang-tidy" OFF)

option(ENABLE_CLANG_FORMAT
  "Enable clang-format" OFF)

option(ENABLE_UNCRUSTIFY
  "Enable uncrustify" OFF)

option(ENABLE_IWYU
  "Enable include-what-you-use" OFF)

option(ENABLE_CPPCLEAN
  "Enable cppclean" OFF)

option(ENABLE_LTO
  "Enable Link Time Optimization" OFF)

# see https://github.com/Ericsson/codechecker/blob/master/tools/report-converter/README.md#undefined-behaviour-sanitizer
# NOTE: Compile with -g and -fno-omit-frame-pointer
# to get proper debug information in your binary.
# NOTE: Run your program with environment variable UBSAN_OPTIONS=print_stacktrace=1.
# see https://github.com/google/sanitizers/wiki/SanitizerCommonFlags
option(ENABLE_UBSAN
  "Enable Undefined Behaviour Sanitizer" OFF)

# see https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer
# see https://github.com/Ericsson/codechecker/blob/master/tools/report-converter/README.md#address-sanitizer
# NOTE: Compile with -g and -fno-omit-frame-pointer
# to get proper debug information in your binary.
# NOTE: use ASAN_OPTIONS=detect_leaks=1 LSAN_OPTIONS=suppressions=suppr.txt
# NOTE: You need the ASAN_OPTIONS=symbolize=1
# to turn on resolving addresses in object code
# to source code line numbers and filenames.
# This option is implicit for Clang but it won't do any harm.
# see https://github.com/google/sanitizers/wiki/SanitizerCommonFlags
option(ENABLE_ASAN
  "Enable Address Sanitizer" OFF)

# see https://github.com/Ericsson/codechecker/blob/master/tools/report-converter/README.md#memory-sanitizer
# NOTE: Compile with -g and -fno-omit-frame-pointer
# to get proper debug information in your binary.
option(ENABLE_MSAN
  "Enable Memory Sanitizer" OFF)

# see https://github.com/Ericsson/codechecker/blob/master/tools/report-converter/README.md#thread-sanitizer
# NOTE: Compile with -g
# to get proper debug information in your binary.
option(ENABLE_TSAN
  "Enable Thread Sanitizer" OFF)

# Things that can catch OCLINT
# http://oclint-docs.readthedocs.io/en/stable/rules/index.html
# OCLINT command-line manual
# https://oclint-docs.readthedocs.io/en/stable/manual/oclint.html
option(ENABLE_OCLINT
  "Enable oclint" OFF)

# TODO: __do_global_dtors_aux, base::debug::CollectStackTrace
option(ENABLE_VALGRIND_TESTS
  "Enable valgrind for unit tests" OFF)

option(USE_LD_GOLD
  "Use GNU gold linker" OFF)

option(USE_CCACHE
  "Use CCACHE" OFF)

option(USE_COVERAGE
  "Use CCACHE" OFF)

if(ENABLE_LWYU)
  # Enable linker flags -r -u to create warnings for unused dependencies at link time.
  # Warning: Unused direct dependencies: '/usr/lib/libm.so.6' (required by std)
  # LWYU will modify the flags to ld to show any libraries
  # being linked into targets that are not contributing symbols
  # to the target being linked.
  # see https://cmake.org/cmake/help/latest/prop_tgt/LINK_WHAT_YOU_USE.html
  # NOTE: This is only applicable to executable and shared library targets
  set(CMAKE_LINK_WHAT_YOU_USE ON)
endif(ENABLE_LWYU)

set(ENABLE_CLING TRUE CACHE BOOL "ENABLE_CLING")
message(STATUS "ENABLE_CLING=${ENABLE_CLING}")

set(ENABLE_CLANG_FROM_CONAN FALSE CACHE BOOL "ENABLE_CLANG_FROM_CONAN")
message(STATUS "ENABLE_CLANG_FROM_CONAN=${ENABLE_CLANG_FROM_CONAN}")

if(ENABLE_CLANG_FROM_CONAN AND ENABLE_CLING)
  message(FATAL_ERROR
    "don't use both ENABLE_CLING and ENABLE_CLANG_FROM_CONAN at the same time. cling already provides clang libtooling")
endif()
