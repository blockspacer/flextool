include_guard( DIRECTORY )

if(NOT PROJECT_NAME)
  message(FATAL_ERROR
    "Project name must be specified!")
endif(NOT PROJECT_NAME)

message(STATUS
  "PROJECT: ${CMAKE_PROJECT_NAME}")

set(FLEXTOOL_LIB_NAME ${PROJECT_NAME})

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_tolower)

set(flextool_include_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/flextool")

set(flextool_src_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
