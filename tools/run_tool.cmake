#
# portable build script based on "cmake -P"
#
# example:
# cmake -DUNCRUSTIFY="ON" -P tools/run_tool.cmake
#
cmake_minimum_required(VERSION 3.5)

# remove old build dirs
option(CLEAN_OLD "CLEAN_OLD" OFF)

option(UNCRUSTIFY "UNCRUSTIFY" OFF)
if(UNCRUSTIFY)
  message(STATUS
    "UNCRUSTIFY ENABLED")
endif(UNCRUSTIFY)

option(CLANG_FORMAT "CLANG_FORMAT" OFF)
if(CLANG_FORMAT)
  message(STATUS
    "CLANG_FORMAT ENABLED")
endif(CLANG_FORMAT)

option(OCLINT "OCLINT" OFF)
if(OCLINT)
  message(STATUS
    "OCLINT ENABLED")
endif(OCLINT)

option(IWYU "IWYU" OFF)
if(IWYU)
  message(STATUS
    "IWYU ENABLED")
endif(IWYU)

option(CPPCLEAN "CPPCLEAN" OFF)
if(CPPCLEAN)
  message(STATUS
    "CPPCLEAN ENABLED")
endif(CPPCLEAN)

option(CLANG_TIDY "CLANG_TIDY" OFF)
if(CLANG_TIDY)
  message(STATUS
    "CLANG_TIDY ENABLED")
endif(CLANG_TIDY)

option(CPPCHECK "CPPCHECK" OFF)
if(CPPCHECK)
  message(STATUS
    "CPPCHECK ENABLED")
endif(CPPCHECK)

# TODO: make local, not global
# allows to run `execute_process` without printing to console
option(PRINT_TO_STDOUT "PRINT_TO_STDOUT" ON)
if(PRINT_TO_STDOUT)
  set(OUTPUT_VARS ) # undefined
else()
  set(OUTPUT_VARS OUTPUT_VARIABLE stdout)
endif(PRINT_TO_STDOUT)

# TODO: refactor CACHE BOOL (TRUE/FALSE) to option (ON/OFF)

set(ALWAYS_BUILD TRUE CACHE BOOL "ALWAYS_BUILD")

# --- includes ---
# WHY CMAKE_CURRENT_LIST_DIR? see https://stackoverflow.com/a/12854575/10904212
set(CURRENT_SCRIPT_DIR ${CMAKE_CURRENT_LIST_DIR})
include(${CURRENT_SCRIPT_DIR}/coloredOut.cmake)

find_program(CONAN_PATH conan
             HINTS ${CONAN_DIR}
                   /usr/bin
                   /usr/local/bin
                   $ENV{PATH}
                   CMAKE_SYSTEM_PROGRAM_PATH)

if(NOT CONAN_PATH)
  message(FATAL_ERROR "conan not found! Aborting...")
endif() # NOT CONAN_PATH

