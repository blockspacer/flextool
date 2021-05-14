include_guard( DIRECTORY )

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_run_cppcheck
cppcheck_enabler(
  PATHS
    # to use cppcheck_installer from conan
    #${CONAN_BIN_DIRS}
    ${CONAN_BIN_DIRS_LLVM_TOOLS}
    # to use from cmake subfolder
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
  NO_SYSTEM_ENVIRONMENT_PATH
  NO_CMAKE_SYSTEM_PATH
  IS_ENABLED
    ${ENABLE_CPPCHECK}
  CHECK_TARGETS
    ${FLEXTOOL_LIB_NAME}
  EXTRA_OPTIONS
    # check all #ifdef
    #--force
    #-j6 # thread count
    --language=c++
    # inconclusive = more checks
    # NOTE: There are false positives with this option.
    --inconclusive
    --enable=all
    #--enable=warning,performance,portability,information,missingInclude
    # Give path to ignore. Give several -i parameters to ignore several paths.
    -igenerated
    --std=c++20
    # include
    -I${basis_HEADER_DIR}
    # ignore
    -i${basis_HEADER_DIR}
    # include
    #-I${cling_includes}
    # ignore
    #-i${cling_includes}
    # include
    #-I${clang_includes}
    # ignore
    #-i${clang_includes}
    # include
    -I${corrade_includes}
    # ignore
    -i${corrade_includes}
    # corrade support
    -DDOXYGEN_GENERATING_OUTPUT=1
    # include
    -I${entt_includes}
    -DNDEBUG
    --max-configs=100
    ${cppcheck_linux_defines}
    # undef
    -USTARBOARD
    # undef
    -UCOBALT
    # less info in console
    #--quiet
    # missingIncludeSystem:
    # Cppcheck does not need standard library headers
    # to get proper results.
    #--suppress=missingIncludeSystem
    # preprocessorErrorDirective:
    # support for
    # __has_include(<boost/filesystem.hpp>)
    #--suppress=preprocessorErrorDirective
  HTML_REPORT TRUE
  VERBOSE
  REQUIRED
)
