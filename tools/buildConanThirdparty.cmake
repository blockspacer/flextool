#
# portable build script based on "cmake -P"
#
# example:
# cmake -DEXTRA_CONAN_OPTS="--profile;default" -P tools/buildConanThirdparty.cmake
#
cmake_minimum_required(VERSION 3.5)

option(CLEAN_BUILD "CLEAN_BUILD" ON)
if(CLEAN_BUILD)
  message(WARNING "(buildConanThirdparty)
    clean rebuild of all conan deps may take a lot of time.
    Use `CLEAN_BUILD=OFF` with `--build=missing` in `EXTRA_CONAN_OPTS`")
endif(CLEAN_BUILD)

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
set(ENABLE_LLVM_TOOLS FALSE CACHE BOOL "ENABLE_LLVM_TOOLS")
set(ENABLE_CLING TRUE CACHE BOOL "ENABLE_CLING")
set(ENABLE_BASIS TRUE CACHE BOOL "ENABLE_BASIS")
set(ENABLE_TYPE_SAFE TRUE CACHE BOOL "ENABLE_TYPE_SAFE")
set(ENABLE_CLANG_FOLLY_CONAN FALSE CACHE BOOL "ENABLE_CLANG_FOLLY_CONAN")
set(ENABLE_CORRADE TRUE CACHE BOOL "ENABLE_CORRADE")
set(ENABLE_FLEXTOOL FALSE CACHE BOOL "ENABLE_FLEXTOOL")
set(ENABLE_CPPCHECK TRUE CACHE BOOL "ENABLE_CPPCHECK")
set(ENABLE_FLEXLIB TRUE CACHE BOOL "ENABLE_FLEXLIB")
set(ENABLE_FLEX_REFLECT_PLUGIN FALSE CACHE BOOL "ENABLE_FLEX_REFLECT_PLUGIN")
set(ENABLE_SQUARETS FALSE CACHE BOOL "ENABLE_SQUARETS")
set(ENABLE_FLEX_SQUARETS_PLUGIN FALSE CACHE BOOL "ENABLE_FLEX_SQUARETS_PLUGIN")
set(ENABLE_FLEX_PIMPL_PLUGIN FALSE CACHE BOOL "ENABLE_FLEX_PIMPL_PLUGIN")
set(ENABLE_FLEX_TYPECLASS_PLUGIN FALSE CACHE BOOL "ENABLE_FLEX_TYPECLASS_PLUGIN")
set(ENABLE_FLEX_META_PLUGIN FALSE CACHE BOOL "ENABLE_FLEX_META_PLUGIN")
set(ENABLE_FLEX_META_DEMO FALSE CACHE BOOL "ENABLE_FLEX_META_DEMO")
set(ENABLE_MONGO_CXX_DRIVER FALSE CACHE BOOL "ENABLE_MONGO_CXX_DRIVER")
set(ENABLE_ABSEIL FALSE CACHE BOOL "ENABLE_ABSEIL")
set(ENABLE_MONGO_C_DRIVER FALSE CACHE BOOL "ENABLE_MONGO_C_DRIVER")
set(ENABLE_FLATC_CONAN FALSE CACHE BOOL "ENABLE_FLATC_CONAN")
set(ENABLE_CONAN_POCO FALSE CACHE BOOL "ENABLE_CONAN_POCO")
set(ENABLE_CONAN_PROTOBUF FALSE CACHE BOOL "ENABLE_CONAN_PROTOBUF")

# --- includes ---
# WHY CMAKE_CURRENT_LIST_DIR? see https://stackoverflow.com/a/12854575/10904212
set(CURRENT_SCRIPT_DIR ${CMAKE_CURRENT_LIST_DIR})
include(${CURRENT_SCRIPT_DIR}/coloredOut.cmake)

set(EXTRA_CONAN_OPTS "" CACHE STRING "conan arguments")
if(EXTRA_CONAN_OPTS STREQUAL "")
  message(FATAL_ERROR "(buildConanThirdparty)
    provide EXTRA_CONAN_OPTS, see comments in .cmake file")
endif()

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

# NOTE: specify OPTIONS with ';' like "-b;v0.2.1"
macro(git_clone WORKING_DIRECTORY PATH_URI OPTIONS)
    message(STATUS "running `git clone` for ${PATH_URI}")
    execute_process(
      COMMAND
        ${COLORED_OUTPUT_ENABLER}
          ${CMAKE_COMMAND} "-E" "time" "git" "clone" ${PATH_URI} "${WORKING_DIRECTORY}" "--recursive" ${OPTIONS}
      WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}
      TIMEOUT 7200 # sec
      RESULT_VARIABLE retcode
      ERROR_VARIABLE stderr
      ${OUTPUT_VARS} # may create `stdout` variable
    )
    if(NOT "${retcode}" STREQUAL "0")
      message( FATAL_ERROR "(git_clone)
        Bad exit status ${retcode} ${stdout} ${stderr}")
    endif()
endmacro(git_clone)

macro(conan_remove_target TARGET_NAME)
  #
  message(STATUS "running `conan remove -f` for ${TARGET_NAME}")
  set(ENV{CONAN_REVISIONS_ENABLED} 1)
  set(ENV{CONAN_VERBOSE_TRACEBACK} 1)
  set(ENV{CONAN_PRINT_RUN_COMMANDS} 1)
  set(ENV{CONAN_LOGGING_LEVEL} 1)
  set(ENV{GIT_SSL_NO_VERIFY} 1)
  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
        ${CMAKE_COMMAND} "-E" "time" "${CONAN_PATH}" "remove" ${TARGET_NAME} "-f"
    WORKING_DIRECTORY ${CURRENT_SCRIPT_DIR}
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( WARNING "(conan_remove_target)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()
endmacro(conan_remove_target)

macro(conan_install_target TARGET_PATH EXTRA_TARGET_OPTS)
  #
  if(NOT EXISTS "${TARGET_PATH}/conanfile.py" AND NOT EXISTS "${TARGET_PATH}/conanfile.txt")
    message(FATAL_ERROR "(conan_install_target)
      path not found: ${TARGET_PATH}/conanfile.py
      AND ${TARGET_PATH}/conanfile.txt")
  endif()
  #
  message(STATUS "running `conan install` for ${TARGET_PATH}")
  set(ENV{CONAN_REVISIONS_ENABLED} 1)
  set(ENV{CONAN_VERBOSE_TRACEBACK} 1)
  set(ENV{CONAN_PRINT_RUN_COMMANDS} 1)
  set(ENV{CONAN_LOGGING_LEVEL} 1)
  set(ENV{GIT_SSL_NO_VERIFY} 1)
  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
        ${CMAKE_COMMAND} "-E" "time"
          "${CONAN_PATH}" "install" "." ${EXTRA_CONAN_OPTS} ${EXTRA_TARGET_OPTS}
    WORKING_DIRECTORY ${TARGET_PATH}
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(conan_install_target)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()
endmacro(conan_install_target)

macro(conan_create_target TARGET_PATH TARGET_CHANNEL EXTRA_TARGET_OPTS)
  #
  if(NOT EXISTS "${TARGET_PATH}/conanfile.py" AND NOT EXISTS "${TARGET_PATH}/conanfile.txt")
    message(FATAL_ERROR "(conan_create_target)
      path not found: ${TARGET_PATH}/conanfile.py
      AND ${TARGET_PATH}/conanfile.txt")
  endif()
  #
  message(STATUS "running `conan create` for ${TARGET_PATH}")
  set(ENV{CONAN_REVISIONS_ENABLED} 1)
  set(ENV{CONAN_VERBOSE_TRACEBACK} 1)
  set(ENV{CONAN_PRINT_RUN_COMMANDS} 1)
  set(ENV{CONAN_LOGGING_LEVEL} 1)
  set(ENV{GIT_SSL_NO_VERIFY} 1)
  execute_process(
    COMMAND
      ${COLORED_OUTPUT_ENABLER}
        ${CMAKE_COMMAND} "-E" "time"
          "${CONAN_PATH}" "create" "." "${TARGET_CHANNEL}" ${EXTRA_CONAN_OPTS} ${EXTRA_TARGET_OPTS}
    WORKING_DIRECTORY ${TARGET_PATH}
    TIMEOUT 7200 # sec
    RESULT_VARIABLE retcode
    ERROR_VARIABLE stderr
    ${OUTPUT_VARS} # may create `stdout` variable
  )
  if(NOT "${retcode}" STREQUAL "0")
    message( FATAL_ERROR "(conan_create_target)
      Bad exit status ${retcode} ${stdout} ${stderr}")
  endif()
endmacro(conan_create_target)

macro(conan_build_target_if TARGET_NAME TARGET_CHANNEL TARGET_PATH OPTION_NAME EXTRA_TARGET_OPTS)
  if(NOT ${OPTION_NAME})
    message(STATUS "(conan_build_target_if)
      DISABLED: ${OPTION_NAME}")
  endif()

  if(${OPTION_NAME})
    if(CLEAN_BUILD)
      conan_remove_target(${TARGET_NAME})
    endif(CLEAN_BUILD)
    conan_install_target(${TARGET_PATH} "${EXTRA_TARGET_OPTS}")
    conan_create_target(${TARGET_PATH} ${TARGET_CHANNEL} "${EXTRA_TARGET_OPTS}")
  endif(${OPTION_NAME})
endmacro(conan_build_target_if)

# --- run `conan create` command ---

if(EXISTS "${CURRENT_SCRIPT_DIR}/.tmp")
  cmake_remove_directory("${CURRENT_SCRIPT_DIR}/.tmp")
endif()

cmake_make_dir("${CURRENT_SCRIPT_DIR}/.tmp")

# conan_build_helper

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/conan_build_helper")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/conan_build_helper"
      "http://github.com/blockspacer/conan_build_helper.git"
      "")
endif()
conan_build_target_if(
  "conan_build_helper" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/conan_build_helper" # target to build
  ALWAYS_BUILD
  "")

# cmake_platform_detection_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/cmake_platform_detection_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/cmake_platform_detection_conan"
      "http://github.com/blockspacer/cmake_platform_detection_conan.git"
      "")
endif()
conan_build_target_if(
  "cmake_platform_detection" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/cmake_platform_detection_conan" # target to build
  ALWAYS_BUILD
  "")

# cmake_build_options_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/cmake_build_options_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/cmake_build_options_conan"
      "http://github.com/blockspacer/cmake_build_options_conan.git"
      "")
endif()
conan_build_target_if(
  "cmake_build_options" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/cmake_build_options_conan" # target to build
  ALWAYS_BUILD
  "")

# cmake_helper_utils_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/cmake_helper_utils_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/cmake_helper_utils_conan"
      "http://github.com/blockspacer/cmake_helper_utils_conan.git"
      "")
endif()
conan_build_target_if(
  "cmake_helper_utils" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/cmake_helper_utils_conan" # target to build
  ALWAYS_BUILD
  "")

# cmake_sanitizers_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/cmake_sanitizers_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/cmake_sanitizers_conan"
      "http://github.com/blockspacer/cmake_sanitizers_conan.git"
      "")
endif()
conan_build_target_if(
  "cmake_sanitizers" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/cmake_sanitizers_conan" # target to build
  ALWAYS_BUILD
  "")

# chromium_build_util_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/chromium_build_util_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/chromium_build_util_conan"
      "http://github.com/blockspacer/chromium_build_util_conan.git"
      "")
endif()
conan_build_target_if(
  "chromium_build_util" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/chromium_build_util_conan" # target to build
  ALWAYS_BUILD
  "")

# llvm_tools

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/llvm_tools")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/llvm_tools"
      "http://github.com/blockspacer/llvm_tools.git"
      "")
