include_guard( DIRECTORY )

set(EXTRA_CLANG_FORMAT_FILES
  # place here files what need to be to be formatted
)

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_clang_format
clang_format_enabler(
  PATHS
    #${CONAN_BIN_DIRS}
    ${CONAN_BIN_DIRS_LLVM_TOOLS}
  NO_SYSTEM_ENVIRONMENT_PATH
  NO_CMAKE_SYSTEM_PATH
  IS_ENABLED
    ${ENABLE_CLANG_FORMAT}
  CHECK_TARGETS
    ${FLEXTOOL_LIB_NAME}
  EXTRA_OPTIONS
    # ferror-limit:
    # Used only with --dry-run or -n
    --ferror-limit=9999
    # Use -style=file to load style configuration from
    # .clang-format file located in one of the parent
    # directories of the source file (or current
    # directory for stdin).
    -style=file
    #  The -i option indicates to apply the formatting options
    # to the files in-place, rather than spitting out
    # the formatted version of the files to the command line.
    -i
    ${EXTRA_CLANG_FORMAT_FILES}
  VERBOSE
  REQUIRED
)
