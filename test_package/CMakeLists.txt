cmake_minimum_required(VERSION 3.1.0)
project(flextool_test_package CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(
  # prevent conan_basic_setup from resetting cmake variables
  TARGETS
  KEEP_RPATHS
  # see https://github.com/conan-io/conan/issues/6012
  NO_OUTPUT_DIRS
)

message(STATUS "Loading conan scripts for {0} dependencies...")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
include("${CMAKE_BINARY_DIR}/conan_paths.cmake" OPTIONAL)
message(STATUS "Doing conan basic setup")
conan_basic_setup()

list(APPEND CMAKE_PROGRAM_PATH ${CONAN_BIN_DIRS})

# TODO: make better: link lib dirs
link_directories(${CONAN_LIB_DIRS})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(HelperFlextool)

find_package(Threads)

add_executable(${PROJECT_NAME}
  test_package.cpp
)

if(NOT TARGET CONAN_PKG::flexlib)
  message(FATAL_ERROR "Use flexlib from conan")
endif()

if(NOT TARGET CONAN_PKG::chromium_build_util)
  message(FATAL_ERROR "Use chromium_build_util from conan")
endif()

find_package(chromium_build_util REQUIRED)
#
if(TARGET chromium_build_util::chromium_build_util-static)
  set(build_util_LIB "chromium_build_util::chromium_build_util-static")
else()
  message(FATAL_ERROR "not supported ${build_util_LIB}: using system provided chromium_build_util library")
endif()

find_package(chromium_base REQUIRED)
if(NOT TARGET ${base_LIB})
  message(FATAL_ERROR "not supported ${base_LIB}: using system provided chromium_base library")
endif()

find_package(flexlib REQUIRED)
if(NOT TARGET ${flexlib_LIB})
  message(FATAL_ERROR "not supported ${flexlib_LIB}: using system provided flexlib library")
endif()

target_link_libraries(${PROJECT_NAME}
  ${CMAKE_THREAD_LIBS_INIT}
  ${base_LIB}
  ${flexlib_LIB}
  ${build_util_LIB}
)
