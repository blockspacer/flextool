# Creates test runner that specifies
# * custom `int main()`
# * custom dirs to include
# * custom libs to link
# * some helper cmake functions/macros

set(gtest_test_runner "${ROOT_PROJECT_NAME}-gtest_test_runner")
add_library( ${gtest_test_runner} OBJECT
  ${CMAKE_CURRENT_SOURCE_DIR}/../flextool/test/run_all_unittests.cc
)

set_property(TARGET ${gtest_test_runner} PROPERTY CXX_STANDARD 17)

# ensure that dependencies build before <target> does.
add_dependencies(${gtest_test_runner} ${ROOT_PROJECT_LIB})

set_tests_default_compile_options( ${gtest_test_runner} )

target_link_libraries(${gtest_test_runner} PUBLIC
  # 3dparty libs
  ${TESTS_3DPARTY_LIBS}
  # system libs
  ${USED_SYSTEM_LIBS}
  # main project lib
  ${ROOT_PROJECT_LIB}
  CONAN_PKG::conan_gtest
)

target_compile_definitions( ${gtest_test_runner} PUBLIC
  BUILD_DATE=FLEXTOOL_BUILD_DATE
  USE_GTEST_TEST=1 )

macro(flextool_test_gtest test_name source_list)
  # NOTE: See `/test/launcher/unit_test_launcher.cc` and command-line options
  # for Google Test program for full list of command-line arguments.
  set( GTEST_TEST_ARGS
    "--test-launcher-retry-limit=1"
    "--test-launcher-timeout=600000"
    "--gtest_repeat=1"
    "--gtest_shuffle"
    "--test-data-dir=${CMAKE_CURRENT_SOURCE_DIR}/data/"
    "--icu-data-file=${CMAKE_CURRENT_BINARY_DIR}/${TESTS_BINARY_DIR_NAME}/resources/icu/optimal/icudt68l.dat")

  flextool_test("${test_name}" "${source_list}" "${GTEST_TEST_ARGS}" "${gtest_test_runner}")
endmacro()