endif()
conan_build_target_if(
  "llvm_tools" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/llvm_tools" # target to build
  ENABLE_LLVM_TOOLS
  ";-s;llvm_tools:build_type=Release")

# conan_gtest

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/conan_gtest")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/conan_gtest"
      "http://github.com/blockspacer/conan_gtest.git"
      "")
endif()
conan_build_target_if(
  "conan_gtest" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/conan_gtest" # target to build
  ALWAYS_BUILD
  "")

# conan_zlib

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/conan_zlib")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/conan_zlib"
      "http://github.com/blockspacer/conan_zlib.git"
      "")
endif()
conan_build_target_if(
  "zlib" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/conan_zlib" # target to build
  ALWAYS_BUILD
  "")

# conan_openssl

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/conan_openssl")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/conan_openssl"
      "http://github.com/blockspacer/conan_openssl.git"
      "")
endif()
conan_build_target_if(
  "openssl" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/conan_openssl" # target to build
  ALWAYS_BUILD
  "")

# conan_libevent

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/conan_libevent")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/conan_libevent"
      "http://github.com/blockspacer/conan_libevent.git"
      "")
endif()
conan_build_target_if(
  "libevent" # target to clean
  "dev/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/conan_libevent" # target to build
  ALWAYS_BUILD
  "")

