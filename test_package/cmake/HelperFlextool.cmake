find_package(flextool MODULE REQUIRED)

message(STATUS "flextool_BIN: ${flextool_BIN}")
message(STATUS "flextool_ROOT: ${flextool_ROOT}")
message(STATUS "CONAN_LIBS_flextool_CONAN: ${CONAN_LIBS_flextool_CONAN}")

find_program(flextool flextool NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
message(STATUS "find_program for flextool ${flextool}")

if(${flextool} STREQUAL "")
    message(FATAL_ERROR "flextool not found ${flextool}")
endif()
