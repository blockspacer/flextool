cmake_minimum_required(VERSION 2.8.11)

project(test_package CXX)

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

add_executable(testme
    main.cpp
)