# chromium_libevent_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/chromium_libevent_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/chromium_libevent_conan"
      "http://github.com/blockspacer/chromium_libevent_conan.git"
      "")
endif()
conan_build_target_if(
  "chromium_libevent" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/chromium_libevent_conan" # target to build
  ALWAYS_BUILD
  "")

# chromium_dynamic_annotations_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/chromium_dynamic_annotations_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/chromium_dynamic_annotations_conan"
      "http://github.com/blockspacer/chromium_dynamic_annotations_conan.git"
      "")
endif()
conan_build_target_if(
  "chromium_dynamic_annotations" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/chromium_dynamic_annotations_conan" # target to build
  ALWAYS_BUILD
  "")

# chromium_tcmalloc_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/chromium_tcmalloc_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/chromium_tcmalloc_conan"
      "http://github.com/blockspacer/chromium_tcmalloc_conan.git"
      "")
endif()
conan_build_target_if(
  "chromium_tcmalloc" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/chromium_tcmalloc_conan" # target to build
  ALWAYS_BUILD
  "")

# chromium_xdg_user_dirs_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/chromium_xdg_user_dirs_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/chromium_xdg_user_dirs_conan"
      "http://github.com/blockspacer/chromium_xdg_user_dirs_conan.git"
      "")
endif()
conan_build_target_if(
  "chromium_xdg_user_dirs" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/chromium_xdg_user_dirs_conan" # target to build
  ALWAYS_BUILD
  "")