macro(cmake_remove_directory DIR_PATH)
    message(STATUS "running `cmake_remove_directory` for ${PATH_URI}")
    execute_process(
      COMMAND
        ${COLORED_OUTPUT_ENABLER}
          ${CMAKE_COMMAND} "-E" "time" "cmake" "-E" "remove_directory" "${DIR_PATH}"
      WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}
      TIMEOUT 7200 # sec
      RESULT_VARIABLE retcode
      ERROR_VARIABLE stderr
      ${OUTPUT_VARS} # may create `stdout` variable
    )
    if(NOT "${retcode}" STREQUAL "0")
      message( FATAL_ERROR "(cmake_remove_directory ${DIR_PATH})
        Bad exit status ${retcode} ${stdout} ${stderr}")
    endif()
endmacro(cmake_remove_directory)

macro(cmake_make_dir DIR_PATH)
    message(STATUS "running `git clone` for ${PATH_URI}")
    execute_process(
      COMMAND
        ${COLORED_OUTPUT_ENABLER}
          ${CMAKE_COMMAND} "-E" "time" "cmake" "-E" "make_directory" "${DIR_PATH}"
      WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}
      TIMEOUT 7200 # sec
      RESULT_VARIABLE retcode
      ERROR_VARIABLE stderr
      ${OUTPUT_VARS} # may create `stdout` variable
    )
    if(NOT "${retcode}" STREQUAL "0")
      message( FATAL_ERROR "(cmake_make_dir)
        Bad exit status ${retcode} ${stdout} ${stderr}")
    endif()
endmacro(cmake_make_dir)

# --- run commands ---

if(CLEAN_OLD)
  if(EXISTS "${CURRENT_SCRIPT_DIR}/.tmp")
    cmake_remove_directory("${CURRENT_SCRIPT_DIR}/.tmp")
  endif()
endif(CLEAN_OLD)

cmake_make_dir("${CURRENT_SCRIPT_DIR}/.tmp")

## ----------------------------- UNCRUSTIFY --------------------------

if(UNCRUSTIFY)
  message(STATUS
    "running UNCRUSTIFY \
    install folder: ${CURRENT_SCRIPT_DIR}/.tmp/uncrustify_local_build...")
  set(ENV{CONAN_REVISIONS_ENABLED} 1)
  set(ENV{CONAN_VERBOSE_TRACEBACK} 1)
  set(ENV{CONAN_PRINT_RUN_COMMANDS} 1)
  set(ENV{CONAN_LOGGING_LEVEL} 10)
  set(ENV{GIT_SSL_NO_VERIFY} "true")
  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
        cmake -E time
          conan install ..
          --install-folder ${CURRENT_SCRIPT_DIR}/.tmp/uncrustify_local_build
          -s build_type=Debug
          -s cling_conan:build_type=Release
          -s llvm_tools:build_type=Release
          --profile clang
              -e flextool:enable_tests=True
              -e flextool:enable_llvm_tools=True
              -e chromium_base:enable_llvm_tools=True
              -o chromium_base:use_alloc_shim=False
              -e basis:enable_llvm_tools=True
              -e flexlib:enable_llvm_tools=True
              -o flexlib:enable_clang_from_conan=False
              -o chromium_tcmalloc:use_alloc_shim=False
              --build chromium_base
              --build chromium_tcmalloc
              --build basis
              --build flexlib
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()

  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
      cmake -E time cmake ../../..
        -DENABLE_UNCRUSTIFY=ON
        -DENABLE_TESTS=FALSE
        -DBUILD_SHARED_LIBS=FALSE
        -DCONAN_AUTO_INSTALL=OFF
        -DCMAKE_BUILD_TYPE=Debug
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}/.tmp/uncrustify_local_build
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()

  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
      cmake -E time cmake --build .
        --target flextool_run_uncrustify
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}/.tmp/uncrustify_local_build
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()
endif(UNCRUSTIFY)

## ---------------------------- CLANG_FORMAT -------------------------

if(CLANG_FORMAT)
  message(STATUS
    "running CLANG_FORMAT \
    install folder: ${CURRENT_SCRIPT_DIR}/.tmp/clang_format_local_build...")
  set(ENV{CONAN_REVISIONS_ENABLED} 1)
  set(ENV{CONAN_VERBOSE_TRACEBACK} 1)
  set(ENV{CONAN_PRINT_RUN_COMMANDS} 1)
  set(ENV{CONAN_LOGGING_LEVEL} 10)
  set(ENV{GIT_SSL_NO_VERIFY} "true")
  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
        cmake -E time
          conan install ..
          --install-folder ${CURRENT_SCRIPT_DIR}/.tmp/clang_format_local_build
          -s build_type=Debug
          -s cling_conan:build_type=Release
          -s llvm_tools:build_type=Release
          --profile clang
              -e flextool:enable_tests=True
              -e flextool:enable_llvm_tools=True
              -e chromium_base:enable_llvm_tools=True
              -o chromium_base:use_alloc_shim=False
              -e basis:enable_llvm_tools=True
              -e flexlib:enable_llvm_tools=True
              -o flexlib:enable_clang_from_conan=False
              -o chromium_tcmalloc:use_alloc_shim=False
              --build chromium_base
              --build chromium_tcmalloc
              --build basis
              --build flexlib
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()

  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
      cmake -E time cmake ../../..
        -DENABLE_CLANG_FORMAT=ON
        -DENABLE_TESTS=FALSE
        -DBUILD_SHARED_LIBS=FALSE
        -DCONAN_AUTO_INSTALL=OFF
        -DCMAKE_BUILD_TYPE=Debug
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}/.tmp/clang_format_local_build
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()

  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
      cmake -E time cmake --build .
        --target flextool_run_clang_format
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}/.tmp/clang_format_local_build
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()
endif(CLANG_FORMAT)

## ---------------------------- OCLINT -------------------------

