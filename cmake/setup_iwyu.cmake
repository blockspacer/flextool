include_guard( DIRECTORY )

# include-what-you-use mappings file
# Mappings file format:
#   { include: [ '@"mutt/.*"', private, '"mutt/mutt.h"', public ] },
#   { include: [ '@"conn/.*"', private, '"conn/conn.h"', public ] },
set(IWYU_IMP "${CMAKE_CURRENT_SOURCE_DIR}/cmake/iwyu/iwyu.imp")
if(NOT EXISTS ${IWYU_IMP})
  message(FATAL_ERROR "Unable to find file: ${IWYU_IMP}")
endif(NOT EXISTS ${IWYU_IMP})

if(ENABLE_IWYU)
  message(STATUS
    "CONAN_LLVM_TOOLS_ROOT: ${CONAN_LLVM_TOOLS_ROOT}")
  # NOTE: you can create symlink to fix issue
  if(NOT EXISTS "${CONAN_LLVM_TOOLS_ROOT}/include")
    message(FATAL_ERROR "Unable to find file: ${CONAN_LLVM_TOOLS_ROOT}/include")
  endif()
endif(ENABLE_IWYU)

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_iwyu
iwyu_enabler(
  PATHS
    #${CONAN_BIN_DIRS}
    ${CONAN_BIN_DIRS_LLVM_TOOLS}
  NO_SYSTEM_ENVIRONMENT_PATH
  NO_CMAKE_SYSTEM_PATH
  IS_ENABLED
    ${ENABLE_IWYU}
  CHECK_TARGETS
    ${LIB_NAME}
  EXTRA_OPTIONS
    -std=c++17
    -DCLING_IS_ON=1
    # include
    -I${entt_includes}
    # include
    -I${basis_HEADER_DIR}
    # include
    -I${corrade_includes}
    # Get llvm version that iwyu depends on:
    # strings /usr/bin/iwyu | grep LLVM
    # Then install required llvm version:
    # apt-get install clang-10 clang-tools-10 clang-10-doc libclang-common-10-dev
    # Use the proper include directory,
    # for clang-* it would be /usr/lib/llvm-*/lib/clang/*/include/.
    # locate stddef.h | sed -ne '/^\/usr/p'
    # see https://github.com/include-what-you-use/include-what-you-use/issues/679
    #-nostdinc++
    #-nodefaultlibs
    # NOTE: you can get `clang_8.0.0` like so:
    # curl -SL http://releases.llvm.org/8.0.0/clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz | tar -xJC .
    # mv clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-18.04 clang_8.0.0
    # mv clang_8.0.0 /usr/local
    #-isystem/usr/lib/llvm-10/lib/clang/10.0.0/include/
    -nostdinc++
    -nodefaultlibs
    # see https://github.com/include-what-you-use/include-what-you-use/issues/802
    -isystem${CONAN_LLVM_TOOLS_ROOT}/include/c++/v1/
    -isystem${CONAN_LLVM_TOOLS_ROOT}/lib/clang/10.0.1/include/
    # -Xiwyu --transitive_includes_only
    # pch_in_code: The file has an important header first
    # -Xiwyu --pch_in_code
    # no_comments: Do not add notes to the output
    -Xiwyu --no_comments
    -Xiwyu --no_default_mappings
    # mapping_file: lookup file
    -Xiwyu --mapping_file=${IWYU_IMP}
    # see https://github.com/include-what-you-use/include-what-you-use/issues/760
    -Wno-unknown-warning-option
    -Wno-error
    # when sorting includes, place quoted ones first.
    -Xiwyu --quoted_includes_first
    # suggests the more concise syntax introduced in C++17
    -Xiwyu --cxx17ns
    # max_line_length: maximum line length for includes.
    # Note that this only affects comments and alignment thereof,
    # the maximum line length can still be exceeded
    # with long file names (default: 80).
    -Xiwyu --max_line_length=180
    #-Xiwyu --check_also=${CMAKE_CURRENT_SOURCE_DIR}/src/*
    #-Xiwyu --check_also=${CMAKE_CURRENT_SOURCE_DIR}/src/*/*
    #-Xiwyu --check_also=${CMAKE_CURRENT_SOURCE_DIR}/src/*/*/*
    #-Xiwyu --check_also=${CMAKE_CURRENT_SOURCE_DIR}/src/*/*/*/*
  #VERBOSE
  REQUIRED
  CHECK_TARGETS_DEPEND
)
