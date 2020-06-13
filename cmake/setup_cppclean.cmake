include_guard( DIRECTORY )

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_cppclean
cppclean_enabler(
  PATHS
    #${CONAN_BIN_DIRS}
    ${CONAN_BIN_DIRS_LLVM_TOOLS}
  #NO_SYSTEM_ENVIRONMENT_PATH
  #NO_CMAKE_SYSTEM_PATH
  IS_ENABLED
    ${ENABLE_CPPCLEAN}
  CHECK_TARGETS
    ${LIB_NAME}
  EXTRA_OPTIONS
    --include-path ${CMAKE_CURRENT_SOURCE_DIR}/include
    #--include-path-non-system ${CMAKE_CURRENT_SOURCE_DIR}/include
    --verbose
    --exclude "*generated*"
    #-extra-arg=-std=c++17
    #-extra-arg=-Qunused-arguments
    #-extra-arg=-DBOOST_SYSTEM_NO_DEPRECATED
    #-extra-arg=-DBOOST_ERROR_CODE_HEADER_ONLY
  VERBOSE
  REQUIRED
)
