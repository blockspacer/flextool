# Note: COMMAND_EXPAND_LISTS is keyword is only available with CMake version >= 3.8
cmake_minimum_required(VERSION 3.8)

# project() must be before checks, see https://stackoverflow.com/a/26437667/10904212
set(PROJECT_NAME "flextool")
set(PROJECT_VERSION 1.0.0.1)
project(${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  LANGUAGES CXX C)

set(LIB_NAME ${PROJECT_NAME})

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

option(ENABLE_TESTS "Enable tests" OFF)

option(ENABLE_CPPCHECK "Enable cppcheck" OFF)

option(ENABLE_VALGRIND "Enable valgrind" OFF)

option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)

option(ENABLE_IWYU "Enable include-what-you-use" OFF)

option(ENABLE_CPPCLEAN "Enable cppclean" OFF)

# Things that can catch OCLINT
# http://oclint-docs.readthedocs.io/en/stable/rules/index.html
# OCLINT command-line manual
# https://oclint-docs.readthedocs.io/en/stable/manual/oclint.html
option(ENABLE_OCLINT "Enable oclint" OFF)

# TODO: __do_global_dtors_aux, base::debug::CollectStackTrace
option(ENABLE_VALGRIND_TESTS "Enable valgrind for unit tests" OFF)

# Hack to depend NOT on CONAN_PKG::libname, but on libname directly
# See for details
# https://docs.conan.io/en/latest/developing_packages/workspaces.html
set(${PROJECT_NAME}_LOCAL_BUILD FALSE CACHE BOOL "${PROJECT_NAME}_LOCAL_BUILD")
message(STATUS "${PROJECT_NAME}_LOCAL_BUILD=${${PROJECT_NAME}_LOCAL_BUILD}")

set(ENABLE_CLING TRUE CACHE BOOL "ENABLE_CLING")
message(STATUS "ENABLE_CLING=${ENABLE_CLING}")

set(ENABLE_CLANG_FROM_CONAN FALSE CACHE BOOL "ENABLE_CLANG_FROM_CONAN")
message(STATUS "ENABLE_CLANG_FROM_CONAN=${ENABLE_CLANG_FROM_CONAN}")

if(ENABLE_CLANG_FROM_CONAN AND ENABLE_CLING)
  message(FATAL_ERROR
    "don't use both ENABLE_CLING and ENABLE_CLANG_FROM_CONAN at the same time. cling already provides clang libtooling")
endif()

# --------------------------- conan configuration ------------------------------

# Note: FetchContent is available since CMake 3.11
# see https://cmake.org/cmake/help/git-master/module/FetchContent.html
# By default, everything is downloaded into your build directory
# Once CMake successfully downloads our external content, it sets two variables that can be used in CMakeLists.txt to locate the new data:
# 1 <resource_name>_SOURCE_DIR
#   specifies the location of the downloaded sources,
# 2 <resource_name>_BINARY_DIR
#   specifies where is the default build directory for the downloaded sources.
include(FetchContent)

set(FetchContent_cmake_utils "cmake_utils")
FetchContent_Declare(
  ${FetchContent_cmake_utils}
  PREFIX external_dependencies/${FetchContent_cmake_utils}
  GIT_REPOSITORY https://github.com/blockspacer/cmake_utils.git
  GIT_TAG origin/master
  # Disable warning about detached HEAD https://stackoverflow.com/a/36794768
  GIT_CONFIG        advice.detachedHead=false
  SOURCE_DIR        "${CMAKE_BINARY_DIR}/${FetchContent_cmake_utils}"
  BINARY_DIR        "${CMAKE_BINARY_DIR}/${FetchContent_cmake_utils}-build"
  CMAKE_ARGS        "-Wno-dev"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)
FetchContent_GetProperties(${FetchContent_cmake_utils})
if (NOT ${FetchContent_cmake_utils}_POPULATED)
  # Populate command can be used only once for every resource
  # during cmake configuration, which explains the conditional above.
  FetchContent_Populate(${FetchContent_cmake_utils})
endif()
message(STATUS "${FetchContent_cmake_utils}_SOURCE_DIR = ${${FetchContent_cmake_utils}_SOURCE_DIR}")
message(STATUS "${FetchContent_cmake_utils}_BINARY_DIR = ${${FetchContent_cmake_utils}_BINARY_DIR}")
include(${${FetchContent_cmake_utils}_SOURCE_DIR}/Utils.cmake)

set(FetchContent_conan_auto_install "conan_auto_install")
FetchContent_Declare(
  ${FetchContent_conan_auto_install}
  PREFIX external_dependencies/${FetchContent_conan_auto_install}
  GIT_REPOSITORY https://github.com/blockspacer/conan_auto_install.git
  GIT_TAG origin/master
  # Disable warning about detached HEAD https://stackoverflow.com/a/36794768
  GIT_CONFIG        advice.detachedHead=false
  SOURCE_DIR        "${CMAKE_BINARY_DIR}/${FetchContent_conan_auto_install}"
  BINARY_DIR        "${CMAKE_BINARY_DIR}/${FetchContent_conan_auto_install}-build"
  CMAKE_ARGS        "-Wno-dev"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)
FetchContent_GetProperties(${FetchContent_conan_auto_install})
if (NOT ${FetchContent_conan_auto_install}_POPULATED)
  # Populate command can be used only once for every resource
  # during cmake configuration, which explains the conditional above.
  FetchContent_Populate(${FetchContent_conan_auto_install})
endif()
message(STATUS "${FetchContent_conan_auto_install}_SOURCE_DIR = ${${FetchContent_conan_auto_install}_SOURCE_DIR}")
message(STATUS "${FetchContent_conan_auto_install}_BINARY_DIR = ${${FetchContent_conan_auto_install}_BINARY_DIR}")
include(${${FetchContent_conan_auto_install}_SOURCE_DIR}/conan-auto-install.cmake)
option(CONAN_AUTO_INSTALL "Let CMake call conan install automatically"
  OFF
)
if (CONAN_AUTO_INSTALL)
  set(CONAN_PROFILE
      "clang"
      CACHE STRING "Conan profile to use during installation")
  if (NOT CMAKE_BUILD_TYPE MATCHES "Debug" )
    set(conan_build_type "Release")
  else()
    set(conan_build_type "Debug")
  endif()
  # No --build=missing cause llvm requires long build
  conan_auto_install(
    CONAN_OPTIONS "--profile=${CONAN_PROFILE} -s build_type=${conan_build_type} -s cling_conan:build_type=Release -o openssl:shared=True"
    #FORCE
  )
endif()

if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR}/)
  include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
  include(${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake OPTIONAL)
  conan_basic_setup(
    # prevent conan_basic_setup from resetting cmake variables
    TARGETS
    KEEP_RPATHS
    # see https://github.com/conan-io/conan/issues/6012
    NO_OUTPUT_DIRS
    )
else()
  message (FATAL_ERROR "must use conan")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Generate clang compilation database
# see https://stackoverflow.com/a/31086619/10904212
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(cmake_platform_detection REQUIRED)
run_cmake_platform_detection()

find_package(cmake_build_options REQUIRED)
setup_default_build_type(RELEASE)
setup_cmake_build_options(RELEASE DEBUG)

message(STATUS "Compiler ${CMAKE_CXX_COMPILER}, version: ${CMAKE_CXX_COMPILER_VERSION}")

set_project_version(0 0 1) # from Utils.cmake

check_cmake_build_type_selected() # from Utils.cmake

enable_colored_diagnostics() # from Utils.cmake

print_cmake_system_info() # from Utils.cmake

check_supported_os() # from Utils.cmake

# Keep symbols for JIT resolution
set(LLVM_NO_DEAD_STRIP 1)

if(NOT TARGET CONAN_PKG::chromium_base)
  message(FATAL_ERROR "Use chromium_base from conan")
endif()

if(NOT TARGET CONAN_PKG::clang_folly_conan)
  message(FATAL_ERROR "Use clang_folly_conan from conan")
endif()


if(NOT TARGET CONAN_PKG::chromium_build_util)
  message(FATAL_ERROR "Use chromium_build_util from conan")
endif()

find_package(chromium_build_util REQUIRED)
#
if(TARGET chromium_build_util::chromium_build_util-static)
  set(build_util_LIB "chromium_build_util::chromium_build_util-static")
else()
  message(FATAL_ERROR "not supported: using system provided chromium_build_util library")
endif()

find_package(chromium_base REQUIRED)
if(TARGET chromium_base::chromium_base-static)
  set(base_LIB chromium_base::chromium_base-static)
else()
  message(FATAL_ERROR "not supported: using system provided chromium_base library")
endif()

# see https://doc.magnum.graphics/corrade/corrade-cmake.html#corrade-cmake-subproject
find_package(Corrade REQUIRED PluginManager)

if(NOT TARGET CONAN_PKG::corrade)
  message(FATAL_ERROR "Use corrade from conan")
endif()

find_package( X11 REQUIRED )
message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

find_package( EXPAT REQUIRED )
message(STATUS "EXPAT_LIBRARIES = ${EXPAT_LIBRARIES}")

find_package( ZLIB REQUIRED )
message(STATUS "ZLIB_LIBRARIES = ${ZLIB_LIBRARIES}")

#add_library( boost_outcome INTERFACE )
#target_include_directories( boost_outcome SYSTEM INTERFACE "submodules/boost.outcome/include" )
#get_target_property (BOOST_OUTCOME_IMPORTED_LOCATION boost_outcome INTERFACE_INCLUDE_DIRECTORIES)
#message( STATUS "boost_outcome=${BOOST_OUTCOME_IMPORTED_LOCATION}" )
#
#add_library(microsoft_gsl INTERFACE)
#target_include_directories(microsoft_gsl SYSTEM INTERFACE "submodules/GSL/include")
#get_target_property (microsoft_gsl_IMPORTED_LOCATION microsoft_gsl INTERFACE_INCLUDE_DIRECTORIES)
#message( STATUS "microsoft_gsl=${microsoft_gsl_IMPORTED_LOCATION}" )

message(STATUS "CMAKE_DL_LIBS = ${CMAKE_DL_LIBS}")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/)