if(OCLINT)
  message(STATUS
    "running OCLINT \
    install folder: ${CURRENT_SCRIPT_DIR}/.tmp/oclint_local_build...")
  set(ENV{CONAN_REVISIONS_ENABLED} 1)
  set(ENV{CONAN_VERBOSE_TRACEBACK} 1)
  set(ENV{CONAN_PRINT_RUN_COMMANDS} 1)
  set(ENV{CONAN_LOGGING_LEVEL} 10)
  set(ENV{GIT_SSL_NO_VERIFY} "true")
  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
        cmake -E time
          conan install ..
          --install-folder ${CURRENT_SCRIPT_DIR}/.tmp/oclint_local_build
          -s build_type=Debug
          -s cling_conan:build_type=Release
          -s llvm_tools:build_type=Release
          --profile clang
              -e flextool:enable_tests=True
              -e flextool:enable_llvm_tools=True
              -e chromium_base:enable_llvm_tools=True
              -o chromium_base:use_alloc_shim=False
              -e basis:enable_llvm_tools=True
              -e flexlib:enable_llvm_tools=True
              -o flexlib:enable_clang_from_conan=False
              -o chromium_tcmalloc:use_alloc_shim=False
              --build chromium_base
              --build chromium_tcmalloc
              --build basis
              --build flexlib
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()

  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
      cmake -E time cmake ../../..
        -DENABLE_OCLINT=ON
        -DENABLE_TESTS=FALSE
        -DBUILD_SHARED_LIBS=FALSE
        -DCONAN_AUTO_INSTALL=OFF
        -DCMAKE_BUILD_TYPE=Debug
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}/.tmp/oclint_local_build
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()

  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
      cmake -E time cmake --build .
        --target flextool_run_oclint
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}/.tmp/oclint_local_build
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()
endif(OCLINT)

## ---------------------------- IWYU -------------------------

if(IWYU)
  message(STATUS
    "running IWYU \
    install folder: ${CURRENT_SCRIPT_DIR}/.tmp/iwyu_local_build...")
  set(ENV{CONAN_REVISIONS_ENABLED} 1)
  set(ENV{CONAN_VERBOSE_TRACEBACK} 1)
  set(ENV{CONAN_PRINT_RUN_COMMANDS} 1)
  set(ENV{CONAN_LOGGING_LEVEL} 10)
  set(ENV{GIT_SSL_NO_VERIFY} "true")
  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
        cmake -E time
          conan install ..
          --install-folder ${CURRENT_SCRIPT_DIR}/.tmp/iwyu_local_build
          -s build_type=Debug
          -s cling_conan:build_type=Release
          -s llvm_tools:build_type=Release
          --profile clang
              -e flextool:enable_tests=True
              -e flextool:enable_llvm_tools=True
              -e chromium_base:enable_llvm_tools=True
              -o chromium_base:use_alloc_shim=False
              -e basis:enable_llvm_tools=True
              -e flexlib:enable_llvm_tools=True
              -o flexlib:enable_clang_from_conan=False
              -o chromium_tcmalloc:use_alloc_shim=False
              --build chromium_base
              --build chromium_tcmalloc
              --build basis
              --build flexlib
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()

  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
      cmake -E time cmake ../../..
        -DENABLE_IWYU=ON
        -DENABLE_TESTS=FALSE
        -DBUILD_SHARED_LIBS=FALSE
        -DCONAN_AUTO_INSTALL=OFF
        -DCMAKE_BUILD_TYPE=Debug
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}/.tmp/iwyu_local_build
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()

  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
      cmake -E time cmake --build .
        # NOTE: no custom target, uses CXX_INCLUDE_WHAT_YOU_USE
        #--target flextool_run_iwyu
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}/.tmp/iwyu_local_build
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()
endif(IWYU)

## ---------------------------- CPPCLEAN -------------------------

if(CPPCLEAN)
  message(STATUS
    "running CPPCLEAN \
    install folder: ${CURRENT_SCRIPT_DIR}/.tmp/cppclean_local_build...")
  set(ENV{CONAN_REVISIONS_ENABLED} 1)
  set(ENV{CONAN_VERBOSE_TRACEBACK} 1)
  set(ENV{CONAN_PRINT_RUN_COMMANDS} 1)
  set(ENV{CONAN_LOGGING_LEVEL} 10)
  set(ENV{GIT_SSL_NO_VERIFY} "true")
  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
        cmake -E time
          conan install ..
          --install-folder ${CURRENT_SCRIPT_DIR}/.tmp/cppclean_local_build
          -s build_type=Debug
          -s cling_conan:build_type=Release
          -s llvm_tools:build_type=Release
          --profile clang
              -e flextool:enable_tests=True
              -e flextool:enable_llvm_tools=True
              -e chromium_base:enable_llvm_tools=True
              -o chromium_base:use_alloc_shim=False
              -e basis:enable_llvm_tools=True
              -e flexlib:enable_llvm_tools=True
              -o flexlib:enable_clang_from_conan=False
              -o chromium_tcmalloc:use_alloc_shim=False
              --build chromium_base
              --build chromium_tcmalloc
              --build basis
              --build flexlib
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()

  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
      cmake -E time cmake ../../..
        -DENABLE_CPPCLEAN=ON
        -DENABLE_TESTS=FALSE
        -DBUILD_SHARED_LIBS=FALSE
        -DCONAN_AUTO_INSTALL=OFF
        -DCMAKE_BUILD_TYPE=Debug
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}/.tmp/cppclean_local_build
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()

  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
      cmake -E time cmake --build .
        --target flextool_run_cppclean
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}/.tmp/cppclean_local_build
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()
endif(CPPCLEAN)