# chromium_xdg_mime_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/chromium_xdg_mime_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/chromium_xdg_mime_conan"
      "http://github.com/blockspacer/chromium_xdg_mime_conan.git"
      "")
endif()
conan_build_target_if(
  "chromium_xdg_mime" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/chromium_xdg_mime_conan" # target to build
  ALWAYS_BUILD
  "")

# chromium_modp_b64_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/chromium_modp_b64_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/chromium_modp_b64_conan"
      "http://github.com/blockspacer/chromium_modp_b64_conan.git"
      "")
endif()
conan_build_target_if(
  "chromium_modp_b64" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/chromium_modp_b64_conan" # target to build
  ALWAYS_BUILD
  "")

# chromium_icu_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/chromium_icu_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/chromium_icu_conan"
      "http://github.com/blockspacer/chromium_icu_conan.git"
      "")
endif()
conan_build_target_if(
  "chromium_icu" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/chromium_icu_conan" # target to build
  ALWAYS_BUILD
  "")

# chromium_compact_enc_det_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/chromium_compact_enc_det_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/chromium_compact_enc_det_conan"
      "http://github.com/blockspacer/chromium_compact_enc_det_conan.git"
      "")
endif()
conan_build_target_if(
  "chromium_compact_enc_det" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/chromium_compact_enc_det_conan" # target to build
  ALWAYS_BUILD
  "")

# cobalt_starboard_headers_only

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/cobalt_starboard_headers_only")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/cobalt_starboard_headers_only"
      "http://github.com/blockspacer/cobalt_starboard_headers_only.git"
      "")
endif()
conan_build_target_if(
  "cobalt_starboard_headers_only" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/cobalt_starboard_headers_only" # target to build
  ALWAYS_BUILD
  "")

# chromium_zlib_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/chromium_zlib_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/chromium_zlib_conan"
      "http://github.com/blockspacer/chromium_zlib_conan.git"
      "")
endif()
conan_build_target_if(
  "chromium_zlib" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/chromium_zlib_conan" # target to build
  ALWAYS_BUILD
  "")

# cobalt_glm_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/cobalt_glm_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/cobalt_glm_conan"
      "http://github.com/blockspacer/cobalt_glm_conan.git"
      "")
endif()
conan_build_target_if(
  "cobalt_glm" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/cobalt_glm_conan" # target to build
  ALWAYS_BUILD
  "")

# chromium_libxml_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/chromium_libxml_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/chromium_libxml_conan"
      "http://github.com/blockspacer/chromium_libxml_conan.git"
      "")
endif()
conan_build_target_if(
  "chromium_libxml" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/chromium_libxml_conan" # target to build
  ALWAYS_BUILD
  "")

# conan_bzip2

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/conan_bzip2")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/conan_bzip2"
      "http://github.com/blockspacer/conan_bzip2.git"
      "")
endif()
conan_build_target_if(
  "bzip2" # target to clean
  "dev/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/conan_bzip2" # target to build
  ALWAYS_BUILD
  "")

# conan_boost

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/conan_boost")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/conan_boost"
      "http://github.com/blockspacer/conan_boost.git"
      "")
endif()
conan_build_target_if(
  "boost" # target to clean
  "dev/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/conan_boost" # target to build
  ALWAYS_BUILD
  "")

