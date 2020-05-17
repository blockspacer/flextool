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
    CONAN_PKG::gtest
    CONAN_PKG::FakeIt
    CONAN_PKG::catch2
    ${ROOT_PROJECT_NAME}-catch2-root # so FakeIt will be able to see 'catch.hpp'
    ${USED_3DPARTY_LIBS}
    ${ROOT_PROJECT_NAME}-test-includes
)

include( testRunner ) # start tests as CMake targets

#findPackageCrossPlatform( Sanitizers ) # see https://github.com/arsenm/sanitizers-cmake
# sanitizer_add_blacklist_file("blacklist.txt") # https://github.com/arsenm/sanitizers-cmake#build-targets-with-sanitizer-support

macro(add_to_tests_list source_list)
  list(APPEND UNIT_TEST_SOURCE_LIST ${source_list})
endmacro()

macro(set_test_compile_options target)
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
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/tests )
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

set(test_main_gtest "${ROOT_PROJECT_NAME}-test_main_gtest")
add_library( ${test_main_gtest} OBJECT
  main.cpp
  #${${ROOT_PROJECT_NAME}_SRCS} # all source files  of root project without main.cpp
  #${THIRDPARTY_SOURCES}
  testsCommon.h # include in IDE
  )

set_property(TARGET ${test_main_gtest} PROPERTY CXX_STANDARD 17)

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

  set ( pimpl_deps
    pimpl.test.cpp
  )
  tests_add_executable(${ROOT_PROJECT_NAME}-pimpl
    "${pimpl_deps}" "${CATCH2_TEST_ARGS}" "${test_main_catch}")

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
endif()

#add_to_tests_list(utils)
#add_to_tests_list(pimpl)

#tests_add_executable(check_all ${UNIT_TEST_SOURCE_LIST} ${GTEST_TEST_ARGS})
#message(FATAL_ERROR ${UNIT_TEST_SOURCE_LIST})

message(STATUS "flextool: copying ${CMAKE_CURRENT_SOURCE_DIR}/data")
# "pimpl" is one of test program names
add_custom_command( TARGET ${ROOT_PROJECT_NAME}-pimpl POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/tests/data )

enable_testing()

# Run ALL tests with verbose output
# Usage: cmake --build build --target run_all_tests
add_custom_target(${ROOT_PROJECT_NAME}_run_all_tests
    DEPENDS ${ROOT_PROJECT_LIB} ${ALL_TESTS_LIST_VERBOSE}
)
foreach(target IN LISTS ALL_TESTS_LIST_VERBOSE)
  add_dependencies(${ROOT_PROJECT_NAME}_run_all_tests ${target})
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

option(ENABLE_VALGRIND_TESTS "ENABLE CODE COVERAGE" OFF)
if (ENABLE_VALGRIND_TESTS)
  find_program( CMAKE_MEMORYCHECK_COMMAND valgrind )
  set( CMAKE_MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full" )
  #set( MEMORYCHECK_SUPPRESSIONS_FILE "${PROJECT_SOURCE_DIR}/valgrind_suppress.txt" )

  get_property(check_all_dir TARGET check_all PROPERTY RUNTIME_OUTPUT_DIRECTORY)
  add_memcheck_test( memcheck_all_tests "${check_all_dir}/check_all" )
else()
  message(WARNING "valgrind tests off")
endif()

#find_package(Sanitizers)
#if(HAS_Address_SANITIZER AND HAS_Fuzzer_SANITIZER)
#	add_executable(fuzz fuzzing.cpp)
#	target_link_libraries(fuzz PUBLIC gltfpp Sanitizer::Fuzzer Sanitizer::Address)
#endif()
