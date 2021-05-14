cmake_minimum_required( VERSION 3.13.3 FATAL_ERROR )

set(ROOT_PROJECT_NAME ${PROJECT_NAME})
set(ROOT_PROJECT_LIB ${FLEXTOOL_LIB_NAME})
set(${ROOT_PROJECT_NAME}_CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
set( TESTS_BINARY_DIR_NAME "${FLEXTOOL_LIB_NAME}-tests" )

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR};${${ROOT_PROJECT_NAME}_CMAKE_MODULE_PATH}")

macro(flextool_test test_name source_list TEST_ARGS TEST_RUNNER_LIB)
  set(single_unittest_target run_test_${test_name})

  # add_test_executable_and_targets from cmake_helper_utils
  add_test_executable_and_targets("${test_name}" "${single_unittest_target}" "${source_list}" "${TEST_ARGS}")
  list(APPEND ${ROOT_PROJECT_NAME}_CTEST_TARGETS ${single_unittest_target})

  target_link_libraries(${test_name} PUBLIC
    ${USED_3DPARTY_LIBS}
    ${ROOT_PROJECT_LIB}-test-includes
    ${ROOT_PROJECT_LIB}
    ${USED_SYSTEM_LIBS}
    ${TEST_RUNNER_LIB}
  )

  add_dependencies(${test_name} ${FLEXTOOL_LIB_NAME}_test_data_files)

  target_include_directories(${test_name} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
  )

  set_tests_default_compile_options( ${test_name} )

  set_target_properties( ${test_name} PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    CMAKE_CXX_STANDARD_REQUIRED ON
    CMAKE_CXX_FLAGS "-fno-rtti"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${TESTS_BINARY_DIR_NAME} )
endmacro()

include(${CMAKE_CURRENT_SOURCE_DIR}/test_data_files.cmake)

# Test runners (gtest/catch/etc.)
include(${CMAKE_CURRENT_SOURCE_DIR}/gtest_test_runner.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/perf_test_runner.cmake)

# Test source files
include(${CMAKE_CURRENT_SOURCE_DIR}/test_sources.cmake)

# Run all unittests
add_custom_target(${ROOT_PROJECT_NAME}_run_unittests
  COMMAND ${CMAKE_COMMAND} -E echo ----------------------------------
  COMMENT "Running unittests"
  DEPENDS ${${ROOT_PROJECT_NAME}_CTEST_TARGETS}
)