set(flextool_include_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/flextool")
set(flextool_src_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformChecks.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CommonOptions.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProjectFiles.cmake)

add_library(${LIB_NAME}-test-includes INTERFACE)

# $<INSTALL_INTERFACE:...> is exported using install(EXPORT)
# $<BUILD_INTERFACE:...> is exported using export(), or when the target is used by another target in the same buildsystem
macro(add_relative_include_dir TARGET VISIBILITY_BUILD VISIBILITY_INSTALL NEW_ELEM)
  target_include_directories(${TARGET}
    ${VISIBILITY_BUILD} "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${NEW_ELEM}>"
    ${VISIBILITY_INSTALL} "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}/${NEW_ELEM}>"
  )
  target_include_directories( ${LIB_NAME}-test-includes SYSTEM INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/${NEW_ELEM} )
endmacro(add_relative_include_dir)

configure_file(${flextool_include_DIR}/version.hpp.in
  ${flextool_include_DIR}/version.hpp)

set_source_files_properties(${PROJECT_BINARY_DIR}/version.hpp
  PROPERTIES GENERATED 1)

add_executable(${LIB_NAME}
  ${flextool_SOURCES}
  ${flextool_src_DIR}/main.cc
  ${flextool_include_DIR}/version.hpp
)

#add_relative_include_dir(${LIB_NAME} PRIVATE PRIVATE "include/flextool")

add_relative_include_dir(${LIB_NAME} PUBLIC PUBLIC "include")

list(APPEND USED_3DPARTY_LIBS
  ${base_LIB}
  ${build_util_LIB}
)

if(NOT TARGET CONAN_PKG::flexlib)
  message(FATAL_ERROR "Use flexlib from conan")
endif()
set(flexlib CONAN_PKG::flexlib)
if(${PROJECT_NAME}_LOCAL_BUILD)
  set(flexlib flexlib)
endif(${PROJECT_NAME}_LOCAL_BUILD)

find_package(basis REQUIRED)
if(${basis_HEADER_DIR} STREQUAL "")
  message(FATAL_ERROR "unable to find basis_HEADER_DIR")
endif()

list(APPEND USED_3DPARTY_LIBS
  ${flexlib}
  ${basis_LIB}
  CONAN_PKG::doctest
  CONAN_PKG::clang_folly_conan
  CONAN_PKG::boost
  CONAN_PKG::double-conversion
  CONAN_PKG::glog
  CONAN_PKG::lz4
  CONAN_PKG::lzma
  CONAN_PKG::zstd
  CONAN_PKG::snappy
  CONAN_PKG::libsodium
  CONAN_PKG::libdwarf
  CONAN_PKG::bzip2
  CONAN_PKG::gflags
  CONAN_PKG::libunwind
  CONAN_PKG::libelf
  CONAN_PKG::xz_utils
  CONAN_PKG::corrade
  Corrade::PluginManager
  ${USED_BOOST_LIBS}
  #boost_outcome
  #microsoft_gsl
)

if(TARGET_EMSCRIPTEN)
  # use PROPERTY CXX_STANDARD 17
else()
  target_compile_features(${LIB_NAME}
    PUBLIC cxx_auto_type
    PRIVATE cxx_variadic_templates)
endif()

set(USED_SYSTEM_LIBS
    Threads::Threads # pthread, https://cmake.org/cmake/help/v3.13/module/FindThreads.html
    ${X11_LIBRARIES} # https://cmake.org/cmake/help/v3.13/module/FindX11.html
    ${CMAKE_DL_LIBS} # https://cmake.org/cmake/help/v3.13/variable/CMAKE_DL_LIBS.html
    EXPAT::EXPAT # https://cmake.org/cmake/help/v3.13/module/FindEXPAT.html
    ZLIB::ZLIB # https://cmake.org/cmake/help/v3.13/module/FindZLIB.html
               # Custom libs
    stdc++fs # C++17 std::filesystem
    # @note: Order matters https://stackoverflow.com/a/10269201/10904212
    ${LIBIBERTY_LIBRARY} # used by folly
    ${DOUBLE_CONVERSION_LIBRARY} # used by folly
    ${LIBEVENT_LIB} # used by folly
    ${LZ4_LIBRARY} # used by folly
    ${LIBUNWIND_LIBRARIES} # used by folly
    ${LIBLZMA_LIBRARIES} # used by folly
    CACHE INTERNAL "USED_SYSTEM_LIBS")

message(STATUS "flextool USED_3DPARTY_LIBS=${USED_3DPARTY_LIBS}")
target_link_libraries(${LIB_NAME} PUBLIC
  ${USED_3DPARTY_LIBS}
  ${USED_SYSTEM_LIBS}
)

# chromium_icu
if(TARGET_LINUX OR TARGET_EMSCRIPTEN)
  find_package(chromium_icu REQUIRED)
  if(NOT TARGET chromium_icu::chromium_icu-static)
    message(FATAL_ERROR "not supported: using system provided libevent library")
  endif()
elseif(TARGET_WINDOWS)
  # skip
else()
  message(FATAL_ERROR "chromium_icu not found")
endif()

target_link_libraries( ${LIB_NAME} PRIVATE
  CONAN_PKG::entt
  chromium_icu::chromium_icu-static
)

# without rtti you will get error:
# ERROR: boost::bad_any_cast: failed conversion using boost::any_cast
target_compile_options(${LIB_NAME} PRIVATE
  -frtti)

set(DEBUG_LIBRARY_SUFFIX "")
set_target_properties(${LIB_NAME}
  PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" # TODO: /lib
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" # TODO: /lib
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" # TODO: /bin
    ENABLE_EXPORTS 1
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    CMAKE_CXX_STANDARD_REQUIRED ON
)

if(${PROJECT_NAME}_LOCAL_BUILD)
  # bin
  list(APPEND CMAKE_PROGRAM_PATH ${CMAKE_BINARY_DIR})
  list(APPEND CMAKE_PROGRAM_PATH ${CMAKE_CURRENT_BINARY_DIR})
endif(${PROJECT_NAME}_LOCAL_BUILD)

# POSITION_INDEPENDENT_CODE for -fPIC
set_property(TARGET ${LIB_NAME}
  PROPERTY POSITION_INDEPENDENT_CODE ON)

if(ENABLE_CLING)
  find_package(Cling REQUIRED)

  list(APPEND CLING_DEFINITIONS CLING_IS_ON=1)
  target_link_libraries(${LIB_NAME} PUBLIC
    CONAN_PKG::cling_conan
  )

  if(MSVC)
    set_target_properties(${LIB_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS 1)
    set_property(
      TARGET ${LIB_NAME}
      APPEND_STRING
      PROPERTY LINK_FLAGS
               "/EXPORT:?setValueNoAlloc@internal@runtime@cling@@YAXPEAX00D_K@Z
                /EXPORT:?setValueNoAlloc@internal@runtime@cling@@YAXPEAX00DM@Z
                /EXPORT:cling_runtime_internal_throwIfInvalidPointer")
  endif()

  target_compile_definitions(${LIB_NAME} PRIVATE CLING_IS_ON=1)
endif(ENABLE_CLING)

if(ENABLE_CLANG_FROM_CONAN)
  target_link_libraries( ${LIB_NAME} PUBLIC#PRIVATE
    CONAN_PKG::libclang
    CONAN_PKG::clang_tooling
    CONAN_PKG::clang_tooling_core
    CONAN_PKG::llvm_support
  )
endif(ENABLE_CLANG_FROM_CONAN)

target_compile_options(${LIB_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:
    /W3 # Set warning level
  >
  $<$<CXX_COMPILER_ID:GNU>:
    -Wformat=2
    -Wall
    -W
    -Wpedantic
    # https://github.com/google/sanitizers/wiki/AddressSanitizer#using-addresssanitizer
    -fno-omit-frame-pointer
  >
  $<$<CXX_COMPILER_ID:Clang>:
    -Wformat=2
    -Wall
    -W
    -Wpedantic
    -Wdeprecated-register
    # https://github.com/google/sanitizers/wiki/AddressSanitizer#using-addresssanitizer
    -fno-omit-frame-pointer
  >
)

target_compile_definitions(${LIB_NAME} PRIVATE
  ${flextool_PRIVATE_DEFINES}
  # https://stackoverflow.com/a/30877725
  BOOST_SYSTEM_NO_DEPRECATED
  BOOST_ERROR_CODE_HEADER_ONLY
  # TODO: make program_options work without rtti (boost::bad_any_cast)
  #BOOST_NO_RTTI
  #BOOST_NO_TYPEID
  # TODO
  #BOOST_NO_EXCEPTIONS
)

target_compile_definitions(${LIB_NAME} PUBLIC
  ${flextool_PUBLIC_DEFINES}
  ${CLING_DEFINITIONS}
)

# remove old resources dir
#add_custom_command( TARGET ${LIB_NAME} PRE_BUILD
#  COMMAND ${CMAKE_COMMAND} -E remove_directory
#    $<TARGET_FILE_DIR:${LIB_NAME}>/resources )

# copy new resources
add_custom_command( TARGET ${LIB_NAME} PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/
    $<TARGET_FILE_DIR:${LIB_NAME}>/resources )

# install and export steps
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Exports.cmake)

function(get_depends_properties RESULT_VARIABLE_NAME TARGET PROPERTIES)
  foreach(PROPERTY ${PROPERTIES})
    set(RESULT_${PROPERTY})
  endforeach()

  get_target_property(_TARGET_TYPE ${TARGET} TYPE)
  if(_TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
    get_target_property(INTERFACE_LINK_LIBRARIES ${TARGET} INTERFACE_LINK_LIBRARIES)
    if(INTERFACE_LINK_LIBRARIES)
      foreach(INTERFACE_LINK_LIBRARY ${INTERFACE_LINK_LIBRARIES})
        if(TARGET ${INTERFACE_LINK_LIBRARY})
          get_depends_properties(TMP ${INTERFACE_LINK_LIBRARY}
            "${PROPERTIES}")
          foreach(PROPERTY ${PROPERTIES})
            set(RESULT_${PROPERTY}
              ${RESULT_${PROPERTY}}
              ${TMP_${PROPERTY}}
            )
          endforeach()
        endif()
      endforeach()
    endif()
  else()
    get_target_property(LINK_LIBRARIES ${TARGET} LINK_LIBRARIES)
    if(LINK_LIBRARIES)
      foreach(LINK_LIBRARY ${LINK_LIBRARIES})
        if(TARGET ${LINK_LIBRARY})
          get_depends_properties(TMP ${LINK_LIBRARY}
            "${PROPERTIES}")
          foreach(PROPERTY ${PROPERTIES})
            set(RESULT_${PROPERTY}
              ${RESULT_${PROPERTY}}
              ${TMP_${PROPERTY}}
            )
          endforeach()
        endif()
      endforeach()
    endif()
  endif()

  foreach(PROPERTY ${PROPERTIES})
    get_target_property(TMP ${TARGET} ${PROPERTY})
    if(TMP)
      set(RESULT_${PROPERTY}
        ${RESULT_${PROPERTY}}
        ${TMP}
      )
    endif()
    set(${RESULT_VARIABLE_NAME}_${PROPERTY} ${RESULT_${PROPERTY}} PARENT_SCOPE)
  endforeach()
endfunction()

function(get_all_include_directories RESULT_VARIABLE_NAME TARGET)
  #get_depends_properties(RESULT ${TARGET}
  #  "INTERFACE_INCLUDE_DIRECTORIES;INTERFACE_SYSTEM_INCLUDE_DIRECTORIES;INCLUDE_DIRECTORIES;SYSTEM_INCLUDE_DIRECTORIES")
  get_depends_properties(RESULT ${TARGET}
    "INTERFACE_INCLUDE_DIRECTORIES;INTERFACE_SYSTEM_INCLUDE_DIRECTORIES")
  set(RESULT
    ${RESULT}
    ${RESULT_INTERFACE_INCLUDE_DIRECTORIES}
    ${RESULT_INTERFACE_SYSTEM_INCLUDE_DIRECTORIES}
    ${RESULT_INCLUDE_DIRECTORIES}
    ${RESULT_SYSTEM_INCLUDE_DIRECTORIES}
  )

  get_target_property(_TARGET_TYPE ${TARGET} TYPE)

  if(_TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
    get_target_property(INTERFACE_INCLUDE_DIRECTORIES ${TARGET} INTERFACE_INCLUDE_DIRECTORIES)
    if(INTERFACE_INCLUDE_DIRECTORIES)
      set(RESULT
        ${RESULT}
        ${INTERFACE_INCLUDE_DIRECTORIES}
      )
    endif()
  else()
    get_target_property(INCLUDE_DIRECTORIES ${TARGET} INCLUDE_DIRECTORIES)
    if(INCLUDE_DIRECTORIES)
      set(RESULT
        ${RESULT}
        ${INCLUDE_DIRECTORIES}
      )
    endif()
  endif()
  if(RESULT)
    list(REMOVE_DUPLICATES RESULT)
  endif()
  set(${RESULT_VARIABLE_NAME} ${RESULT} PARENT_SCOPE)
endfunction()


function(get_target_sources RESULT_VARIABLE_NAME TARGET)
  get_target_property(_TARGET_TYPE ${TARGET} TYPE)
  if(NOT _TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
    get_target_property(RESULT
      ${TARGET} SOURCES
    )
  endif()
  set(${RESULT_VARIABLE_NAME} ${RESULT} PARENT_SCOPE)
endfunction()

function(get_all_compile_definitions RESULT_VARIABLE_NAME TARGET)
  #get_depends_properties(RESULT ${TARGET}
  #  "INTERFACE_COMPILE_DEFINITIONS;COMPILE_DEFINITIONS")
  get_depends_properties(RESULT ${TARGET}
    "INTERFACE_COMPILE_DEFINITIONS")
  set(RESULT
    ${RESULT}
    ${RESULT_INTERFACE_COMPILE_DEFINITIONS}
    ${RESULT_COMPILE_DEFINITIONS}
  )
  get_target_property(COMPILE_DEFINITIONS ${TARGET} COMPILE_DEFINITIONS)
  if(COMPILE_DEFINITIONS)
    set(RESULT
      ${RESULT}
      ${COMPILE_DEFINITIONS}
    )
  endif()
  if(RESULT)
    list(REMOVE_DUPLICATES RESULT)
  endif()
  set(${RESULT_VARIABLE_NAME} ${RESULT} PARENT_SCOPE)
endfunction()

# prepends file path and line number to string
# EXAMPLE:
# FROM_HERE("invalid ...")
# message("${FROM_HERE}")
macro(FROM_HERE _MSG)
  set(FROM_HERE "${CMAKE_CURRENT_LIST_FILE}:${CMAKE_CURRENT_LIST_LINE}: ${_MSG}")
endmacro(FROM_HERE)

# EXAMPLE:
# message_if(ARGUMENTS_VERBOSE STATUS
#   "CPPCHECK_HTMLREPORT=${CPPCHECK_HTMLREPORT}"
# )
macro(message_if _CONDITION _MSG_TYPE _MSG)
  if(${_CONDITION})
    message(${_MSG_TYPE} "${_MSG}")
  endif(${_CONDITION})
endmacro(message_if)

# EXAMPLE:
# find_program_helper("cppcheck-htmlreport"
#   PATHS
#     ${CONAN_BIN_DIRS}
#     ${CONAN_BIN_DIRS_LLVM_TOOLS}
#     # to use `cppcheck-htmlreport` from cmake subfolder
#     ${CMAKE_SOURCE_DIR}/cmake
#     ${CMAKE_CURRENT_SOURCE_DIR}/cmake
#   NO_SYSTEM_ENVIRONMENT_PATH
#   NO_CMAKE_SYSTEM_PATH
#   REQUIRED
#   OUT_VAR CPPCHECK_HTMLREPORT
#   VERBOSE TRUE
#   TEXT "not found"
# )
# NOTE: TEXT must be last argument
function(find_program_helper)
  # see https://cliutils.gitlab.io/modern-cmake/chapters/basics/functions.html
  set(options
    CHECK_NOT_EMPTY # check: must be not empty
    REQUIRED
  )
  set(oneValueArgs
    VERBOSE
    OUT_VAR
  )
  set(multiValueArgs
    TEXT # text for displayed message
  )
  #
  cmake_parse_arguments(
    ARGUMENTS # prefix of output variables
    "${options}" # list of names of the boolean arguments (only defined ones will be true)
    "${oneValueArgs}" # list of names of mono-valued arguments
    "${multiValueArgs}" # list of names of multi-valued arguments (output variables are lists)
    ${ARGN} # arguments of the function to parse, here we take the all original ones
  )
  #
  set(args_unparsed ${ARGUMENTS_UNPARSED_ARGUMENTS})
  if(${ARGUMENTS_VERBOSE})
    message(STATUS "validate: ARGUMENTS_UNPARSED_ARGUMENTS=${ARGUMENTS_UNPARSED_ARGUMENTS}")
  endif(${ARGUMENTS_VERBOSE})

  # default
  set(TEXT
    "unable to find ${ARGUMENTS_UNPARSED_ARGUMENTS}"
  )
  if(ARGUMENTS_TEXT)
    set(TEXT
      ${ARGUMENTS_TEXT}
    )
  endif(ARGUMENTS_TEXT)

  if("${ARGUMENTS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(FATAL_ERROR "ARGUMENTS_UNPARSED_ARGUMENTS must be not empty")
  endif()

  # NOTE: cmake cached result of previous find_program,
  # so added `${ARGUMENTS_OUT_VAR}_` before FOUND_PROGRAM
  # to avoid storing result of find_program into cache
  find_program(${ARGUMENTS_OUT_VAR}_FOUND_PROGRAM ${ARGUMENTS_UNPARSED_ARGUMENTS})
  if(ARGUMENTS_REQUIRED AND NOT ${ARGUMENTS_OUT_VAR}_FOUND_PROGRAM)
    message(FATAL_ERROR ${TEXT})
  endif(ARGUMENTS_REQUIRED AND NOT ${ARGUMENTS_OUT_VAR}_FOUND_PROGRAM)
  if(${ARGUMENTS_VERBOSE})
    message(STATUS "ARGUMENTS_UNPARSED_ARGUMENTS=${ARGUMENTS_UNPARSED_ARGUMENTS}")
    message(STATUS "${ARGUMENTS_OUT_VAR}_FOUND_PROGRAM=${${ARGUMENTS_OUT_VAR}_FOUND_PROGRAM}")
  endif(${ARGUMENTS_VERBOSE})

  set(${ARGUMENTS_OUT_VAR} ${${ARGUMENTS_OUT_VAR}_FOUND_PROGRAM} PARENT_SCOPE)
endfunction(find_program_helper)

# USAGE:
# validate(
#   ${MY_VAR}
#   TYPE FATAL_ERROR
#   CHECK_NOT_EMPTY
#   TEXT "${CMAKE_CURRENT_LIST_FILE}:${CMAKE_CURRENT_LIST_LINE}:"
#        "invalid MY_VAR"
#   VERBOSE TRUE
# )
# NOTE: TEXT must be after ${MY_VAR}
function(validate)
  # see https://cliutils.gitlab.io/modern-cmake/chapters/basics/functions.html
  set(options
    CHECK_NOT_EMPTY # check: must be not empty
  )
  set(oneValueArgs
    TYPE # may be FATAL_ERROR, STATUS, etc.
    VERBOSE
  )
  set(multiValueArgs
    TEXT # text for displayed message
  )
  #
  cmake_parse_arguments(
    ARGUMENTS # prefix of output variables
    "${options}" # list of names of the boolean arguments (only defined ones will be true)
    "${oneValueArgs}" # list of names of mono-valued arguments
    "${multiValueArgs}" # list of names of multi-valued arguments (output variables are lists)
    ${ARGN} # arguments of the function to parse, here we take the all original ones
  )
  #
  set(args_unparsed ${ARGUMENTS_UNPARSED_ARGUMENTS})
  if(${ARGUMENTS_VERBOSE})
    message(STATUS "validate: ARGUMENTS_UNPARSED_ARGUMENTS=${ARGUMENTS_UNPARSED_ARGUMENTS}")
  endif(${ARGUMENTS_VERBOSE})

  # default
  set(TEXT
    "check failed: invalid ${ARGUMENTS_UNPARSED_ARGUMENTS}"
  )
  if(ARGUMENTS_TEXT)
    set(TEXT
      ${ARGUMENTS_TEXT}
    )
  endif(ARGUMENTS_TEXT)

  # default
  set(TYPE
    FATAL_ERROR
  )
  if(ARGUMENTS_TYPE)
    set(TYPE
      ${ARGUMENTS_TYPE}
    )
  endif(ARGUMENTS_TYPE)

  # check: must be not empty
  if(ARGUMENTS_CHECK_NOT_EMPTY)
    if("${ARGUMENTS_UNPARSED_ARGUMENTS}" STREQUAL "")
      message(${TYPE} ${TEXT})
    endif()
  else()
    message(FATAL_ERROR
      "you must provide at least one validation TYPE"
    )
  endif()
endfunction()

# EXAMPLE (joins defines by -D):
# get_all_compile_definitions(DEFINES_VARIABLE_NAME, TARGET_NAME)
# join_with_separator(DEFINES_VARIABLE_NAME, DEFINES_RESULT, "-D")
function(join_with_separator)
  # see https://cliutils.gitlab.io/modern-cmake/chapters/basics/functions.html
  set(options
    PATH_MUST_EXIST
  )
  set(oneValueArgs
    START_SEPARATOR
    END_SEPARATOR
    RESULT_VARIABLE_NAME
  )
  set(multiValueArgs
    INPUT
  )
  #
  cmake_parse_arguments(
    ARGUMENTS # prefix of output variables
    "${options}" # list of names of the boolean arguments (only defined ones will be true)
    "${oneValueArgs}" # list of names of mono-valued arguments
    "${multiValueArgs}" # list of names of multi-valued arguments (output variables are lists)
    ${ARGN} # arguments of the function to parse, here we take the all original ones
  )
  #
  foreach(ITEM ${ARGUMENTS_INPUT})
    set({ITEM_WITH_SEPARATOR
      ${ARGUMENTS_START_SEPARATOR}${ITEM}${ARGUMENTS_END_SEPARATOR}
    )
    set(RESULT
      ${RESULT}
      "${ITEM_WITH_SEPARATOR}"
    )
  endforeach()
  set(${ARGUMENTS_RESULT_VARIABLE_NAME} ${RESULT} PARENT_SCOPE)
endfunction()

function(add_cppcheck_target)
  # see https://cliutils.gitlab.io/modern-cmake/chapters/basics/functions.html
  set(options
    # empty
  )
  set(oneValueArgs
    TARGET_NAME
    HTML_REPORT
  )
  set(multiValueArgs
    CPPCHECK_FULL_CMD
    # adds options to default options
    EXTRA_OPTIONS
  )
  #
  cmake_parse_arguments(
    ARGUMENTS # prefix of output variables
    "${options}" # list of names of the boolean arguments (only defined ones will be true)
    "${oneValueArgs}" # list of names of mono-valued arguments
    "${multiValueArgs}" # list of names of multi-valued arguments (output variables are lists)
    ${ARGN} # arguments of the function to parse, here we take the all original ones
  )
  #
  set(args_unparsed ${ARGUMENTS_UNPARSED_ARGUMENTS})

  FROM_HERE("invalid ARGUMENTS_TARGET_NAME")
  validate(CHECK_NOT_EMPTY
    ${ARGUMENTS_TARGET_NAME}
    TEXT "${FROM_HERE}"
  )
  #
  FROM_HERE("invalid ARGUMENTS_CPPCHECK_FULL_CMD")
  validate(CHECK_NOT_EMPTY
    ${ARGUMENTS_CPPCHECK_FULL_CMD}
    TEXT "${FROM_HERE}"
  )
  message_if(ARGUMENTS_VERBOSE STATUS
    "ARGUMENTS_CPPCHECK_FULL_CMD=${ARGUMENTS_CPPCHECK_FULL_CMD}"
  )
  #
  set(CPPCHECK_FULL_CMD
    ${ARGUMENTS_CPPCHECK_FULL_CMD}
    ${ARGUMENTS_EXTRA_OPTIONS}
  )
  if(NOT ${ARGUMENTS_HTML_REPORT} STREQUAL "")
    message_if(ARGUMENTS_VERBOSE STATUS
      "ARGUMENTS_HTML_REPORT=${ARGUMENTS_HTML_REPORT}"
    )
    #
    # NOTE: must be ending argument,
    # uses `2>` to re-route console output
    set(CPPCHECK_FULL_CMD
      ${CPPCHECK_FULL_CMD}
      --xml
      --xml-version=2
      2> ${CMAKE_BINARY_DIR}/${ARGUMENTS_TARGET_NAME}_cppcheck_report.xml
    )
    set(HTMLREPORT_FULL_CMD
      ${ARGUMENTS_HTML_REPORT}
      --file=${CMAKE_BINARY_DIR}/${ARGUMENTS_TARGET_NAME}_cppcheck_report.xml
      --report-dir=${CMAKE_BINARY_DIR}/${ARGUMENTS_TARGET_NAME}_report
      # directory with explored source
      --source-dir=
      --title=${ARGUMENTS_TARGET_NAME}
    )
  else() # ARGUMENTS_HTML_REPORT
    set(HTMLREPORT_FULL_CMD
      ${CMAKE_COMMAND}
      -E
      echo
      "html report disabled"
    )
  endif() # ARGUMENTS_HTML_REPORT
  #
  # USAGE:
  # cmake -E time cmake --build . --target TARGET_NAME_run_cppcheck
  message_if(ARGUMENTS_VERBOSE STATUS
    "ARGUMENTS_TARGET_NAME=${ARGUMENTS_TARGET_NAME}"
  )
  add_custom_target(${ARGUMENTS_TARGET_NAME}_run_cppcheck
    # remove old report
    COMMAND
      # Remove the file(s).
      # If any of the listed files already do not exist,
      # the command returns a non-zero exit code,
      # but no message is logged.
      # The -f option changes the behavior to return a zero exit code
      # (i.e. success) in such situations instead.
      ${CMAKE_COMMAND}
      -E
      remove
      -f
      ${CMAKE_BINARY_DIR}/${ARGUMENTS_TARGET_NAME}_cppcheck_report.xml
    # create report dir
    COMMAND
      ${CMAKE_COMMAND}
      -E
      make_directory
      ${CMAKE_BINARY_DIR}/${ARGUMENTS_TARGET_NAME}_report
    # remove old report
    COMMAND
      # Remove the file(s).
      # If any of the listed files already do not exist,
      # the command returns a non-zero exit code,
      # but no message is logged.
      # The -f option changes the behavior to return a zero exit code
      # (i.e. success) in such situations instead.
      ${CMAKE_COMMAND}
      -E
      remove
      -f
      ${CMAKE_BINARY_DIR}/${ARGUMENTS_TARGET_NAME}_report/index.html
    # print command that will be executed
    # NOTE: uses COMMAND_EXPAND_LISTS
    # to support generator expressions
    # see https://cmake.org/cmake/help/v3.13/command/add_custom_target.html
    COMMAND
      "${CMAKE_COMMAND}"
      -E
      echo
      "executing command: ${CPPCHECK_FULL_CMD}"
    # Run cppcheck static analysis
    COMMAND
      "${CPPCHECK_FULL_CMD}"
    # print command that will be executed
    # NOTE: uses COMMAND_EXPAND_LISTS
    # to support generator expressions
    # see https://cmake.org/cmake/help/v3.13/command/add_custom_target.html
    COMMAND
      "${CMAKE_COMMAND}"
      -E
      echo
      "executing command: ${HTMLREPORT_FULL_CMD}"
    # Generate html output
    COMMAND
      ${CMAKE_COMMAND}
      -E
      time
      "${HTMLREPORT_FULL_CMD}"
    VERBATIM
    # NOTE: uses COMMAND_EXPAND_LISTS
    # to support generator expressions
    # see https://cmake.org/cmake/help/v3.13/command/add_custom_target.html
    COMMAND_EXPAND_LISTS
    #USES_TERMINAL
    # Set work directory for target
    WORKING_DIRECTORY
      ${CMAKE_BINARY_DIR}
    # Echo what is being done
    COMMENT "running cppcheck"
  )
endfunction(add_cppcheck_target)

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_run_cppcheck
# EXAMPLE:
# cppcheck_enabler(
#   PATHS
#     # to use cppcheck_installer from conan
#     ${CONAN_BIN_DIRS}
#     ${CONAN_BIN_DIRS_LLVM_TOOLS}
#     # to use `cppcheck-htmlreport` from cmake subfolder
#     ${CMAKE_SOURCE_DIR}/cmake
#     ${CMAKE_CURRENT_SOURCE_DIR}/cmake
#   NO_SYSTEM_ENVIRONMENT_PATH
#   NO_CMAKE_SYSTEM_PATH
#   IS_ENABLED
#     ${ENABLE_CPPCHECK}
#   CHECK_TARGETS
#     ${LIB_NAME}
#   EXTRA_OPTIONS
#     --enable=all
#   HTML_REPORT TRUE
#   VERBOSE
# )
# @see https://github.com/cyyever/cmake/blob/master/static_code_analysis.cmake
# @see https://gitlab.cern.ch/Caribou/peary/blob/master/cmake/clang-cpp-checks.cmake
# @see https://github.com/doevelopper/cfs-third-parties/blob/master/src/main/resources/cmake/macros/CPPCheck.cmake
# @see https://habr.com/ru/post/210256/
# NOTE: cppcheck have bug: it does not report errors
# in file if it can not parse that file
# see https://sourceforge.net/p/cppcheck/discussion/general/thread/2a2e3ce6/
function(cppcheck_enabler)
  # see https://cliutils.gitlab.io/modern-cmake/chapters/basics/functions.html
  set(options
    VERBOSE
    REQUIRED
  )
  set(oneValueArgs
    IS_ENABLED
    HTML_REPORT
    STANDALONE_TARGET
  )
  set(multiValueArgs
    # completely changes options to provided options
    OVERRIDE_OPTIONS
    # adds options to default options
    EXTRA_OPTIONS
    # Will collect include dirs, defines and source files
    # from cmake targets
    # NOTE: Why not compile_commands.json?
    # - cmake can not generate that file on per-target basis
    # or into custom out dir
    # NOTE: Why not CMAKE_CXX_CPPCHECK?
    # - we want to run custom cppcheck target without
    # need to build whole poject per each check
    CHECK_TARGETS
  )
  #
  cmake_parse_arguments(
    ARGUMENTS # prefix of output variables
    "${options}" # list of names of the boolean arguments (only defined ones will be true)
    "${oneValueArgs}" # list of names of mono-valued arguments
    "${multiValueArgs}" # list of names of multi-valued arguments (output variables are lists)
    ${ARGN} # arguments of the function to parse, here we take the all original ones
  )
  #
  set(args_unparsed ${ARGUMENTS_UNPARSED_ARGUMENTS})
  #
  # default options
  #
  # Set cppcheck supressions file.
  # This file must contain enough suppressions to result in no cppcheck warnings
  # see for example usage https://gitlab.kitware.com/vtk/vtk/blob/master/CMake/VTKcppcheckSuppressions.txt
  #
  # EXAMPLE FILE CONTENTS:
  #
  # // system libs
  # *:*/usr/include/*
  # *:/usr/local/*
  #
  # // Gives too many false positives.
  # unmatchedSuppression
  #
  # // False positives
  # syntaxError:*my_types.hpp
  # syntaxError:*my_conf.hpp
  set(CPPCHECK_SUPRESSIONS
    ${PROJECT_SOURCE_DIR}/cmake/cppcheck.cfg
  )
  # set supressions file only if it exists
  if(EXISTS "${CPPCHECK_SUPRESSIONS}")
    set(CPPCHECK_DEFAULT_SUPRESSIONS_ARG
      --suppressions-list=${CPPCHECK_SUPRESSIONS}
    )
    message(STATUS
      "found cppcheck config file: ${CPPCHECK_SUPRESSIONS}"
    )
  else()
    message(WARNING
      "unable to find cppcheck config file: ${CPPCHECK_SUPRESSIONS}"
    )
  endif()
  # Set message template
  set(CPPCHECK_TEMPLATE
    "[{file}:{line}] ({severity}) {message} ({id}) ({callstack})"
  )
  # Set cppcheck cache directory
  set(CPPCHECK_BUILD_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/cppcheck_cache
  )
  #
  set(CPPCHECK_OPTIONS
    #--enable=warning,performance,portability,information,missingInclude
    #--enable=all
    # more info in console
    --verbose
    # less info in console
    #--quiet
    # require well formed path for headers
    # NOTE: The normal code analysis is disabled by check-config.
    #--check-config
    # check all #ifdef
    #--force
    # see CMAKE_EXPORT_COMPILE_COMMANDS
    #--project=${CMAKE_BINARY_DIR}/compile_commands.json
    # Enable inline suppressions.
    # Use them by placing comments in the form:
    # // cppcheck-suppress memleak
    # before the line to suppress.
    --inline-suppr
    --platform=native
    --cppcheck-build-dir=${CPPCHECK_BUILD_DIR}
    --template="${CPPCHECK_TEMPLATE}"
    ${CPPCHECK_DEFAULT_SUPRESSIONS_ARG}
    --report-progress
    #--language=c++ #
    --library=std.cfg
    --library=posix.cfg
    --library=qt.cfg
    --library=boost.cfg
    #--error-exitcode=1
  )
  if(ARGUMENTS_OVERRIDE_OPTIONS)
    set(CPPCHECK_OPTIONS ${ARGUMENTS_OVERRIDE_OPTIONS})
  else()
    message_if(ARGUMENTS_VERBOSE STATUS
      "cppcheck: no OVERRIDE_OPTIONS provided"
    )
    # skip, use defaults
  endif()
  #
  list(APPEND CPPCHECK_OPTIONS ${ARGUMENTS_EXTRA_OPTIONS})
  message_if(ARGUMENTS_VERBOSE STATUS
    "ARGUMENTS_EXTRA_OPTIONS=${ARGUMENTS_EXTRA_OPTIONS}"
  )
  #
  if(${ARGUMENTS_IS_ENABLED})
    message_if(ARGUMENTS_VERBOSE STATUS
      "cppcheck enabled"
    )

    # to use `cppcheck` from conan
    list(APPEND CMAKE_PROGRAM_PATH ${CONAN_BIN_DIRS})

    # use cppcheck_installer from conan
    find_program_helper(cppcheck
      #PATHS
      #  ${CONAN_BIN_DIRS}
      #  ${CONAN_BIN_DIRS_LLVM_TOOLS}
      #NO_SYSTEM_ENVIRONMENT_PATH
      #NO_CMAKE_SYSTEM_PATH
      ${ARGUMENTS_UNPARSED_ARGUMENTS}
      REQUIRED
      OUT_VAR CPPCHECK_PROGRAM
      VERBOSE TRUE
    )

    message_if(ARGUMENTS_VERBOSE STATUS
      "ARGUMENTS_HTML_REPORT=${ARGUMENTS_HTML_REPORT}"
    )

    if(${ARGUMENTS_HTML_REPORT})
      list(APPEND CPPCHECK_OPTIONS
        --xml
        --xml-version=2
      )
    endif(${ARGUMENTS_HTML_REPORT})

    if(CPPCHECK_PROGRAM)
      # Create cppcheck cache directory
      file(MAKE_DIRECTORY ${CPPCHECK_BUILD_DIR})

      # Set cppcheck program + options.
      list(APPEND CPPCHECK_RUNNABLE
        ${CPPCHECK_PROGRAM}
      )

      message_if(ARGUMENTS_VERBOSE STATUS
        "CPPCHECK_RUNNABLE=${CPPCHECK_RUNNABLE}"
      )

      if(${ARGUMENTS_HTML_REPORT})
        # to use `cppcheck-htmlreport` from cmake subfolder
        list(APPEND CMAKE_PROGRAM_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
        list(APPEND CMAKE_PROGRAM_PATH ${CMAKE_SOURCE_DIR}/cmake)
        #
        find_program_helper("cppcheck-htmlreport"
          PATHS
            #${CONAN_BIN_DIRS}
            #${CONAN_BIN_DIRS_LLVM_TOOLS}
            ## to use `cppcheck-htmlreport` from cmake subfolder
            #${CMAKE_SOURCE_DIR}/cmake
            #${CMAKE_CURRENT_SOURCE_DIR}/cmake
            ${ARGUMENTS_UNPARSED_ARGUMENTS}
          NO_SYSTEM_ENVIRONMENT_PATH
          NO_CMAKE_SYSTEM_PATH
          REQUIRED
          OUT_VAR CPPCHECK_HTMLREPORT
          VERBOSE TRUE
        )
      else()
        set(CPPCHECK_HTMLREPORT "")
      endif(${ARGUMENTS_HTML_REPORT})

      # create separate target for cppcheck
      if(ARGUMENTS_STANDALONE_TARGET)
        add_cppcheck_target(
          TARGET_NAME ${ARGUMENTS_STANDALONE_TARGET}
          CPPCHECK_FULL_CMD
            ${CPPCHECK_RUNNABLE}
          HTML_REPORT ${CPPCHECK_HTMLREPORT}
          EXTRA_OPTIONS ${CPPCHECK_OPTIONS}
        )
      endif(ARGUMENTS_STANDALONE_TARGET)

      # collect headers and defines from existing target
      if(ARGUMENTS_CHECK_TARGETS)
        if(ARGUMENTS_VERBOSE)
          message(STATUS "cppcheck: ARGUMENTS_CHECK_TARGETS=${ARGUMENTS_CHECK_TARGETS}")
        endif(ARGUMENTS_VERBOSE)
        foreach(TARGET_NAME ${ARGUMENTS_CHECK_TARGETS})
          if(ARGUMENTS_VERBOSE)
            message(STATUS "enabled cppcheck for target ${TARGET_NAME}")
          endif(ARGUMENTS_VERBOSE)
          #
          get_all_compile_definitions(collected_defines
            ${TARGET_NAME}
          )
          #
          get_all_include_directories(collected_includes
            ${TARGET_NAME}
          )
          #
          get_target_sources(TARGET_SOURCES
            ${TARGET_NAME}
          )
          #
          add_cppcheck_target(
            TARGET_NAME ${LIB_NAME}
            CPPCHECK_FULL_CMD
              ${CPPCHECK_RUNNABLE}
              # NOTE: generator expression, expands during build time
              # if the ${ITEM} is non-empty, than append it
              $<$<BOOL:${collected_defines}>:-D$<JOIN:${collected_defines}, -D>>
              # NOTE: generator expression, expands during build time
              # if the ${ITEM} is non-empty, than append it
              $<$<BOOL:${collected_includes}>:-I$<JOIN:${collected_includes}, -I>>
              ${TARGET_SOURCES}
            HTML_REPORT ${ARGUMENTS_HTML_REPORT}
            EXTRA_OPTIONS ${CPPCHECK_OPTIONS}
          )
          # run valgrind on each build of target
          add_dependencies(
            ${LIB_NAME}
            ${LIB_NAME}_run_cppcheck
          )
        endforeach()
      else(ARGUMENTS_CHECK_TARGETS)
        if(ARGUMENTS_VERBOSE)
          message(STATUS "cppcheck: no CHECK_TARGETS provided")
        endif(ARGUMENTS_VERBOSE)
      endif(ARGUMENTS_CHECK_TARGETS)
    else(CPPCHECK_PROGRAM)
      message(WARNING "Program 'cppcheck' not found, unable to run 'cppcheck'.")
    endif(CPPCHECK_PROGRAM)
  else() # ARGUMENTS_IS_ENABLED
    if(ARGUMENTS_VERBOSE)
      message(STATUS "cppcheck disabled")
    endif(ARGUMENTS_VERBOSE)
  endif() # ARGUMENTS_IS_ENABLED
endfunction(cppcheck_enabler)

find_package(basis REQUIRED)
FROM_HERE("invalid basis_HEADER_DIR")
validate(CHECK_NOT_EMPTY ${basis_HEADER_DIR}
  TEXT "${FROM_HERE}"
)
#
set(cling_includes
  ${CONAN_CLING_CONAN_ROOT}/include
)
message(STATUS "cling_includes=${cling_includes}")
set(clang_includes
  ${CONAN_CLING_CONAN_ROOT}/lib/clang/5.0.0/include
)
message(STATUS "clang_includes=${clang_includes}")
set(corrade_includes
  ${CONAN_CORRADE_ROOT}/include
)
message(STATUS "corrade_includes=${corrade_includes}")
set(entt_includes
  ${CONAN_ENTT_ROOT}/include
)
message(STATUS "entt_includes=${entt_includes}")

# cppcheck is not compiler
# and can not define platform-specific defines
# We must do it manually
set(cppcheck_linux_defines
  # Compiler detection
  -D__GNUC__=100
  # Compiler detection
  -DCOMPILER_GCC=1
  -D__WCHAR_MAX__=0x7fffffff
  -D__GNUC_MINOR__=100
  -D__GNUC_PATCHLEVEL__=100
  -D__STDC__=1
  -D__i386__=1
  # Processor architecture detection
  -D__x86_64__=1
  -D__WORDSIZE=64
  # see https://fossies.org/linux/cppcheck/cfg/avr.cfg
  #"-D__CONCATenate(left, right)=left ## right"
  #"-D__CONCAT(left, right)=__CONCATenate(left, right)"
  #"-DUINT32_C(value)=__CONCAT(value, UL)"
  #"-DSB_IS(SB_FEATURE)=((defined SB_IS_##SB_FEATURE) && SB_IS_##SB_FEATURE)"
  -DSB_IS_ARCH_X64=1
  -DSB_IS_ARCH_X86=1
  -DSB_IS_ARCH_ARM=1
  -DSB_IS_32_BIT=1
  -DSB_HAS_1_CORE=1
  #-DSTARBOARD_CONFIGURATION_INCLUDE="starboard/linux/x64x11/configuration_public.h"
  #-DSTARBOARD_ATOMIC_INCLUDE="starboard/linux/x64x11/atomic_public.h"
  #-DSTARBOARD_THREAD_TYPES_INCLUDE="starboard/linux/x64x11/thread_types_public.h"
  -Dlinux=1
  -D__linux__=1
  # chromium/base support
  -DOS_POSIX=1
  # chromium/base support
  -DOS_LINUX=1
  -DULONG_MAX=18446744073709551615U
)

# see https://valgrind.org/docs/manual/QuickStart.html
function(add_valgrind_target)
  # see https://cliutils.gitlab.io/modern-cmake/chapters/basics/functions.html
  set(options
    # empty
  )
  set(oneValueArgs
    TARGET_NAME
  )
  set(multiValueArgs
    MEMORYCHECK_FULL_CMD
    # adds options to default options
    EXTRA_OPTIONS
  )
  #
  cmake_parse_arguments(
    ARGUMENTS # prefix of output variables
    "${options}" # list of names of the boolean arguments (only defined ones will be true)
    "${oneValueArgs}" # list of names of mono-valued arguments
    "${multiValueArgs}" # list of names of multi-valued arguments (output variables are lists)
    ${ARGN} # arguments of the function to parse, here we take the all original ones
  )
  #
  set(args_unparsed ${ARGUMENTS_UNPARSED_ARGUMENTS})

  FROM_HERE("invalid ARGUMENTS_TARGET_NAME")
  validate(CHECK_NOT_EMPTY ${ARGUMENTS_TARGET_NAME}
    TEXT "${FROM_HERE}"
  )
  #
  FROM_HERE("invalid ARGUMENTS_MEMORYCHECK_FULL_CMD")
  validate(CHECK_NOT_EMPTY ${ARGUMENTS_MEMORYCHECK_FULL_CMD}
    TEXT "${FROM_HERE}"
  )
  if(ARGUMENTS_VERBOSE)
    message(STATUS "ARGUMENTS_MEMORYCHECK_FULL_CMD=${ARGUMENTS_MEMORYCHECK_FULL_CMD}")
  endif(ARGUMENTS_VERBOSE)
  #
  if(ARGUMENTS_VERBOSE)
    message(STATUS "ARGUMENTS_HTML_REPORT=${ARGUMENTS_HTML_REPORT}")
  endif(ARGUMENTS_VERBOSE)
  #
  set(MEMORYCHECK_FULL_CMD
    ${ARGUMENTS_MEMORYCHECK_FULL_CMD}
    ${ARGUMENTS_EXTRA_OPTIONS}
  )
  #
  # USAGE:
  # cmake -E time cmake --build . --target TARGET_NAME_run_valgrind
  if(ARGUMENTS_VERBOSE)
    message(STATUS "added new target: ${ARGUMENTS_TARGET_NAME}_run_valgrind")
  endif(ARGUMENTS_VERBOSE)
  add_custom_target(${ARGUMENTS_TARGET_NAME}_run_valgrind
    # remove old report
    COMMAND
      # Remove the file(s).
      # If any of the listed files already do not exist,
      # the command returns a non-zero exit code,
      # but no message is logged.
      # The -f option changes the behavior to return a zero exit code
      # (i.e. success) in such situations instead.
      ${CMAKE_COMMAND}
      -E
      remove
      -f
      ${CMAKE_BINARY_DIR}/${ARGUMENTS_TARGET_NAME}_valgrind_report.xml
    # print command that will be executed
    # NOTE: uses COMMAND_EXPAND_LISTS
    # to support generator expressions
    # see https://cmake.org/cmake/help/v3.13/command/add_custom_target.html
    COMMAND
      "${CMAKE_COMMAND}"
      -E
      echo
      "executing command: ${MEMORYCHECK_FULL_CMD}"
    # Run valgrind static analysis
    COMMAND
      "${MEMORYCHECK_FULL_CMD}"
    VERBATIM
    # NOTE: uses COMMAND_EXPAND_LISTS
    # to support generator expressions
    # see https://cmake.org/cmake/help/v3.13/command/add_custom_target.html
    COMMAND_EXPAND_LISTS
    #USES_TERMINAL
    # Set work directory for target
    WORKING_DIRECTORY
      ${CMAKE_BINARY_DIR}
    # Echo what is being done
    COMMENT "running valgrind"
  )
endfunction(add_valgrind_target)

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_run_valgrind
# EXAMPLE:
# valgrind_enabler(
#  IS_ENABLED
#    ${ENABLE_VALGRIND}
#  STANDALONE_TARGET
#    ${LIB_NAME}
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
#    --log-file=${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}_valgrind_raw.log
#    # executable
#    $<TARGET_FILE:${LIB_NAME}>
#    # arguments of executable
#    --version
#   VERBOSE
# )
# @see https://habr.com/ru/post/210256/
function(valgrind_enabler)
  # see https://cliutils.gitlab.io/modern-cmake/chapters/basics/functions.html
  set(options
    VERBOSE
    REQUIRED
  )
  set(oneValueArgs
    IS_ENABLED
    STANDALONE_TARGET
  )
  set(multiValueArgs
    # completely changes options to provided options
    OVERRIDE_OPTIONS
    # adds options to default options
    EXTRA_OPTIONS
  )
  #
  cmake_parse_arguments(
    ARGUMENTS # prefix of output variables
    "${options}" # list of names of the boolean arguments (only defined ones will be true)
    "${oneValueArgs}" # list of names of mono-valued arguments
    "${multiValueArgs}" # list of names of multi-valued arguments (output variables are lists)
    ${ARGN} # arguments of the function to parse, here we take the all original ones
  )
  #
  set(args_unparsed ${ARGUMENTS_UNPARSED_ARGUMENTS})

  set(MEMORYCHECK_SUPRESSIONS
    ${PROJECT_SOURCE_DIR}/cmake/valgrind.cfg
  )

  # see https://valgrind.org/docs/manual/manual-core.html
  # see https://wiki.wxwidgets.org/Valgrind_Suppression_File_Howto
  # set supressions file only if it exists
  if(EXISTS "${MEMORYCHECK_SUPRESSIONS}")
    set(MEMORYCHECK_DEFAULT_SUPRESSIONS_ARG
      --suppressions=${MEMORYCHECK_SUPRESSIONS}
    )
    message(STATUS
      "found valgrind config file: ${MEMORYCHECK_SUPRESSIONS}"
    )
  else()
    message(WARNING
      "unable to find valgrind config file: ${MEMORYCHECK_SUPRESSIONS}"
    )
  endif()

  #
  # default options
  #
  set(MEMORYCHECK_OPTIONS
    ${MEMORYCHECK_DEFAULT_SUPRESSIONS_ARG}
  )

  if(ARGUMENTS_OVERRIDE_OPTIONS)
    set(MEMORYCHECK_OPTIONS ${ARGUMENTS_OVERRIDE_OPTIONS})
  else()
    if(ARGUMENTS_VERBOSE)
      message(STATUS "valgrind: no OVERRIDE_OPTIONS provided")
    endif(ARGUMENTS_VERBOSE)
    # skip, use defaults
  endif()
  #
  list(APPEND MEMORYCHECK_OPTIONS ${ARGUMENTS_EXTRA_OPTIONS})
  if(ARGUMENTS_VERBOSE)
    message(STATUS "ARGUMENTS_EXTRA_OPTIONS=${ARGUMENTS_EXTRA_OPTIONS}")
  endif(ARGUMENTS_VERBOSE)
  #
  if(${ARGUMENTS_IS_ENABLED})
    message(STATUS "valgrind enabled")

    # to use `valgrind` from conan
    list(APPEND CMAKE_PROGRAM_PATH ${CONAN_BIN_DIRS})

    find_program_helper(valgrind
      #PATHS
      #  ${CONAN_BIN_DIRS}
      #  ${CONAN_BIN_DIRS_LLVM_TOOLS}
      #NO_SYSTEM_ENVIRONMENT_PATH
      #NO_CMAKE_SYSTEM_PATH
      ${ARGUMENTS_UNPARSED_ARGUMENTS}
      REQUIRED
      OUT_VAR CMAKE_MEMORYCHECK_COMMAND
      VERBOSE TRUE
    )

    if(CMAKE_MEMORYCHECK_COMMAND)
      # Set valgrind program + options.
      list(APPEND MEMORYCHECK_RUNNABLE
        ${CMAKE_MEMORYCHECK_COMMAND}
      )
      if(ARGUMENTS_VERBOSE)
        message(STATUS "MEMORYCHECK_RUNNABLE=${MEMORYCHECK_RUNNABLE}")
      endif(ARGUMENTS_VERBOSE)

      # create separate target for valgrind
      if(ARGUMENTS_STANDALONE_TARGET)
        add_valgrind_target(
          TARGET_NAME ${ARGUMENTS_STANDALONE_TARGET}
          MEMORYCHECK_FULL_CMD ${MEMORYCHECK_RUNNABLE}
          EXTRA_OPTIONS ${MEMORYCHECK_OPTIONS}
        )
      endif(ARGUMENTS_STANDALONE_TARGET)

    else(CMAKE_MEMORYCHECK_COMMAND)
      message(WARNING "Program 'valgrind' not found, unable to run 'valgrind'.")
    endif(CMAKE_MEMORYCHECK_COMMAND)
  else() # ARGUMENTS_IS_ENABLED
    if(ARGUMENTS_VERBOSE)
      message(STATUS "valgrind disabled")
    endif(ARGUMENTS_VERBOSE)
  endif() # ARGUMENTS_IS_ENABLED
endfunction(valgrind_enabler)

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_run_cppcheck
cppcheck_enabler(
  PATHS
    # to use cppcheck_installer from conan
    ${CONAN_BIN_DIRS}
    ${CONAN_BIN_DIRS_LLVM_TOOLS}
    # to use `cppcheck-htmlreport` from cmake subfolder
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
  NO_SYSTEM_ENVIRONMENT_PATH
  NO_CMAKE_SYSTEM_PATH
  IS_ENABLED
    ${ENABLE_CPPCHECK}
  CHECK_TARGETS
    ${LIB_NAME}
  EXTRA_OPTIONS
    # check all #ifdef
    #--force
    #-j6 # thread count
    --language=c++
    # inconclusive = more checks
    # NOTE: There are false positives with this option.
    --inconclusive
    --enable=all
    #--enable=warning,performance,portability,information,missingInclude
    # Give path to ignore. Give several -i parameters to ignore several paths.
    -igenerated
    --std=c++20
    # include
    -I${basis_HEADER_DIR}
    # ignore
    -i${basis_HEADER_DIR}
    # include
    #-I${cling_includes}
    # ignore
    #-i${cling_includes}
    # include
    #-I${clang_includes}
    # ignore
    #-i${clang_includes}
    # include
    -I${corrade_includes}
    # ignore
    -i${corrade_includes}
    # corrade support
    -DDOXYGEN_GENERATING_OUTPUT=1
    # include
    -I${entt_includes}
    -DNDEBUG
    --max-configs=100
    ${cppcheck_linux_defines}
    # undef
    -USTARBOARD
    # undef
    -UCOBALT
    # less info in console
    #--quiet
    # missingIncludeSystem:
    # Cppcheck does not need standard library headers
    # to get proper results.
    #--suppress=missingIncludeSystem
    # preprocessorErrorDirective:
    # support for
    # __has_include(<boost/filesystem.hpp>)
    #--suppress=preprocessorErrorDirective
  HTML_REPORT TRUE
  VERBOSE
  REQUIRED
)

function(add_clang_tidy_target)
  # see https://cliutils.gitlab.io/modern-cmake/chapters/basics/functions.html
  set(options
    # empty
  )
  set(oneValueArgs
    TARGET_NAME
  )
  set(multiValueArgs
    CLANG_TIDY_FULL_CMD
    # adds options to default options
    EXTRA_OPTIONS
  )
  #
  cmake_parse_arguments(
    ARGUMENTS # prefix of output variables
    "${options}" # list of names of the boolean arguments (only defined ones will be true)
    "${oneValueArgs}" # list of names of mono-valued arguments
    "${multiValueArgs}" # list of names of multi-valued arguments (output variables are lists)
    ${ARGN} # arguments of the function to parse, here we take the all original ones
  )
  #
  set(args_unparsed ${ARGUMENTS_UNPARSED_ARGUMENTS})

  FROM_HERE("invalid ARGUMENTS_TARGET_NAME")
  validate(CHECK_NOT_EMPTY ${ARGUMENTS_TARGET_NAME}
    TEXT "${FROM_HERE}"
  )

  #
  FROM_HERE("invalid ARGUMENTS_CLANG_TIDY_FULL_CMD")
  validate(CHECK_NOT_EMPTY ${ARGUMENTS_CLANG_TIDY_FULL_CMD}
    TEXT ${FROM_HERE}
  )
  if(ARGUMENTS_VERBOSE)
    message(STATUS "ARGUMENTS_CLANG_TIDY_FULL_CMD=${ARGUMENTS_CLANG_TIDY_FULL_CMD}")
  endif(ARGUMENTS_VERBOSE)
  #
  set(CLANG_TIDY_FULL_CMD
    ${ARGUMENTS_CLANG_TIDY_FULL_CMD}
    ${ARGUMENTS_EXTRA_OPTIONS}
  )
  #
  # USAGE:
  # cmake -E time cmake --build . --target TARGET_NAME_run_clang_tidy
  if(ARGUMENTS_VERBOSE)
    message(STATUS "added new target: ${ARGUMENTS_TARGET_NAME}_run_clang_tidy")
  endif(ARGUMENTS_VERBOSE)
  add_custom_target(${ARGUMENTS_TARGET_NAME}_run_clang_tidy
    # remove old report
    COMMAND
      # Remove the file(s).
      # If any of the listed files already do not exist,
      # the command returns a non-zero exit code,
      # but no message is logged.
      # The -f option changes the behavior to return a zero exit code
      # (i.e. success) in such situations instead.
      ${CMAKE_COMMAND}
      -E
      remove
      -f
      ${CMAKE_BINARY_DIR}/${ARGUMENTS_TARGET_NAME}_clang_tidy_report.xml
    # create report dir
    COMMAND
      ${CMAKE_COMMAND}
      -E
      make_directory
      ${CMAKE_BINARY_DIR}/${ARGUMENTS_TARGET_NAME}_report
    # remove old report
    COMMAND
      # Remove the file(s).
      # If any of the listed files already do not exist,
      # the command returns a non-zero exit code,
      # but no message is logged.
      # The -f option changes the behavior to return a zero exit code
      # (i.e. success) in such situations instead.
      ${CMAKE_COMMAND}
      -E
      remove
      -f
      ${CMAKE_BINARY_DIR}/${ARGUMENTS_TARGET_NAME}_report/index.html
    # print command that will be executed
    # NOTE: uses COMMAND_EXPAND_LISTS
    # to support generator expressions
    # see https://cmake.org/cmake/help/v3.13/command/add_custom_target.html
    COMMAND
      "${CMAKE_COMMAND}"
      -E
      echo
      "executing command: ${CLANG_TIDY_FULL_CMD}"
    # Run clang_tidy static analysis
    COMMAND
      "${CLANG_TIDY_FULL_CMD}"
    # print command that will be executed
    # NOTE: uses COMMAND_EXPAND_LISTS
    # to support generator expressions
    # see https://cmake.org/cmake/help/v3.13/command/add_custom_target.html
    COMMAND
      "${CMAKE_COMMAND}"
      -E
      echo
      "executing command: ${HTMLREPORT_FULL_CMD}"
    # Generate html output
    COMMAND
      ${CMAKE_COMMAND}
      -E
      time
      "${HTMLREPORT_FULL_CMD}"
    VERBATIM
    # NOTE: uses COMMAND_EXPAND_LISTS
    # to support generator expressions
    # see https://cmake.org/cmake/help/v3.13/command/add_custom_target.html
    COMMAND_EXPAND_LISTS
    #USES_TERMINAL
    # Set work directory for target
    WORKING_DIRECTORY
      ${CMAKE_BINARY_DIR}
    # Echo what is being done
    COMMENT "running clang_tidy"
  )
endfunction(add_clang_tidy_target)

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_run_clang_tidy
# EXAMPLE:
# clang_tidy_enabler(
#   PATHS
#     ${CONAN_BIN_DIRS}
#     ${CONAN_BIN_DIRS_LLVM_TOOLS}
#   NO_SYSTEM_ENVIRONMENT_PATH
#   NO_CMAKE_SYSTEM_PATH
#   IS_ENABLED
#     ${ENABLE_CLANG_TIDY}
#   CHECK_TARGETS
#     ${LIB_NAME}
#   EXTRA_OPTIONS
#    # see clang-tidy --list-checks -checks='*' | grep "modernize"
#    -checks=*
#    -extra-arg=-std=c++17
#    -extra-arg=-Qunused-arguments
#    -extra-arg=-DBOOST_SYSTEM_NO_DEPRECATED
#    -extra-arg=-DBOOST_ERROR_CODE_HEADER_ONLY
#    -header-filter=${CMAKE_CURRENT_SOURCE_DIR}
#    -warnings-as-errors=cppcoreguidelines-avoid-goto
#   VERBOSE
# )
function(clang_tidy_enabler)
  # see https://cliutils.gitlab.io/modern-cmake/chapters/basics/functions.html
  set(options
    VERBOSE
    REQUIRED
  )
  set(oneValueArgs
    IS_ENABLED
    STANDALONE_TARGET
  )
  set(multiValueArgs
    # completely changes options to provided options
    OVERRIDE_OPTIONS
    # adds options to default options
    EXTRA_OPTIONS
    # Will collect include dirs, defines and source files
    # from cmake targets
    # NOTE: Why not compile_commands.json?
    # - cmake can not generate that file on per-target basis
    # or into custom out dir
    # NOTE: Why not CMAKE_CXX_CLANG_TIDY?
    # - we want to run custom clang_tidy target without
    # need to build whole poject per each check
    CHECK_TARGETS
  )
  #
  cmake_parse_arguments(
    ARGUMENTS # prefix of output variables
    "${options}" # list of names of the boolean arguments (only defined ones will be true)
    "${oneValueArgs}" # list of names of mono-valued arguments
    "${multiValueArgs}" # list of names of multi-valued arguments (output variables are lists)
    ${ARGN} # arguments of the function to parse, here we take the all original ones
  )
  #
  set(args_unparsed ${ARGUMENTS_UNPARSED_ARGUMENTS})
  #
  # default options
  #
  # TODO
  #set(CLANG_TIDY_SUPRESSIONS
  #  ${PROJECT_SOURCE_DIR}/cmake/clang_tidy.cfg
  #)
  ## set supressions file only if it exists
  #if(EXISTS "${CLANG_TIDY_SUPRESSIONS}")
  #  set(CLANG_TIDY_DEFAULT_SUPRESSIONS_ARG
  #    --suppressions-list=${CLANG_TIDY_SUPRESSIONS}
  #  )
  #  message(STATUS
  #    "found clang_tidy config file: ${CLANG_TIDY_SUPRESSIONS}"
  #  )
  #else()
  #  message(WARNING
  #    "unable to find clang_tidy config file: ${CLANG_TIDY_SUPRESSIONS}"
  #  )
  #endif()
  # Set message template
  # TODO
  #set(CLANG_TIDY_TEMPLATE
  #  "[{file}:{line}] ({severity}) {message} ({id}) ({callstack})"
  #)
  # Set clang_tidy cache directory
  set(CLANG_TIDY_BUILD_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/clang_tidy_cache
  )
  #
  set(CLANG_TIDY_OPTIONS
    # more info in console
    #--verbose
  )
  if(ARGUMENTS_OVERRIDE_OPTIONS)
    set(CLANG_TIDY_OPTIONS ${ARGUMENTS_OVERRIDE_OPTIONS})
  else()
    if(ARGUMENTS_VERBOSE)
      message(STATUS "clang_tidy: no OVERRIDE_OPTIONS provided")
    endif(ARGUMENTS_VERBOSE)
    # skip, use defaults
  endif()
  #
  list(APPEND CLANG_TIDY_OPTIONS ${ARGUMENTS_EXTRA_OPTIONS})
  if(ARGUMENTS_VERBOSE)
    message(STATUS "ARGUMENTS_EXTRA_OPTIONS=${ARGUMENTS_EXTRA_OPTIONS}")
  endif(ARGUMENTS_VERBOSE)
  #
  if(${ARGUMENTS_IS_ENABLED})
    message(STATUS "clang-tidy enabled")

    # to use `clang_tidy` from conan
    list(APPEND CMAKE_PROGRAM_PATH ${CONAN_BIN_DIRS})

    find_program_helper(clang-tidy
      #PATHS
      #  ${CONAN_BIN_DIRS}
      #  ${CONAN_BIN_DIRS_LLVM_TOOLS}
      #NO_SYSTEM_ENVIRONMENT_PATH
      #NO_CMAKE_SYSTEM_PATH
      ${ARGUMENTS_UNPARSED_ARGUMENTS}
      REQUIRED
      OUT_VAR CLANG_TIDY_PROGRAM
      VERBOSE TRUE
    )

    if(CLANG_TIDY_PROGRAM)
      # Create clang_tidy cache directory
      file(MAKE_DIRECTORY ${CLANG_TIDY_BUILD_DIR})

      # Set clang_tidy program + options.
      list(APPEND CLANG_TIDY_RUNNABLE
        ${CLANG_TIDY_PROGRAM}
      )
      if(ARGUMENTS_VERBOSE)
        message(STATUS "CLANG_TIDY_RUNNABLE=${CLANG_TIDY_RUNNABLE}")
      endif(ARGUMENTS_VERBOSE)

      # create separate target for clang_tidy
      if(ARGUMENTS_STANDALONE_TARGET)
        add_clang_tidy_target(
          TARGET_NAME ${ARGUMENTS_STANDALONE_TARGET}
          CLANG_TIDY_FULL_CMD
            ${CLANG_TIDY_RUNNABLE}
          EXTRA_OPTIONS ${CLANG_TIDY_OPTIONS}
        )
      endif(ARGUMENTS_STANDALONE_TARGET)

      # collect headers and defines from existing target
      if(ARGUMENTS_CHECK_TARGETS)
        if(ARGUMENTS_VERBOSE)
          message(STATUS "clang_tidy: ARGUMENTS_CHECK_TARGETS=${ARGUMENTS_CHECK_TARGETS}")
        endif(ARGUMENTS_VERBOSE)
        foreach(TARGET_NAME ${ARGUMENTS_CHECK_TARGETS})
          if(ARGUMENTS_VERBOSE)
            message(STATUS "enabled clang_tidy for target ${TARGET_NAME}")
          endif(ARGUMENTS_VERBOSE)
          #
          get_all_compile_definitions(collected_defines
            ${TARGET_NAME}
          )
          #
          get_all_include_directories(collected_includes
            ${TARGET_NAME}
          )
          #
          get_target_sources(TARGET_SOURCES
            ${TARGET_NAME}
          )
          #
          add_clang_tidy_target(
            TARGET_NAME ${LIB_NAME}
            CLANG_TIDY_FULL_CMD
              ${CLANG_TIDY_RUNNABLE}
              # NOTE: generator expression, expands during build time
              # if the ${ITEM} is non-empty, than append it
              $<$<BOOL:${collected_defines}>:-extra-arg=-D$<JOIN:${collected_defines}, -extra-arg=-D>>
              # NOTE: generator expression, expands during build time
              # if the ${ITEM} is non-empty, than append it
              # To suppress compiler diagnostic messages
              # from third-party headers just use -isystem
              # instead of -I to include those headers.
              $<$<BOOL:${collected_includes}>:-extra-arg=-isystem$<JOIN:${collected_includes}, -extra-arg=-isystem>>
              ${TARGET_SOURCES}
            EXTRA_OPTIONS ${CLANG_TIDY_OPTIONS}
          )
          # run valgrind on each build of target
          add_dependencies(
            ${LIB_NAME}
            ${LIB_NAME}_run_clang_tidy
          )
        endforeach()
      else(ARGUMENTS_CHECK_TARGETS)
        if(ARGUMENTS_VERBOSE)
          message(STATUS "clang_tidy: no CHECK_TARGETS provided")
        endif(ARGUMENTS_VERBOSE)
      endif(ARGUMENTS_CHECK_TARGETS)
    else(CLANG_TIDY_PROGRAM)
      message(WARNING "Program 'clang_tidy' not found, unable to run 'clang_tidy'.")
    endif(CLANG_TIDY_PROGRAM)
  else() # ARGUMENTS_IS_ENABLED
    if(ARGUMENTS_VERBOSE)
      message(STATUS "clang_tidy disabled")
    endif(ARGUMENTS_VERBOSE)
  endif() # ARGUMENTS_IS_ENABLED
endfunction(clang_tidy_enabler)

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_clang_tidy
clang_tidy_enabler(
  PATHS
    ${CONAN_BIN_DIRS}
    ${CONAN_BIN_DIRS_LLVM_TOOLS}
  NO_SYSTEM_ENVIRONMENT_PATH
  NO_CMAKE_SYSTEM_PATH
  IS_ENABLED
    ${ENABLE_CLANG_TIDY}
  CHECK_TARGETS
    ${LIB_NAME}
  EXTRA_OPTIONS
    # see clang-tidy --list-checks -checks='*' | grep "modernize"
    #-checks="*,-modernize*,-*readability*,-hicpp-braces*,-cppcoreguidelines*,-misc-non-private-member-variables-in-classes,-hicpp-no-malloc,-*uppercase-literal-suffix*,-llvm-include-order,-bugprone-narrowing-conversions,-performance-type-promotion-in-math-fn,-hicpp-signed-bitwise"
    -checks=*
    #-config="{CheckOptions: [ {key: readability-identifier-naming.ClassCase, value: CamelCase} ]}"
    -extra-arg=-std=c++17
    -extra-arg=-Qunused-arguments
    # To suppress compiler diagnostic messages
    # from third-party headers just use -isystem
    # instead of -I to include those headers.
    #-extra-arg=-nostdinc
    #-extra-arg=-nostdinc++
    -extra-arg=-DBOOST_SYSTEM_NO_DEPRECATED
    -extra-arg=-DBOOST_ERROR_CODE_HEADER_ONLY
    #List of files with line ranges to filter the
    #    warnings. Can be used together with
    #    -header-filter. The format of the list is a JSON
    #    array of objects:
    #      [
    #        {"name":"file1.cpp","lines":[[1,3],[5,7]]},
    #        {"name":"file2.h"}
    #      ]
    #-line-filter=\"[\
    #  {\"name\":\"path/to/file.cpp\"},\
    #  {\"name\":\"path/to/file.h\"}\
    #  ]\"
    # -header-filter is a whitelist not a blacklist.
    #-header-filter="^((?!/usr/|thirdparty|third_party|/cocos2d-x/external/|/cocos/scripting/).)*$"
    #-header-filter=flextool/*
    -header-filter=${CMAKE_CURRENT_SOURCE_DIR}
    -warnings-as-errors=cppcoreguidelines-avoid-goto
  VERBOSE
  REQUIRED
)

## ---------------------------- cppclean -------------------------------- ##

function(add_cppclean_target)
  # see https://cliutils.gitlab.io/modern-cmake/chapters/basics/functions.html
  set(options
    # empty
  )
  set(oneValueArgs
    TARGET_NAME
  )
  set(multiValueArgs
    CPPCLEAN_FULL_CMD
    # adds options to default options
    EXTRA_OPTIONS
  )
  #
  cmake_parse_arguments(
    ARGUMENTS # prefix of output variables
    "${options}" # list of names of the boolean arguments (only defined ones will be true)
    "${oneValueArgs}" # list of names of mono-valued arguments
    "${multiValueArgs}" # list of names of multi-valued arguments (output variables are lists)
    ${ARGN} # arguments of the function to parse, here we take the all original ones
  )
  #
  set(args_unparsed ${ARGUMENTS_UNPARSED_ARGUMENTS})

  FROM_HERE("invalid ARGUMENTS_TARGET_NAME")
  validate(CHECK_NOT_EMPTY ${ARGUMENTS_TARGET_NAME}
    TEXT "${FROM_HERE}"
  )

  #
  FROM_HERE("invalid ARGUMENTS_CPPCLEAN_FULL_CMD")
  validate(CHECK_NOT_EMPTY ${ARGUMENTS_CPPCLEAN_FULL_CMD}
    TEXT ${FROM_HERE}
  )
  if(ARGUMENTS_VERBOSE)
    message(STATUS "ARGUMENTS_CPPCLEAN_FULL_CMD=${ARGUMENTS_CPPCLEAN_FULL_CMD}")
  endif(ARGUMENTS_VERBOSE)
  #
  set(CPPCLEAN_FULL_CMD
    ${ARGUMENTS_CPPCLEAN_FULL_CMD}
    ${ARGUMENTS_EXTRA_OPTIONS}
  )
  #
  # USAGE:
  # cmake -E time cmake --build . --target TARGET_NAME_run_cppclean
  if(ARGUMENTS_VERBOSE)
    message(STATUS "added new target: ${ARGUMENTS_TARGET_NAME}_run_cppclean")
  endif(ARGUMENTS_VERBOSE)
  add_custom_target(${ARGUMENTS_TARGET_NAME}_run_cppclean
    # remove old report
    COMMAND
      # Remove the file(s).
      # If any of the listed files already do not exist,
      # the command returns a non-zero exit code,
      # but no message is logged.
      # The -f option changes the behavior to return a zero exit code
      # (i.e. success) in such situations instead.
      ${CMAKE_COMMAND}
      -E
      remove
      -f
      ${CMAKE_BINARY_DIR}/${ARGUMENTS_TARGET_NAME}_cppclean_report.xml
    # create report dir
    COMMAND
      ${CMAKE_COMMAND}
      -E
      make_directory
      ${CMAKE_BINARY_DIR}/${ARGUMENTS_TARGET_NAME}_report
    # remove old report
    COMMAND
      # Remove the file(s).
      # If any of the listed files already do not exist,
      # the command returns a non-zero exit code,
      # but no message is logged.
      # The -f option changes the behavior to return a zero exit code
      # (i.e. success) in such situations instead.
      ${CMAKE_COMMAND}
      -E
      remove
      -f
      ${CMAKE_BINARY_DIR}/${ARGUMENTS_TARGET_NAME}_report/index.html
    # print command that will be executed
    # NOTE: uses COMMAND_EXPAND_LISTS
    # to support generator expressions
    # see https://cmake.org/cmake/help/v3.13/command/add_custom_target.html
    COMMAND
      "${CMAKE_COMMAND}"
      -E
      echo
      "executing command: ${CPPCLEAN_FULL_CMD}"
    # Run cppclean static analysis
    COMMAND
      "${CPPCLEAN_FULL_CMD}"
    # print command that will be executed
    # NOTE: uses COMMAND_EXPAND_LISTS
    # to support generator expressions
    # see https://cmake.org/cmake/help/v3.13/command/add_custom_target.html
    COMMAND
      "${CMAKE_COMMAND}"
      -E
      echo
      "executing command: ${HTMLREPORT_FULL_CMD}"
    # Generate html output
    COMMAND
      ${CMAKE_COMMAND}
      -E
      time
      "${HTMLREPORT_FULL_CMD}"
    VERBATIM
    # NOTE: uses COMMAND_EXPAND_LISTS
    # to support generator expressions
    # see https://cmake.org/cmake/help/v3.13/command/add_custom_target.html
    COMMAND_EXPAND_LISTS
    #USES_TERMINAL
    # Set work directory for target
    WORKING_DIRECTORY
      ${CMAKE_BINARY_DIR}
    # Echo what is being done
    COMMENT "running cppclean"
  )
endfunction(add_cppclean_target)

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_run_cppclean
# EXAMPLE:
# cppclean_enabler(
#   PATHS
#     ${CONAN_BIN_DIRS}
#     ${CONAN_BIN_DIRS_LLVM_TOOLS}
#   NO_SYSTEM_ENVIRONMENT_PATH
#   NO_CMAKE_SYSTEM_PATH
#   IS_ENABLED
#     ${ENABLE_CPPCLEAN}
#   CHECK_TARGETS
#     ${LIB_NAME}
#   EXTRA_OPTIONS
#    --include-path=allergies
#    allergies/allergies.h
#    allergies/allergies.cpp
#   VERBOSE
# )
function(cppclean_enabler)
  # see https://cliutils.gitlab.io/modern-cmake/chapters/basics/functions.html
  set(options
    VERBOSE
    REQUIRED
  )
  set(oneValueArgs
    IS_ENABLED
    STANDALONE_TARGET
  )
  set(multiValueArgs
    # completely changes options to provided options
    OVERRIDE_OPTIONS
    # adds options to default options
    EXTRA_OPTIONS
    # Will collect include dirs, defines and source files
    # from cmake targets
    # NOTE: Why not compile_commands.json?
    # - cmake can not generate that file on per-target basis
    # or into custom out dir
    # NOTE: Why not CMAKE_CXX_CPPCLEAN?
    # - we want to run custom cppclean target without
    # need to build whole poject per each check
    CHECK_TARGETS
  )
  #
  cmake_parse_arguments(
    ARGUMENTS # prefix of output variables
    "${options}" # list of names of the boolean arguments (only defined ones will be true)
    "${oneValueArgs}" # list of names of mono-valued arguments
    "${multiValueArgs}" # list of names of multi-valued arguments (output variables are lists)
    ${ARGN} # arguments of the function to parse, here we take the all original ones
  )
  #
  set(args_unparsed ${ARGUMENTS_UNPARSED_ARGUMENTS})
  #
  # default options
  #
  # TODO
  #set(CPPCLEAN_SUPRESSIONS
  #  ${PROJECT_SOURCE_DIR}/cmake/cppclean.cfg
  #)
  ## set supressions file only if it exists
  #if(EXISTS "${CPPCLEAN_SUPRESSIONS}")
  #  set(CPPCLEAN_DEFAULT_SUPRESSIONS_ARG
  #    --suppressions-list=${CPPCLEAN_SUPRESSIONS}
  #  )
  #  message(STATUS
  #    "found cppclean config file: ${CPPCLEAN_SUPRESSIONS}"
  #  )
  #else()
  #  message(WARNING
  #    "unable to find cppclean config file: ${CPPCLEAN_SUPRESSIONS}"
  #  )
  #endif()
  # Set message template
  # TODO
  #set(CPPCLEAN_TEMPLATE
  #  "[{file}:{line}] ({severity}) {message} ({id}) ({callstack})"
  #)
  # Set cppclean cache directory
  set(CPPCLEAN_BUILD_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/cppclean_cache
  )
  #
  set(CPPCLEAN_OPTIONS
    # more info in console
    #--verbose
  )
  if(ARGUMENTS_OVERRIDE_OPTIONS)
    set(CPPCLEAN_OPTIONS ${ARGUMENTS_OVERRIDE_OPTIONS})
  else()
    if(ARGUMENTS_VERBOSE)
      message(STATUS "cppclean: no OVERRIDE_OPTIONS provided")
    endif(ARGUMENTS_VERBOSE)
    # skip, use defaults
  endif()
  #
  list(APPEND CPPCLEAN_OPTIONS ${ARGUMENTS_EXTRA_OPTIONS})
  if(ARGUMENTS_VERBOSE)
    message(STATUS "ARGUMENTS_EXTRA_OPTIONS=${ARGUMENTS_EXTRA_OPTIONS}")
  endif(ARGUMENTS_VERBOSE)
  #
  if(${ARGUMENTS_IS_ENABLED})
    message(STATUS "cppclean enabled")

    # to use `cppclean` from conan
    list(APPEND CMAKE_PROGRAM_PATH ${CONAN_BIN_DIRS})

    find_program_helper(cppclean
      #PATHS
      #  ${CONAN_BIN_DIRS}
      #  ${CONAN_BIN_DIRS_LLVM_TOOLS}
      #NO_SYSTEM_ENVIRONMENT_PATH
      #NO_CMAKE_SYSTEM_PATH
      ${ARGUMENTS_UNPARSED_ARGUMENTS}
      REQUIRED
      OUT_VAR CPPCLEAN_PROGRAM
      VERBOSE TRUE
    )

    if(CPPCLEAN_PROGRAM)
      # Create cppclean cache directory
      file(MAKE_DIRECTORY ${CPPCLEAN_BUILD_DIR})

      # Set cppclean program + options.
      list(APPEND CPPCLEAN_RUNNABLE
        ${CPPCLEAN_PROGRAM}
      )
      if(ARGUMENTS_VERBOSE)
        message(STATUS "CPPCLEAN_RUNNABLE=${CPPCLEAN_RUNNABLE}")
      endif(ARGUMENTS_VERBOSE)

      # create separate target for cppclean
      if(ARGUMENTS_STANDALONE_TARGET)
        add_cppclean_target(
          TARGET_NAME ${ARGUMENTS_STANDALONE_TARGET}
          CPPCLEAN_FULL_CMD
            ${CPPCLEAN_RUNNABLE}
          EXTRA_OPTIONS ${CPPCLEAN_OPTIONS}
        )
      endif(ARGUMENTS_STANDALONE_TARGET)

      # collect headers and defines from existing target
      if(ARGUMENTS_CHECK_TARGETS)
        if(ARGUMENTS_VERBOSE)
          message(STATUS "cppclean: ARGUMENTS_CHECK_TARGETS=${ARGUMENTS_CHECK_TARGETS}")
        endif(ARGUMENTS_VERBOSE)
        foreach(TARGET_NAME ${ARGUMENTS_CHECK_TARGETS})
          if(ARGUMENTS_VERBOSE)
            message(STATUS "enabled cppclean for target ${TARGET_NAME}")
          endif(ARGUMENTS_VERBOSE)
          #
          get_all_compile_definitions(collected_defines
            ${TARGET_NAME}
          )
          #
          get_all_include_directories(collected_includes
            ${TARGET_NAME}
          )
          #
          get_target_sources(TARGET_SOURCES
            ${TARGET_NAME}
          )
          #
          add_cppclean_target(
            TARGET_NAME ${LIB_NAME}
            CPPCLEAN_FULL_CMD
              ${CPPCLEAN_RUNNABLE}
              # NOTE: generator expression, expands during build time
              # if the ${ITEM} is non-empty, than append it
              #$<$<BOOL:${collected_defines}>:-D$<JOIN:${collected_defines}, -D>>
              # NOTE: generator expression, expands during build time
              # if the ${ITEM} is non-empty, than append it
              # To suppress compiler diagnostic messages
              # from third-party headers just use -isystem
              # instead of -I to include those headers.
              #$<$<BOOL:${collected_includes}>:--include-path-system $<JOIN:${collected_includes}, --include-path-system >>
              $<$<BOOL:${collected_includes}>:--include-path $<JOIN:${collected_includes}, --include-path >>
              ${TARGET_SOURCES}
            EXTRA_OPTIONS ${CPPCLEAN_OPTIONS}
          )
          # run valgrind on each build of target
          add_dependencies(
            ${LIB_NAME}
            ${LIB_NAME}_run_cppclean
          )
        endforeach()
      else(ARGUMENTS_CHECK_TARGETS)
        if(ARGUMENTS_VERBOSE)
          message(STATUS "cppclean: no CHECK_TARGETS provided")
        endif(ARGUMENTS_VERBOSE)
      endif(ARGUMENTS_CHECK_TARGETS)
    else(CPPCLEAN_PROGRAM)
      message(WARNING "Program 'cppclean' not found, unable to run 'cppclean'.")
    endif(CPPCLEAN_PROGRAM)
  else() # ARGUMENTS_IS_ENABLED
    if(ARGUMENTS_VERBOSE)
      message(STATUS "cppclean disabled")
    endif(ARGUMENTS_VERBOSE)
  endif() # ARGUMENTS_IS_ENABLED
endfunction(cppclean_enabler)

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_cppclean
cppclean_enabler(
  PATHS
    ${CONAN_BIN_DIRS}
    ${CONAN_BIN_DIRS_LLVM_TOOLS}
  #NO_SYSTEM_ENVIRONMENT_PATH
  #NO_CMAKE_SYSTEM_PATH
  IS_ENABLED
    ${ENABLE_CPPCLEAN}
  CHECK_TARGETS
    ${LIB_NAME}
  EXTRA_OPTIONS
    --include-path ${CMAKE_CURRENT_SOURCE_DIR}/include
    #--include-path-non-system ${CMAKE_CURRENT_SOURCE_DIR}/include
    --verbose
    --exclude "*generated*"
    #-extra-arg=-std=c++17
    #-extra-arg=-Qunused-arguments
    #-extra-arg=-DBOOST_SYSTEM_NO_DEPRECATED
    #-extra-arg=-DBOOST_ERROR_CODE_HEADER_ONLY
  VERBOSE
  REQUIRED
)

## ---------------------------- oclint -------------------------------- ##


function(add_oclint_target)
  # see https://cliutils.gitlab.io/modern-cmake/chapters/basics/functions.html
  set(options
    # empty
  )
  set(oneValueArgs
    TARGET_NAME
  )
  set(multiValueArgs
    OCLINT_FULL_CMD
    # adds options to default options
    EXTRA_OPTIONS
  )
  #
  cmake_parse_arguments(
    ARGUMENTS # prefix of output variables
    "${options}" # list of names of the boolean arguments (only defined ones will be true)
    "${oneValueArgs}" # list of names of mono-valued arguments
    "${multiValueArgs}" # list of names of multi-valued arguments (output variables are lists)
    ${ARGN} # arguments of the function to parse, here we take the all original ones
  )
  #
  set(args_unparsed ${ARGUMENTS_UNPARSED_ARGUMENTS})

  FROM_HERE("invalid ARGUMENTS_TARGET_NAME")
  validate(CHECK_NOT_EMPTY ${ARGUMENTS_TARGET_NAME}
    TEXT "${FROM_HERE}"
  )

  #
  FROM_HERE("invalid ARGUMENTS_OCLINT_FULL_CMD")
  validate(CHECK_NOT_EMPTY ${ARGUMENTS_OCLINT_FULL_CMD}
    TEXT ${FROM_HERE}
  )
  if(ARGUMENTS_VERBOSE)
    message(STATUS "ARGUMENTS_OCLINT_FULL_CMD=${ARGUMENTS_OCLINT_FULL_CMD}")
  endif(ARGUMENTS_VERBOSE)
  #
  set(OCLINT_FULL_CMD
    ${ARGUMENTS_OCLINT_FULL_CMD}
    ${ARGUMENTS_EXTRA_OPTIONS}
  )
  #
  # USAGE:
  # cmake -E time cmake --build . --target TARGET_NAME_run_oclint
  if(ARGUMENTS_VERBOSE)
    message(STATUS "added new target: ${ARGUMENTS_TARGET_NAME}_run_oclint")
  endif(ARGUMENTS_VERBOSE)
  add_custom_target(${ARGUMENTS_TARGET_NAME}_run_oclint
    # remove old report
    COMMAND
      # Remove the file(s).
      # If any of the listed files already do not exist,
      # the command returns a non-zero exit code,
      # but no message is logged.
      # The -f option changes the behavior to return a zero exit code
      # (i.e. success) in such situations instead.
      ${CMAKE_COMMAND}
      -E
      remove
      -f
      ${CMAKE_BINARY_DIR}/${ARGUMENTS_TARGET_NAME}_oclint_report.xml
    # create report dir
    COMMAND
      ${CMAKE_COMMAND}
      -E
      make_directory
      ${CMAKE_BINARY_DIR}/${ARGUMENTS_TARGET_NAME}_report
    # remove old report
    COMMAND
      # Remove the file(s).
      # If any of the listed files already do not exist,
      # the command returns a non-zero exit code,
      # but no message is logged.
      # The -f option changes the behavior to return a zero exit code
      # (i.e. success) in such situations instead.
      ${CMAKE_COMMAND}
      -E
      remove
      -f
      ${CMAKE_BINARY_DIR}/${ARGUMENTS_TARGET_NAME}_report/index.html
    # print command that will be executed
    # NOTE: uses COMMAND_EXPAND_LISTS
    # to support generator expressions
    # see https://cmake.org/cmake/help/v3.13/command/add_custom_target.html
    COMMAND
      "${CMAKE_COMMAND}"
      -E
      echo
      "executing command: ${OCLINT_FULL_CMD}"
    # Run oclint static analysis
    COMMAND
      "${OCLINT_FULL_CMD}"
    # print command that will be executed
    # NOTE: uses COMMAND_EXPAND_LISTS
    # to support generator expressions
    # see https://cmake.org/cmake/help/v3.13/command/add_custom_target.html
    COMMAND
      "${CMAKE_COMMAND}"
      -E
      echo
      "executing command: ${HTMLREPORT_FULL_CMD}"
    # Generate html output
    COMMAND
      ${CMAKE_COMMAND}
      -E
      time
      "${HTMLREPORT_FULL_CMD}"
    VERBATIM
    # NOTE: uses COMMAND_EXPAND_LISTS
    # to support generator expressions
    # see https://cmake.org/cmake/help/v3.13/command/add_custom_target.html
    COMMAND_EXPAND_LISTS
    #USES_TERMINAL
    # Set work directory for target
    WORKING_DIRECTORY
      ${CMAKE_BINARY_DIR}
    # Echo what is being done
    COMMENT "running oclint"
  )
endfunction(add_oclint_target)

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_run_oclint
# EXAMPLE:
# oclint_enabler(
#   PATHS
#     ${CONAN_BIN_DIRS}
#     ${CONAN_BIN_DIRS_LLVM_TOOLS}
#   NO_SYSTEM_ENVIRONMENT_PATH
#   NO_CMAKE_SYSTEM_PATH
#   IS_ENABLED
#     ${ENABLE_OCLINT}
#   CHECK_TARGETS
#     ${LIB_NAME}
#   EXTRA_OPTIONS
#     # OCLINT command-line manual
#     # https://oclint-docs.readthedocs.io/en/stable/manual/oclint.html
#     -extra-arg=-std=c++17
#     -extra-arg=-Qunused-arguments
#     # To suppress compiler diagnostic messages
#     # from third-party headers just use -isystem
#     # instead of -I to include those headers.
#     #-extra-arg=-nostdinc
#     #-extra-arg=-nostdinc++
#     -extra-arg=-DBOOST_SYSTEM_NO_DEPRECATED
#     -extra-arg=-DBOOST_ERROR_CODE_HEADER_ONLY
#     # Enable Clang Static Analyzer,
#     # and integrate results into OCLint report
#     -enable-clang-static-analyzer
#     # Compile every source, and analyze across global contexts
#     # (depends on number of source files,
#     # could results in high memory load)
#     # -enable-global-analysis
#     # Write output to <path>
#     -o=${CMAKE_CURRENT_BINARY_DIR}
#     # Build path is used to read a compile command database.
#     -p=${CMAKE_CURRENT_BINARY_DIR}
#     # Add directory to rule loading path
#     -R=${CMAKE_CURRENT_SOURCE_DIR}
#     # Disable the anonymous analytics
#     -no-analytics
#     #-rc=<parameter>=<value>       - Override the default behavior of rules
#     #-report-type=<name>           - Change output report type
#     #-rule=<rule name>             - Explicitly pick rules
# )
function(oclint_enabler)
  # see https://cliutils.gitlab.io/modern-cmake/chapters/basics/functions.html
  set(options
    VERBOSE
    REQUIRED
  )
  set(oneValueArgs
    IS_ENABLED
    STANDALONE_TARGET
  )
  set(multiValueArgs
    # completely changes options to provided options
    OVERRIDE_OPTIONS
    # adds options to default options
    EXTRA_OPTIONS
    # Will collect include dirs, defines and source files
    # from cmake targets
    # NOTE: Why not compile_commands.json?
    # - cmake can not generate that file on per-target basis
    # or into custom out dir
    # NOTE: Why not CMAKE_CXX_OCLINT?
    # - we want to run custom oclint target without
    # need to build whole poject per each check
    CHECK_TARGETS
  )
  #
  cmake_parse_arguments(
    ARGUMENTS # prefix of output variables
    "${options}" # list of names of the boolean arguments (only defined ones will be true)
    "${oneValueArgs}" # list of names of mono-valued arguments
    "${multiValueArgs}" # list of names of multi-valued arguments (output variables are lists)
    ${ARGN} # arguments of the function to parse, here we take the all original ones
  )
  #
  set(args_unparsed ${ARGUMENTS_UNPARSED_ARGUMENTS})
  #
  # default options
  #
  # TODO
  #set(OCLINT_SUPRESSIONS
  #  ${PROJECT_SOURCE_DIR}/cmake/oclint.cfg
  #)
  ## set supressions file only if it exists
  #if(EXISTS "${OCLINT_SUPRESSIONS}")
  #  set(OCLINT_DEFAULT_SUPRESSIONS_ARG
  #    --suppressions-list=${OCLINT_SUPRESSIONS}
  #  )
  #  message(STATUS
  #    "found oclint config file: ${OCLINT_SUPRESSIONS}"
  #  )
  #else()
  #  message(WARNING
  #    "unable to find oclint config file: ${OCLINT_SUPRESSIONS}"
  #  )
  #endif()
  # Set message template
  # TODO
  #set(OcLINT_TEMPLATE
  #  "[{file}:{line}] ({severity}) {message} ({id}) ({callstack})"
  #)
  # Set oclint cache directory
  set(OCLINT_BUILD_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/oclint_cache
  )
  #
  set(OCLINT_OPTIONS
    # more info in console
    #--verbose
  )
  if(ARGUMENTS_OVERRIDE_OPTIONS)
    set(OCLINT_OPTIONS ${ARGUMENTS_OVERRIDE_OPTIONS})
  else()
    if(ARGUMENTS_VERBOSE)
      message(STATUS "oclint: no OVERRIDE_OPTIONS provided")
    endif(ARGUMENTS_VERBOSE)
    # skip, use defaults
  endif()
  #
  list(APPEND OCLINT_OPTIONS ${ARGUMENTS_EXTRA_OPTIONS})
  if(ARGUMENTS_VERBOSE)
    message(STATUS "ARGUMENTS_EXTRA_OPTIONS=${ARGUMENTS_EXTRA_OPTIONS}")
  endif(ARGUMENTS_VERBOSE)
  #
  if(${ARGUMENTS_IS_ENABLED})
    message(STATUS "oclint enabled")

    # to use `oclint` from conan
    list(APPEND CMAKE_PROGRAM_PATH ${CONAN_BIN_DIRS})

    find_program_helper(oclint
      #PATHS
      #  ${CONAN_BIN_DIRS}
      #  ${CONAN_BIN_DIRS_LLVM_TOOLS}
      #NO_SYSTEM_ENVIRONMENT_PATH
      #NO_CMAKE_SYSTEM_PATH
      ${ARGUMENTS_UNPARSED_ARGUMENTS}
      REQUIRED
      OUT_VAR OCLINT_PROGRAM
      VERBOSE TRUE
    )

    if(OCLINT_PROGRAM)
      # Create oclint cache directory
      file(MAKE_DIRECTORY ${OCLINT_BUILD_DIR})

      # Set oclint program + options.
      list(APPEND OCLINT_RUNNABLE
        ${OCLINT_PROGRAM}
      )
      if(ARGUMENTS_VERBOSE)
        message(STATUS "OCLINT_RUNNABLE=${OCLINT_RUNNABLE}")
      endif(ARGUMENTS_VERBOSE)

      # create separate target for oclint
      if(ARGUMENTS_STANDALONE_TARGET)
        add_oclint_target(
          TARGET_NAME ${ARGUMENTS_STANDALONE_TARGET}
          OCLINT_FULL_CMD
            ${OCLINT_RUNNABLE}
          EXTRA_OPTIONS ${OCLINT_OPTIONS}
        )
      endif(ARGUMENTS_STANDALONE_TARGET)

      # collect headers and defines from existing target
      if(ARGUMENTS_CHECK_TARGETS)
        if(ARGUMENTS_VERBOSE)
          message(STATUS "oclint: ARGUMENTS_CHECK_TARGETS=${ARGUMENTS_CHECK_TARGETS}")
        endif(ARGUMENTS_VERBOSE)
        foreach(TARGET_NAME ${ARGUMENTS_CHECK_TARGETS})
          if(ARGUMENTS_VERBOSE)
            message(STATUS "enabled oclint for target ${TARGET_NAME}")
          endif(ARGUMENTS_VERBOSE)
          #
          get_all_compile_definitions(collected_defines
            ${TARGET_NAME}
          )
          #
          get_all_include_directories(collected_includes
            ${TARGET_NAME}
          )
          #
          get_target_sources(TARGET_SOURCES
            ${TARGET_NAME}
          )
          #
          add_oclint_target(
            TARGET_NAME ${LIB_NAME}
            OCLINT_FULL_CMD
              ${OCLINT_RUNNABLE}
              # NOTE: generator expression, expands during build time
              # if the ${ITEM} is non-empty, than append it
              $<$<BOOL:${collected_defines}>:-extra-arg=-D$<JOIN:${collected_defines}, -extra-arg=-D>>
              # NOTE: generator expression, expands during build time
              # if the ${ITEM} is non-empty, than append it
              # To suppress compiler diagnostic messages
              # from third-party headers just use -isystem
              # instead of -I to include those headers.
              # $<$<BOOL:${collected_includes}>:-extra-arg=-isystem$<JOIN:${collected_includes}, -extra-arg=-isystem>>
              $<$<BOOL:${collected_includes}>:-extra-arg=-I$<JOIN:${collected_includes}, -extra-arg=-I>>
              ${TARGET_SOURCES}
            EXTRA_OPTIONS ${OCLINT_OPTIONS}
          )
          # run valgrind on each build of target
          add_dependencies(
            ${LIB_NAME}
            ${LIB_NAME}_run_oclint
          )
        endforeach()
      else(ARGUMENTS_CHECK_TARGETS)
        if(ARGUMENTS_VERBOSE)
          message(STATUS "oclint: no CHECK_TARGETS provided")
        endif(ARGUMENTS_VERBOSE)
      endif(ARGUMENTS_CHECK_TARGETS)
    else(OCLINT_PROGRAM)
      message(WARNING "Program 'oclint' not found, unable to run 'oclint'.")
    endif(OCLINT_PROGRAM)
  else() # ARGUMENTS_IS_ENABLED
    if(ARGUMENTS_VERBOSE)
      message(STATUS "oclint disabled")
    endif(ARGUMENTS_VERBOSE)
  endif() # ARGUMENTS_IS_ENABLED
endfunction(oclint_enabler)

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_oclint
oclint_enabler(
  PATHS
    ${CONAN_BIN_DIRS}
    ${CONAN_BIN_DIRS_LLVM_TOOLS}
  #NO_SYSTEM_ENVIRONMENT_PATH
  #NO_CMAKE_SYSTEM_PATH
  IS_ENABLED
    ${ENABLE_OCLINT}
  CHECK_TARGETS
    ${LIB_NAME}
  EXTRA_OPTIONS
    # OCLINT command-line manual
    # https://oclint-docs.readthedocs.io/en/stable/manual/oclint.html
    -extra-arg=-std=c++17
    -extra-arg=-Qunused-arguments
    # To suppress compiler diagnostic messages
    # from third-party headers just use -isystem
    # instead of -I to include those headers.
    #-extra-arg=-nostdinc
    #-extra-arg=-nostdinc++
    -extra-arg=-DBOOST_SYSTEM_NO_DEPRECATED
    -extra-arg=-DBOOST_ERROR_CODE_HEADER_ONLY
    # Enable Clang Static Analyzer,
    # and integrate results into OCLint report
    -enable-clang-static-analyzer
    # Compile every source, and analyze across global contexts
    # (depends on number of source files,
    # could results in high memory load)
    # -enable-global-analysis
    # Write output to <path>
    -o=${CMAKE_CURRENT_BINARY_DIR}/report.html
    -report-type html
    # Build path is used to read a compile command database.
    #-p=${CMAKE_CURRENT_BINARY_DIR}
    # Add directory to rule loading path
    #-R=${CMAKE_CURRENT_SOURCE_DIR}
    # Disable the anonymous analytics
    -no-analytics
    #-rc=<parameter>=<value>       - Override the default behavior of rules
    #-report-type=<name>           - Change output report type
    #-rule=<rule name>             - Explicitly pick rules
    # -e exclude_this_folder*
    #-max-priority-2=15 -max-priority-3=30
    #-rc CYCLOMATIC_COMPLEXITY=4 -rc LONG_CLASS=200 -rc LONG_LINE=120
    #-rc LONG_METHOD=10 -rc NPATH_COMPLEXITY=20 -rc NESTED_BLOCK_DEPTH=3
    #-rc TOO_MANY_PARAMETERS=5 -rc LONG_VARIABLE_NAME=30
    # disable compiler errors and compiler warnings in my report
    #-extra-arg=-Wno-everything
  VERBOSE
  REQUIRED
)

if(ENABLE_TESTS)
  add_subdirectory( tests )
endif()
