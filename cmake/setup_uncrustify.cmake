include_guard( DIRECTORY )

set(EXTRA_UNCRUSTIFY_FILES
  # place here files what need to be to be formatted
)

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_uncrustify
uncrustify_enabler(
  PATHS
    #${CONAN_BIN_DIRS}
    ${CONAN_BIN_DIRS_LLVM_TOOLS}
  IS_ENABLED
    ${ENABLE_UNCRUSTIFY}
  CHECK_TARGETS
    ${LIB_NAME}
  EXTRA_OPTIONS
    --no-backup
    -c ${CMAKE_CURRENT_SOURCE_DIR}/uncrustify.cfg
    ${EXTRA_UNCRUSTIFY_FILES}
  VERBOSE
  REQUIRED
)