# cobalt_starboard_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/cobalt_starboard_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/cobalt_starboard_conan"
      "http://github.com/blockspacer/cobalt_starboard_conan.git"
      "")
endif()
conan_build_target_if(
  "cobalt_starboard" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/cobalt_starboard_conan" # target to build
  ALWAYS_BUILD
  "")

# chromium_base_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/chromium_base_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/chromium_base_conan"
      "http://github.com/blockspacer/chromium_base_conan.git"
      "")
endif()
conan_build_target_if(
  "chromium_base" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/chromium_base_conan" # target to build
  ALWAYS_BUILD
  "")

# clang_folly_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/clang_folly_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/clang_folly_conan"
      "http://github.com/blockspacer/clang_folly_conan.git"
      "")
endif()
conan_build_target_if(
  "clang_folly_conan" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/clang_folly_conan" # target to build
  ENABLE_CLANG_FOLLY_CONAN
  "")

# corrade

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/corrade")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/corrade"
      "https://github.com/blockspacer/corrade_conan.git"
      "")
endif()
conan_build_target_if(
  "corrade" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/corrade" # target to build
  ENABLE_CORRADE
  "")

# type_safe

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/type_safe")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/type_safe"
      "http://github.com/foonathan/type_safe.git"
      "-b;v0.2.1")
endif()
conan_build_target_if(
  "type_safe" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/type_safe" # target to build
  ENABLE_TYPE_SAFE
  "")

# basis

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/basis")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/basis"
      "https://github.com/blockspacer/basis.git"
      "")
endif()
conan_build_target_if(
  "basis" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/basis" # target to build
  ENABLE_BASIS
  "")

# cling_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/cling_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/cling_conan"
      "https://github.com/blockspacer/cling_conan.git"
      "")
endif()
conan_build_target_if(
  "cling_conan" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/cling_conan" # target to build
  ENABLE_CLING
  ";-s;cling_conan:build_type=Release")

# flexlib

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/flexlib")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/flexlib"
      "https://github.com/blockspacer/flexlib.git"
      "")
endif()
conan_build_target_if(
  "flexlib" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/flexlib" # target to build
  ENABLE_FLEXLIB
  "")

# flex_support_headers

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/flex_support_headers")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/flex_support_headers"
      "https://github.com/blockspacer/flex_support_headers.git"
      "")
endif()
conan_build_target_if(
  "flex_support_headers" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/flex_support_headers" # target to build
  ALWAYS_BUILD
  "")

# conan-cppcheck_installer

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/conan-cppcheck_installer")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/conan-cppcheck_installer"
      "https://github.com/bincrafters/conan-cppcheck_installer.git"
      "-b;testing/1.90")
endif()
conan_build_target_if(
  "conan-cppcheck_installer" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/conan-cppcheck_installer" # target to build
  ENABLE_CPPCHECK
  "")

# flextool

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/flextool")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/flextool"
      "https://github.com/blockspacer/flextool.git"
      "")
endif()
conan_build_target_if(
  "flextool" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/flextool" # target to build
  ENABLE_FLEXTOOL
  ";-e;flextool:enable_tests=True;\
-e;flextool:enable_llvm_tools=False")

# flex_reflect_plugin

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/flex_reflect_plugin")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/flex_reflect_plugin"
      "https://github.com/blockspacer/flex_reflect_plugin.git"
      "")
endif()
conan_build_target_if(
  "flex_reflect_plugin" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/flex_reflect_plugin" # target to build
  ENABLE_FLEX_REFLECT_PLUGIN
  ";-o;flex_reflect_plugin:shared=True;\
-o;flex_reflect_plugin:enable_clang_from_conan=False;\
-e;flex_reflect_plugin:enable_tests=True")

# squarets

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/squarets")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/squarets"
      "https://github.com/blockspacer/squarets.git"
      "")
endif()
conan_build_target_if(
  "squarets" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/squarets" # target to build
  ENABLE_SQUARETS
  ";-o;squarets:shared=False;\
-o;squarets:enable_clang_from_conan=False;\
-e;squarets:enable_tests=True")

# flex_squarets_plugin

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/flex_squarets_plugin")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/flex_squarets_plugin"
      "https://github.com/blockspacer/flex_squarets_plugin.git"
      "")
