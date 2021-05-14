# Creates test runner that specifies
# * custom `int main()`
# * custom dirs to include
# * custom libs to link
# * some helper cmake functions/macros

set(perf_test_runner "${ROOT_PROJECT_NAME}-perf_test_runner")
add_library( ${perf_test_runner} OBJECT
  ${CMAKE_CURRENT_SOURCE_DIR}/../flextool/test/run_all_perftests.cc
  )

set_property(TARGET ${perf_test_runner} PROPERTY CXX_STANDARD 17)

# ensure that dependencies build before <target> does.
add_dependencies(${perf_test_runner} ${ROOT_PROJECT_LIB})

set_tests_default_compile_options( ${perf_test_runner} )

target_link_libraries(${perf_test_runner} PUBLIC
  # 3dparty libs
  ${TESTS_3DPARTY_LIBS}
  # system libs
  ${USED_SYSTEM_LIBS}
  # main project lib
  ${ROOT_PROJECT_LIB}
  CONAN_PKG::conan_gtest
)

target_compile_definitions( ${perf_test_runner} PUBLIC
  BUILD_DATE=FLEXTOOL_BUILD_DATE
  USE_GTEST_TEST=1
  GTEST_PERF_SUITE=1
  PERF_TEST=1)
