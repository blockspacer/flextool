cmake_minimum_required( VERSION 3.13.3 FATAL_ERROR )

set(ROOT_PROJECT_NAME ${LIB_NAME})
set(ROOT_PROJECT_LIB ${LIB_NAME})
set(${ROOT_PROJECT_NAME}_CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

set( PROJECT_NAME "${ROOT_PROJECT_NAME}-tests" )
set( PROJECT_DESCRIPTION "unit tests" )
set( ${PROJECT_NAME}_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "${PROJECT_NAME}_PROJECT_DIR" )

# Get CMAKE_MODULE_PATH from parent project
#include( ${ROOT_PROJECT_DIR}/cmake/Utils.cmake )
#set_cmake_module_paths( ${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR};${${ROOT_PROJECT_NAME}_CMAKE_MODULE_PATH}" ) # from Utils.cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR};${${ROOT_PROJECT_NAME}_CMAKE_MODULE_PATH}")

# Run ALL tests with verbose output
# Usage: cmake --build build --target run_all_tests
add_custom_target(${ROOT_PROJECT_NAME}_run_all_tests
    DEPENDS ${ROOT_PROJECT_LIB}
)

if (ENABLE_CODE_COVERAGE)
  set( CATCH2_TEST_ARGS -r junit -o unit_test_result.xml )
else()
  set( CATCH2_TEST_ARGS -r console --use-colour yes --order rand --durations yes --warn NoTests --invisibles)
endif()

set( GTEST_TEST_ARGS --gtest_shuffle)

add_library(${ROOT_PROJECT_NAME}-catch2-root INTERFACE)
target_include_directories( ${ROOT_PROJECT_NAME}-catch2-root SYSTEM INTERFACE
  ${CONAN_CATCH2_ROOT}/include/catch2 )
get_target_property (FakeIt_IMPORTED_LOCATION CONAN_PKG::catch2 INTERFACE_INCLUDE_DIRECTORIES)
message( "CONAN_PKG::catch2=${catch2_IMPORTED_LOCATION}" )
get_target_property (FakeIt_IMPORTED_LOCATION CONAN_PKG::FakeIt INTERFACE_INCLUDE_DIRECTORIES)
message( "CONAN_PKG::FakeIt=${FakeIt_IMPORTED_LOCATION}" )

list(APPEND TESTS_3DPARTY_LIBS
    #FakeIt
    CONAN_PKG::conan_gtest
    CONAN_PKG::FakeIt
    CONAN_PKG::catch2
    ${ROOT_PROJECT_NAME}-catch2-root # so FakeIt will be able to see 'catch.hpp'
    ${ROOT_PROJECT_NAME}-test-includes
    ${base_LIB}
    ${basis_LIB}
    ${build_util_LIB}
)

include( testRunner ) # start tests as CMake targets

#findPackageCrossPlatform( Sanitizers ) # see https://github.com/arsenm/sanitizers-cmake
# sanitizer_add_blacklist_file("blacklist.txt") # https://github.com/arsenm/sanitizers-cmake#build-targets-with-sanitizer-support

macro(add_to_tests_list source_list)
  list(APPEND UNIT_TEST_SOURCE_LIST ${source_list})
endmacro()

macro(set_test_compile_options target)
  # FIX for undefined reference to `typeinfo for clang::Decl'
  target_compile_options(${target} PRIVATE
    -frtti)

  target_compile_options(${target} PRIVATE
              $<$<CXX_COMPILER_ID:MSVC>:
              /W3 # Set warning level
              #/WX # Treats all compiler warnings as errors.
              >
              $<$<CXX_COMPILER_ID:GNU>:
              -Wformat=2
              -Wall
              -W
##              "$<$<CONFIG:RELEASE>:-Werror>" # Treats all compiler warnings as errors.
              -Wpedantic
              #-fsanitize-address-use-after-scope # https://clang.llvm.org/docs/AddressSanitizer.html
              #-fsanitize=pointer-compare # https://kristerw.blogspot.com/2018/06/useful-gcc-address-sanitizer-checks-not.html
              -fno-omit-frame-pointer # https://github.com/google/sanitizers/wiki/AddressSanitizer#using-addresssanitizer
              >
              $<$<CXX_COMPILER_ID:Clang>:
              -Wformat=2
              -Wall
              -W
##              "$<$<CONFIG:RELEASE>:-Werror>" # Treats all compiler warnings as errors.
              -Wpedantic
              -Wdeprecated-register
              #-fsanitize-address-use-after-scope # https://clang.llvm.org/docs/AddressSanitizer.html
              #-fsanitize=pointer-compare # https://kristerw.blogspot.com/2018/06/useful-gcc-address-sanitizer-checks-not.html
              -fno-omit-frame-pointer # https://github.com/google/sanitizers/wiki/AddressSanitizer#using-addresssanitizer
              >
          )

  # @see https://github.com/eranpeer/FakeIt
  target_compile_options( ${target} PRIVATE
    # If you don't use /ZI, you will have exceptions mocking destructors (which includes unique_ptr and other smart pointers).
    $<$<CXX_COMPILER_ID:MSVC>:/ZI>
    # On GCC, optimization flag O2 and O3 are not supported. You must compile the test project with -O1 or -O0.
    $<$<CXX_COMPILER_ID:GNU>:-O0>
    $<$<CXX_COMPILER_ID:Clang>:-O0> )

  set_target_properties( ${target} PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    CMAKE_CXX_STANDARD_REQUIRED ON
    CMAKE_CXX_FLAGS "-rtti /std:c++latest"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endmacro()

macro(tests_add_executable target source_list TEST_ARGS TEST_LIB)
  list(APPEND UNIT_TEST_TARGETS ${target})

  add_to_tests_list(${source_list})

  add_executable(${target} ${source_list})

  if (NOT CMAKE_BUILD_TYPE MATCHES "Debug" )
##    add_sanitizers( ${target} ) # https://github.com/arsenm/sanitizers-cmake#build-targets-with-sanitizer-support
  endif()

  target_link_libraries(${target} PRIVATE
    # 3dparty libs
    ${TESTS_3DPARTY_LIBS}
    # system libs
    ${USED_SYSTEM_LIBS}
    # main project lib
    ${ROOT_PROJECT_LIB}
    ${TEST_LIB}
  )

  set_test_compile_options( ${target} )

  add_test(
    NAME ${target}
    COMMAND ${target} ${TEST_ARGS})

  add_test_command_targets( ${target} )

  if(ENABLE_VALGRIND)
    # |base::TestSuite| tries to kill test process
    # but valgrind wraps process, so nothing to kill
    target_compile_definitions( ${target} PUBLIC
      GTEST_NO_SUITE=1) # TODO: valgrind may not support chromium base, FIXME. And remove GTEST_NO_SUITE
  endif(ENABLE_VALGRIND)

  # USAGE:
  # cmake -E time cmake --build . --target TARGET_NAME_run_valgrind
  valgrind_enabler(
    IS_ENABLED
      ${ENABLE_VALGRIND}
    STANDALONE_TARGET
      ${target}
    EXTRA_OPTIONS # see https://valgrind.org/docs/manual/manual-core.html
      # When enabled, Valgrind will trace into sub-processes
      # initiated via the exec system call.
      # This is necessary for multi-process programs.
      --trace-children=yes
      # --leak-check=full:
      # each individual leak will be shown in detail
      --leak-check=full
      # --show-leak-kinds=all:
      # Show all of "definite, indirect, possible, reachable"
      # leak kinds in the "full" report.
      --show-leak-kinds=all
      # --track-origins=yes:
      # Favor useful output over speed.
      # This tracks the origins of uninitialized values,
      # which could be very useful for memory errors.
      # Consider turning off if Valgrind is unacceptably slow.
      --track-origins=yes
      # --verbose:
      # Can tell you about unusual behavior of your program.
      # Repeat for more verbosity.
      --verbose
      --show-reachable=yes
      # When enabled, Valgrind stops reporting errors
      # after 10,000,000 in total, or 1,000 different ones,
      # have been seen.
      --error-limit=yes
      # is option is particularly useful with C++ programs,
      # as it prints out the suppressions with mangled names,
      # as required.
      --gen-suppressions=all
      # Write to a file.
      # Useful when output exceeds terminal space.
      --log-file=${CMAKE_CURRENT_BINARY_DIR}/${target}_valgrind_raw.log
      # executable
      $<TARGET_FILE:${target}>
      # arguments of executable
      ${TEST_ARGS}
    VERBOSE
    REQUIRED
  )
  if(ENABLE_VALGRIND AND ENABLE_VALGRIND_TESTS)
    add_dependencies(${ROOT_PROJECT_NAME}_run_all_tests
      ${target}_run_valgrind
    )
  endif(ENABLE_VALGRIND AND ENABLE_VALGRIND_TESTS)

  if(AUTORUN_TESTS)
    add_test_autostarter( ${target} )
  endif()
endmacro()


set(test_main_catch "${ROOT_PROJECT_NAME}-test_main_catch")
add_library( ${test_main_catch} OBJECT
  main.cpp
  #${${ROOT_PROJECT_NAME}_SRCS} # all source files  of root project without main.cpp
  #${THIRDPARTY_SOURCES}
  testsCommon.h # include in IDE
  )

set_property(TARGET ${test_main_catch} PROPERTY CXX_STANDARD 17)

# FIX for undefined reference to `typeinfo for clang::Decl'
target_compile_options(${test_main_catch} PRIVATE
  -frtti)

# ensure that dependencies build before <target> does.
add_dependencies(${test_main_catch} ${ROOT_PROJECT_LIB})

set_test_compile_options( ${test_main_catch} )

target_link_libraries(${test_main_catch} PUBLIC
  # 3dparty libs
  ${TESTS_3DPARTY_LIBS}
  # system libs
  ${USED_SYSTEM_LIBS}
  # main project lib
  ${ROOT_PROJECT_LIB}
)

target_compile_definitions( ${test_main_catch} PUBLIC
  "-DUSE_CATCH_TEST=1" )

if(ENABLE_VALGRIND)
  # |base::TestSuite| tries to kill test process
  # but valgrind wraps process, so nothing to kill
  target_compile_definitions( ${test_main_catch} PUBLIC
    GTEST_NO_SUITE=1) # TODO: valgrind may not support chromium base, FIXME. And remove GTEST_NO_SUITE
endif(ENABLE_VALGRIND)

set(test_main_gtest "${ROOT_PROJECT_NAME}-test_main_gtest")
add_library( ${test_main_gtest} OBJECT
  main.cpp
  #${${ROOT_PROJECT_NAME}_SRCS} # all source files  of root project without main.cpp
  #${THIRDPARTY_SOURCES}
  testsCommon.h # include in IDE
  )

set_property(TARGET ${test_main_gtest} PROPERTY CXX_STANDARD 17)

# FIX for undefined reference to `typeinfo for clang::Decl'
target_compile_options(${test_main_gtest} PRIVATE
  -frtti)

# ensure that dependencies build before <target> does.
add_dependencies(${test_main_gtest} ${ROOT_PROJECT_LIB})

set_test_compile_options( ${test_main_gtest} )

target_link_libraries(${test_main_gtest} PUBLIC
  # 3dparty libs
  ${TESTS_3DPARTY_LIBS}
  # system libs
  ${USED_SYSTEM_LIBS}
  # main project lib
  ${ROOT_PROJECT_LIB}
)

target_compile_definitions( ${test_main_gtest} PUBLIC
  "-DUSE_GTEST_TEST=1" )

if(ENABLE_VALGRIND)
  # |base::TestSuite| tries to kill test process
  # but valgrind wraps process, so nothing to kill
  target_compile_definitions( ${test_main_gtest} PUBLIC
    GTEST_NO_SUITE=1) # TODO: valgrind may not support chromium base, FIXME. And remove GTEST_NO_SUITE
endif(ENABLE_VALGRIND)

#target_link_libraries( test_main PUBLIC
#  sol2::sol2_single Catch FakeIt boost_beast nlohmann_json microsoft_gsl boost_outcome better_enums )

#target_compile_definitions( test_main PUBLIC
#  ${WEBRTC_DEFINITIONS} ${RAPIDJSON_DEFINITIONS} )

#target_link_directories( test_main PUBLIC ${WEBRTC_LIB_PATHS} )

#target_include_directories( test_main SYSTEM PUBLIC
#  ${ROOT_PROJECT_DIR}/src
#  ${WEBRTC_INCLUDES}
#  ${THIRDPARTY_FILES}
#  ${ABSEIL_BASE_IMPORTED_LOCATION})

option(AUTORUN_TESTS "Autorun tests" OFF) # see add_test_autostarter

option(${ROOT_PROJECT_NAME}_BUILD_TESTS "Enable tests" ON)
if(${ROOT_PROJECT_NAME}_BUILD_TESTS)
  message( "${PROJECT_NAME} testing enabled" )

  set ( gmock_deps
    gmock.test.cpp
  )
  tests_add_executable(${ROOT_PROJECT_NAME}-gmock
    "${gmock_deps}" "${GTEST_TEST_ARGS}" "${test_main_gtest}")

  set ( fakeit_deps
    fakeit.test.cpp
  )
  tests_add_executable(${ROOT_PROJECT_NAME}-fakeit
    "${fakeit_deps}" "${CATCH2_TEST_ARGS}" "${test_main_catch}")

  # TODO: remove dummy_test after
  # FIXME see https://stackoverflow.com/questions/25922895/clang-link-time-optimization-with-replaced-operator-new-causes-mismatched-free

  #add_executable(dummy_test
  #  dummy_test.cpp)
  #
  #set_property(TARGET dummy_test
  #  PROPERTY CXX_STANDARD 17)
  #
  #target_link_libraries(dummy_test PRIVATE
  #  ${base_LIB}
  #  ${basis_LIB}
  #  #${build_util_LIB}
  #  ## 3dparty libs
  #  #${TESTS_3DPARTY_LIBS}
  #  ## system libs
  #  #${USED_SYSTEM_LIBS}
  #  ## main project lib
  #  #${ROOT_PROJECT_LIB}
  #  #${TEST_LIB}
  #)
  #
  ## USAGE:
  ## cmake -E time cmake --build . --target TARGET_NAME_run_valgrind
  #valgrind_enabler(
  #  IS_ENABLED
  #    ${ENABLE_VALGRIND}
  #  STANDALONE_TARGET
  #    dummy_test
  #  EXTRA_OPTIONS # see https://valgrind.org/docs/manual/manual-core.html
  #    # When enabled, Valgrind will trace into sub-processes
  #    # initiated via the exec system call.
  #    # This is necessary for multi-process programs.
  #    --trace-children=yes
  #    # --leak-check=full:
  #    # each individual leak will be shown in detail
  #    --leak-check=full
  #    # --show-leak-kinds=all:
  #    # Show all of "definite, indirect, possible, reachable"
  #    # leak kinds in the "full" report.
  #    --show-leak-kinds=all
  #    # --track-origins=yes:
  #    # Favor useful output over speed.
  #    # This tracks the origins of uninitialized values,
  #    # which could be very useful for memory errors.
  #    # Consider turning off if Valgrind is unacceptably slow.
  #    --track-origins=yes
  #    # --verbose:
  #    # Can tell you about unusual behavior of your program.
  #    # Repeat for more verbosity.
  #    --verbose
  #    --show-reachable=yes
  #    # When enabled, Valgrind stops reporting errors
  #    # after 10,000,000 in total, or 1,000 different ones,
  #    # have been seen.
  #    --error-limit=yes
  #    # is option is particularly useful with C++ programs,
  #    # as it prints out the suppressions with mangled names,
  #    # as required.
  #    --gen-suppressions=all
  #    # Write to a file.
  #    # Useful when output exceeds terminal space.
  #    --log-file=${CMAKE_CURRENT_BINARY_DIR}/dummy_test_valgrind_raw.log
  #    # executable
  #    $<TARGET_FILE:dummy_test>
  #    # arguments of executable
  #    #${TEST_ARGS}
  #  VERBOSE
  #  REQUIRED
  #)

  #add_to_tests_list(utils)

  #tests_add_executable(check_all ${UNIT_TEST_SOURCE_LIST} ${GTEST_TEST_ARGS})
  #message(FATAL_ERROR ${UNIT_TEST_SOURCE_LIST})

  message(STATUS "flextool: copying ${CMAKE_CURRENT_SOURCE_DIR}/data")
  # "gmock" is one of test program names
  add_custom_command( TARGET ${ROOT_PROJECT_NAME}-gmock POST_BUILD
                      COMMAND ${CMAKE_COMMAND} -E copy_directory
                          ${CMAKE_CURRENT_SOURCE_DIR}/data
                          ${CMAKE_CURRENT_BINARY_DIR})

  enable_testing()
endif()

foreach(target IN LISTS ALL_TESTS_LIST_VERBOSE)
  add_dependencies(${ROOT_PROJECT_NAME}_run_all_tests
    ${target}
  )
endforeach()

option(ENABLE_CODE_COVERAGE "ENABLE CODE COVERAGE" OFF)
if (ENABLE_CODE_COVERAGE)
  set(LCOV_REMOVE_EXTRA "'*/src/include/webrtc/*'" "'${ROOT_PROJECT_DIR}/lib/*'" "'${ROOT_PROJECT_DIR}/submodules/*'" "'test/*'" "'ext/*'" "'abseil/*'" "'submodules/*'" "'*/tests/*'" "'scripts/*'" "'lib/*'" "'build/*'" "'bin/*'" "'*Catch/*'" "'*FakeIt/*'")
  set(CMAKE_BUILD_TYPE Coverage)
  # test coverage build configuration for C++: debug build, no optimization, profiling enabled
  #set(CMAKE_CXX_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
  #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")

  target_compile_options(${ROOT_PROJECT_NAME}_run_all_tests PRIVATE
    -g -O0 -Wall -fprofile-arcs -ftest-coverage
  )
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
  # include the cmake code coverage module
  include( CodeCoverage )
  # cmake code coverage module target
  setup_target_for_coverage(
    # Name for custom target.
    unit_tests_coverage
    # Name of the test driver executable that runs the tests.
    # NOTE! This should always have a ZERO as exit code
    # otherwise the coverage generation will not complete.
    check_all
    coverage # Name of output directory.
    )
  message("added make code coverage for ${PROJECT_NAME}_coverage")
else()
  message(WARNING "code coverage off")
endif()

#find_package(Sanitizers)
#if(HAS_Address_SANITIZER AND HAS_Fuzzer_SANITIZER)
#	add_executable(fuzz fuzzing.cpp)
#	target_link_libraries(fuzz PUBLIC gltfpp Sanitizer::Fuzzer Sanitizer::Address)
#endif()
