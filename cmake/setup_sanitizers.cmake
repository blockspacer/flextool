include_guard( DIRECTORY )

if(ENABLE_MSAN OR ENABLE_TSAN OR ENABLE_ASAN OR ENABLE_UBSAN)
  if(cmake_build_type_tolower MATCHES "release" )
    message(WARNING
      "Disable sanitizers in Release builds")
  endif()

  # use llvm_tools from conan
  find_program_helper(llvm-symbolizer
    PATHS
      #${CONAN_BIN_DIRS}
      ${CONAN_BIN_DIRS_LLVM_TOOLS}
    NO_SYSTEM_ENVIRONMENT_PATH
    NO_CMAKE_SYSTEM_PATH
    ${ARGUMENTS_UNPARSED_ARGUMENTS}
    REQUIRED
    OUT_VAR LLVM_SYMBOLIZER_PROGRAM
    VERBOSE TRUE
  )

  check_sanitizer_options(
    ENABLE_TSAN ${ENABLE_TSAN}
    ENABLE_ASAN ${ENABLE_ASAN}
    ENABLE_MSAN ${ENABLE_MSAN}
    ENABLE_UBSAN ${ENABLE_UBSAN}
    LLVM_SYMBOLIZER_PROGRAM ${LLVM_SYMBOLIZER_PROGRAM}
  )
endif()

if(ENABLE_MSAN)
  message(STATUS "enabling MSAN on ${FLEXTOOL_LIB_NAME}")
  add_msan_static_link(${FLEXTOOL_LIB_NAME})
  add_msan_definitions(${FLEXTOOL_LIB_NAME})
  add_msan_flags()
else(ENABLE_MSAN)
  message(STATUS
    "Disabled ENABLE_MSAN")
endif(ENABLE_MSAN)

if(ENABLE_TSAN)
  message(STATUS "enabling TSAN on ${FLEXTOOL_LIB_NAME}")
  add_tsan_static_link(${FLEXTOOL_LIB_NAME})
  add_tsan_definitions(${FLEXTOOL_LIB_NAME})
  add_tsan_flags()
else(ENABLE_TSAN)
  message(STATUS
    "Disabled ENABLE_TSAN")
endif(ENABLE_TSAN)

if(ENABLE_ASAN)
  message(STATUS "enabling ASAN on ${FLEXTOOL_LIB_NAME}")
  add_asan_static_link(${FLEXTOOL_LIB_NAME})
  add_asan_definitions(${FLEXTOOL_LIB_NAME})
  add_asan_flags()
else(ENABLE_ASAN)
  message(STATUS
    "Disabled ENABLE_ASAN")
endif(ENABLE_ASAN)

if(ENABLE_UBSAN)
  message(STATUS "enabling UBSAN on ${FLEXTOOL_LIB_NAME}")
  add_ubsan_static_link(${FLEXTOOL_LIB_NAME})
  add_ubsan_definitions(${FLEXTOOL_LIB_NAME})
  add_ubsan_flags()
else(ENABLE_UBSAN)
  message(STATUS
    "Disabled ENABLE_UBSAN")
endif(ENABLE_UBSAN)