## ---------------------------- CLANG_TIDY -------------------------

if(CLANG_TIDY)
  message(STATUS
    "running CLANG_TIDY \
    install folder: ${CURRENT_SCRIPT_DIR}/.tmp/clang_tidy_local_build...")
  set(ENV{CONAN_REVISIONS_ENABLED} 1)
  set(ENV{CONAN_VERBOSE_TRACEBACK} 1)
  set(ENV{CONAN_PRINT_RUN_COMMANDS} 1)
  set(ENV{CONAN_LOGGING_LEVEL} 10)
  set(ENV{GIT_SSL_NO_VERIFY} "true")
  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
        cmake -E time
          conan install ..
          --install-folder ${CURRENT_SCRIPT_DIR}/.tmp/clang_tidy_local_build
          -s build_type=Debug
          -s cling_conan:build_type=Release
          -s llvm_tools:build_type=Release
          --profile clang
              -e flextool:enable_tests=True
              -e flextool:enable_llvm_tools=True
              -e chromium_base:enable_llvm_tools=True
              -o chromium_base:use_alloc_shim=False
              -e basis:enable_llvm_tools=True
              -e flexlib:enable_llvm_tools=True
              -o flexlib:enable_clang_from_conan=False
              -o chromium_tcmalloc:use_alloc_shim=False
              --build chromium_base
              --build chromium_tcmalloc
              --build basis
              --build flexlib
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()

  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
      cmake -E time cmake ../../..
        -DENABLE_CLANG_TIDY=ON
        -DENABLE_TESTS=FALSE
        -DBUILD_SHARED_LIBS=FALSE
        -DCONAN_AUTO_INSTALL=OFF
        -DCMAKE_BUILD_TYPE=Debug
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}/.tmp/clang_tidy_local_build
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()

  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
      cmake -E time cmake --build .
        --target flextool_run_clang_tidy
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}/.tmp/clang_tidy_local_build
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()
endif(CLANG_TIDY)

## ---------------------------- CPPCHECK -------------------------

if(CPPCHECK)
  message(STATUS
    "running CPPCHECK \
    install folder: ${CURRENT_SCRIPT_DIR}/.tmp/cppcheck_local_build...")
  set(ENV{CONAN_REVISIONS_ENABLED} 1)
  set(ENV{CONAN_VERBOSE_TRACEBACK} 1)
  set(ENV{CONAN_PRINT_RUN_COMMANDS} 1)
  set(ENV{CONAN_LOGGING_LEVEL} 10)
  set(ENV{GIT_SSL_NO_VERIFY} "true")
  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
        cmake -E time
          conan install ..
          --install-folder ${CURRENT_SCRIPT_DIR}/.tmp/cppcheck_local_build
          -s build_type=Debug
          -s cling_conan:build_type=Release
          -s llvm_tools:build_type=Release
          --profile clang
              -e flextool:enable_tests=True
              -e flextool:enable_llvm_tools=True
              -e chromium_base:enable_llvm_tools=True
              -o chromium_base:use_alloc_shim=False
              -e basis:enable_llvm_tools=True
              -e flexlib:enable_llvm_tools=True
              -o flexlib:enable_clang_from_conan=False
              -o chromium_tcmalloc:use_alloc_shim=False
              --build chromium_base
              --build chromium_tcmalloc
              --build basis
              --build flexlib
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()

  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
      cmake -E time cmake ../../..
        -DENABLE_CPPCHECK=ON
        -DENABLE_TESTS=FALSE
        -DBUILD_SHARED_LIBS=FALSE
        -DCONAN_AUTO_INSTALL=OFF
        -DCMAKE_BUILD_TYPE=Debug
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}/.tmp/cppcheck_local_build
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()

  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
      cmake -E time cmake --build .
        --target flextool_run_cppcheck
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}/.tmp/cppcheck_local_build
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(execute_process)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()
endif(CPPCHECK)

## ---------------------------- CLEAN_OLD -------------------------

if(CLEAN_OLD)
  if(EXISTS "${CURRENT_SCRIPT_DIR}/.tmp")
    cmake_remove_directory("${CURRENT_SCRIPT_DIR}/.tmp")
  endif()
endif(CLEAN_OLD)