endif()
conan_build_target_if(
  "flex_squarets_plugin" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/flex_squarets_plugin" # target to build
  ENABLE_FLEX_SQUARETS_PLUGIN
  ";-o;flex_squarets_plugin:shared=True;\
-o;flex_squarets_plugin:enable_clang_from_conan=False;\
-e;flex_squarets_plugin:enable_tests=True")

# flex_pimpl_plugin

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/flex_pimpl_plugin")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/flex_pimpl_plugin"
      "https://github.com/blockspacer/flex_pimpl_plugin.git"
      "")
endif()
conan_build_target_if(
  "flex_pimpl_plugin" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/flex_pimpl_plugin" # target to build
  ENABLE_FLEX_PIMPL_PLUGIN
  ";-o;flex_pimpl_plugin:shared=True;\
-o;flex_pimpl_plugin:enable_clang_from_conan=False;\
-e;flex_pimpl_plugin:enable_tests=True")

# flex_typeclass_plugin

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/flex_typeclass_plugin")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/flex_typeclass_plugin"
      "https://github.com/blockspacer/flex_typeclass_plugin.git"
      "")
endif()
conan_build_target_if(
  "flex_typeclass_plugin" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/flex_typeclass_plugin" # target to build
  ENABLE_FLEX_TYPECLASS_PLUGIN
  ";-o;flex_typeclass_plugin:shared=True;\
-o;flex_typeclass_plugin:enable_clang_from_conan=False;\
-e;flex_typeclass_plugin:enable_tests=True")

# flex_meta_plugin

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/flex_meta_plugin")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/flex_meta_plugin"
      "https://github.com/blockspacer/flex_meta_plugin.git"
      "")
endif()
conan_build_target_if(
  "flex_meta_plugin" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/flex_meta_plugin" # target to build
  ENABLE_FLEX_META_PLUGIN
  ";-o;flex_meta_plugin:shared=True;\
-o;flex_meta_plugin:enable_clang_from_conan=False;\
-e;flex_meta_plugin:enable_tests=True")

# flex_meta_demo

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/flex_meta_demo")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/flex_meta_demo"
      "https://github.com/blockspacer/flex_meta_demo.git"
      "")
endif()
conan_build_target_if(
  "flex_meta_demo" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/flex_meta_demo" # target to build
  ENABLE_FLEX_META_DEMO
  ";-o;flex_meta_demo:enable_clang_from_conan=False;\
-e;flex_meta_demo:enable_tests=True")

# conan_protobuf

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/conan_protobuf")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/conan_protobuf"
      "https://github.com/blockspacer/conan_protobuf.git"
      "")
endif()
conan_build_target_if(
  "protobuf" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/conan_protobuf" # target to build
  ENABLE_CONAN_PROTOBUF)

# poco

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/poco")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/poco"
      "https://github.com/blockspacer/conan_poco.git"
      "")
endif()
conan_build_target_if(
  "poco" # target to clean
  "dev/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/poco" # target to build
  ENABLE_CONAN_POCO)

# flatc_conan

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/flatc_conan")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/flatc_conan"
      "https://github.com/blockspacer/conan-flatc_installer.git"
      "")
endif()
conan_build_target_if(
  "flatc_conan" # target to clean
  "conan/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/flatc_conan" # target to build
  ENABLE_FLATC_CONAN)

# mongo-c-driver

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/mongo-c-driver")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/mongo-c-driver"
      "https://github.com/blockspacer/mongo-c-driver.git"
      "")
endif()
conan_build_target_if(
  "mongo-c-driver" # target to clean
  "dev/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/mongo-c-driver" # target to build
  ENABLE_MONGO_C_DRIVER)

# mongo-cxx-driver

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/mongo-cxx-driver")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/mongo-cxx-driver"
      "https://github.com/blockspacer/mongo-cxx-driver-patched.git"
      "")
endif()
conan_build_target_if(
  "mongo-cxx-driver" # target to clean
  "dev/stable"
  "${CURRENT_SCRIPT_DIR}/.tmp/mongo-cxx-driver" # target to build
  ENABLE_MONGO_CXX_DRIVER)

# abseil

if(NOT EXISTS "${CURRENT_SCRIPT_DIR}/.tmp/abseil")
  git_clone("${CURRENT_SCRIPT_DIR}/.tmp/abseil"
      "https://github.com/abseil/abseil-cpp.git"
      "")
endif()
conan_build_target_if(
  "abseil" # target to clean
  "abseil/20200225@conan/stable" # target to create
  "${CURRENT_SCRIPT_DIR}/.tmp/abseil" # target to build
  ENABLE_ABSEIL)
