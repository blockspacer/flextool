# Note: COMMAND_EXPAND_LISTS is keyword is only available with CMake version >= 3.8
cmake_minimum_required(VERSION 3.8)

# project() must be before checks, see https://stackoverflow.com/a/26437667/10904212
set(PROJECT_NAME "flextool")

# Parse the VERSION file.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS VERSION)
file(READ "VERSION" VERSION_FILE_CONTENTS)
string(REGEX MATCH
  "([0-9]+)\.([0-9]+)\.([0-9]+)(-[a-zA-Z0-9]+)?"
  ${PROJECT_NAME}_VERSION_STRING ${VERSION_FILE_CONTENTS})
if(NOT ${PROJECT_NAME}_VERSION_STRING)
  message(FATAL_ERROR "Could not parse 'VERSION' file")
endif()
set(${PROJECT_NAME}_VERSION_MAJOR  ${CMAKE_MATCH_1})
set(${PROJECT_NAME}_VERSION_MINOR  ${CMAKE_MATCH_2})
set(${PROJECT_NAME}_VERSION_PATCH  ${CMAKE_MATCH_3})
set(${PROJECT_NAME}_VERSION_SUFFIX ${CMAKE_MATCH_4})

set(PROJECT_VERSION
  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}")

project(${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  LANGUAGES CXX C)

set(LIB_NAME ${PROJECT_NAME})

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Helper module for selecting option based on multiple values. Module help:
# https://cmake.org/cmake/help/latest/module/CMakeDependentOption.html
include(CMakeDependentOption)

option(ENABLE_LWYU
  "Enable CMAKE_LINK_WHAT_YOU_USE" ON)

option(COMPILE_WITH_LLVM_TOOLS
  "Enable clnag from llvm_tools (conan package)" OFF)

option(ENABLE_TESTS
  "Enable tests" OFF)

option(ENABLE_CPPCHECK
  "Enable cppcheck" OFF)

# TODO: Dr. Memory https://github.com/DynamoRIO/drmemory

option(ENABLE_VALGRIND
  "Enable valgrind" OFF)

option(ENABLE_CLANG_TIDY
  "Enable clang-tidy" OFF)

option(ENABLE_CLANG_FORMAT
  "Enable clang-format" OFF)

option(ENABLE_UNCRUSTIFY
  "Enable uncrustify" OFF)

option(ENABLE_IWYU
  "Enable include-what-you-use" OFF)

option(ENABLE_CPPCLEAN
  "Enable cppclean" OFF)

# see https://github.com/Ericsson/codechecker/blob/master/tools/report-converter/README.md#undefined-behaviour-sanitizer
# NOTE: Compile with -g and -fno-omit-frame-pointer
# to get proper debug information in your binary.
# NOTE: Run your program with environment variable UBSAN_OPTIONS=print_stacktrace=1.
# see https://github.com/google/sanitizers/wiki/SanitizerCommonFlags
option(ENABLE_UBSAN
  "Enable Undefined Behaviour Sanitizer" OFF)

# see https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer
# see https://github.com/Ericsson/codechecker/blob/master/tools/report-converter/README.md#address-sanitizer
# NOTE: Compile with -g and -fno-omit-frame-pointer
# to get proper debug information in your binary.
# NOTE: use ASAN_OPTIONS=detect_leaks=1 LSAN_OPTIONS=suppressions=suppr.txt
# NOTE: You need the ASAN_OPTIONS=symbolize=1
# to turn on resolving addresses in object code
# to source code line numbers and filenames.
# This option is implicit for Clang but it won't do any harm.
# see https://github.com/google/sanitizers/wiki/SanitizerCommonFlags
option(ENABLE_ASAN
  "Enable Address Sanitizer" OFF)

# see https://github.com/Ericsson/codechecker/blob/master/tools/report-converter/README.md#memory-sanitizer
# NOTE: Compile with -g and -fno-omit-frame-pointer
# to get proper debug information in your binary.
option(ENABLE_MSAN
  "Enable Memory Sanitizer" OFF)

# see https://github.com/Ericsson/codechecker/blob/master/tools/report-converter/README.md#thread-sanitizer
# NOTE: Compile with -g
# to get proper debug information in your binary.
option(ENABLE_TSAN
  "Enable Thread Sanitizer" OFF)

# Things that can catch OCLINT
# http://oclint-docs.readthedocs.io/en/stable/rules/index.html
# OCLINT command-line manual
# https://oclint-docs.readthedocs.io/en/stable/manual/oclint.html
option(ENABLE_OCLINT
  "Enable oclint" OFF)

# TODO: __do_global_dtors_aux, base::debug::CollectStackTrace
option(ENABLE_VALGRIND_TESTS
  "Enable valgrind for unit tests" OFF)

if(ENABLE_LWYU)
  # Enable linker flags -r -u to create warnings for unused dependencies at link time.
  # Warning: Unused direct dependencies: '/usr/lib/libm.so.6' (required by std)
  # LWYU will modify the flags to ld to show any libraries
  # being linked into targets that are not contributing symbols
  # to the target being linked.
  # see https://cmake.org/cmake/help/latest/prop_tgt/LINK_WHAT_YOU_USE.html
  # NOTE: This is only applicable to executable and shared library targets
  set(CMAKE_LINK_WHAT_YOU_USE ON)
endif(ENABLE_LWYU)

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
option(CONAN_AUTO_INSTALL
  "Let CMake call conan install automatically"
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
    CONAN_OPTIONS "--profile=${CONAN_PROFILE} -s build_type=${conan_build_type} -s cling_conan:build_type=Release -s llvm_tools:build_type=Release -o openssl:shared=True"
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

set_project_version(
  ${${PROJECT_NAME}_VERSION_MAJOR}
  ${${PROJECT_NAME}_VERSION_MINOR}
  ${${PROJECT_NAME}_VERSION_PATCH}) # from Utils.cmake

check_cmake_build_type_selected() # from Utils.cmake

enable_colored_diagnostics() # from Utils.cmake

print_cmake_system_info() # from Utils.cmake

check_supported_os() # from Utils.cmake

find_package(cmake_helper_utils REQUIRED)

if(ENABLE_MSAN OR ENABLE_TSAN OR ENABLE_ASAN OR ENABLE_UBSAN)
  find_package(cmake_sanitizers REQUIRED)
endif()

if(COMPILE_WITH_LLVM_TOOLS)
  compile_with_llvm_tools()
endif(COMPILE_WITH_LLVM_TOOLS)

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

# Executable target definition. CMake-build-system documentation:
# https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html
add_executable(${LIB_NAME}
  ${flextool_SOURCES}
  ${flextool_src_DIR}/main.cc
  ${flextool_include_DIR}/version.hpp
)

# Alias target for the executable to be used outside of the project. Command:
# https://cmake.org/cmake/help/latest/command/add_executable.html
add_executable(flextool::exe ALIAS ${LIB_NAME})

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
  #TODO: doctest
  #CONAN_PKG::doctest
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
    /Wall
    /analyze
  >
  $<$<CXX_COMPILER_ID:GNU>:
    -Wformat=2
    -Wall
    -W
    -Wextra
    -Wpedantic
  >
  $<$<CXX_COMPILER_ID:Clang>:
    -Wformat=2
    -Wall
    -W
    -Wextra
    -Wpedantic
    -Wdeprecated-register
    # disable warning: designated initializers are a C99 feature
    -Wno-c99-extensions
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

# TODO: DISABLE_DOCTEST
target_compile_definitions(${LIB_NAME} PUBLIC
  DISABLE_DOCTEST=1
)

# TODO
# Optionally enable Clang's libfuzzer.
#if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND ${PROJECT_NAME}_ENABLE_LIBFUZZER)
#  target_compile_options(${TARGET_NAME} PRIVATE "-fsanitize=fuzzer")
#  target_compile_options(${TARGET_NAME} PRIVATE "-fsanitize-coverage=trace-cmp,trace-div,trace-gep")
#endif()

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

## ---------------------------- valgrind -------------------------------- ##

if(ENABLE_VALGRIND)
  check_valgrind_config()
endif()

## ---------------------------- cppcheck -------------------------------- ##

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_run_cppcheck
cppcheck_enabler(
  PATHS
    # to use cppcheck_installer from conan
    #${CONAN_BIN_DIRS}
    ${CONAN_BIN_DIRS_LLVM_TOOLS}
    # to use from cmake subfolder
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

## ---------------------------- clang_tidy -------------------------------- ##

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_clang_tidy
clang_tidy_enabler(
  PATHS
    #${CONAN_BIN_DIRS}
    ${CONAN_BIN_DIRS_LLVM_TOOLS}
  NO_SYSTEM_ENVIRONMENT_PATH
  NO_CMAKE_SYSTEM_PATH
  IS_ENABLED
    ${ENABLE_CLANG_TIDY}
  CHECK_TARGETS
    ${LIB_NAME}
  EXTRA_OPTIONS
    # TODO: use file .clang-tidy, similar to
    # https://github.com/mapbox/protozero/blob/ed6ba50979acf97b9c5300ae88a20d8b8b6c2fc4/.clang-tidy
    # https://fuchsia.googlesource.com/fuchsia/+/f7851796155f50b57d5ebd26a43ef49f5d5bdee4/.clang-tidy
    # https://github.com/jar-git/cmake-template/blob/master/.clang-tidy
    #
    # see clang-tidy --list-checks -checks='*' | grep "modernize"
    # see list of clang-tidy checks:
    # https://clang.llvm.org/extra/clang-tidy/checks/list.html
    #
    #  Disabled checks:
    #
    #  bugprone-signed-char-misuse
    #    Lots of warnings in varint.hpp otherwise.
    #
    #  cert-dcl21-cpp
    #    It is unclear whether this is still a good recommendation in modern C++.
    #
    #  cert-err58-cpp
    #    Due to the Catch2 test framework.
    #
    #  cert-err60-cpp
    #    Reports std::runtime_error as broken which we can't do anything about.
    #
    #  cppcoreguidelines-avoid-c-arrays
    #  hicpp-avoid-c-arrays
    #  modernize-avoid-c-arrays
    #    Makes sense for some array, but especially for char arrays using
    #    std::array isn't a good solution.
    #
    #  cppcoreguidelines-avoid-magic-numbers
    #  readability-magic-numbers
    #    Good idea, but it goes too far to force this everywhere.
    #
    #  cppcoreguidelines-macro-usage
    #    There are cases where macros are simply needed.
    #
    #  cppcoreguidelines-pro-bounds-array-to-pointer-decay
    #    Limited use and many false positives including for all asserts.
    #
    #  cppcoreguidelines-pro-bounds-pointer-arithmetic
    #    This is a low-level library, it needs to do pointer arithmetic.
    #
    #  cppcoreguidelines-pro-type-reinterpret-cast
    #    This is a low-level library, it needs to do reinterpret-casts.
    #
    #  fuchsia-*
    #    Much too strict.
    #
    #  google-runtime-references
    #    This is just a matter of preference, and we can't change the interfaces
    #    now anyways.
    #
    #  hicpp-no-array-decay
    #    Limited use and many false positives including for all asserts.
    #
    #  modernize-use-trailing-return-type
    #    We are not quite that modern.
    #    We do not want to rewrite code like so:
    #    int main(int argc, char* argv[])
    #    ~~~~
    #    auto                            -> int
    #
    #  readability-implicit-bool-conversion
    #    Not necessarily more readable.
    #
    -checks=*,-bugprone-signed-char-misuse,-cert-dcl21-cpp,-cert-err58-cpp,-cert-err60-cpp,-cppcoreguidelines-avoid-c-arrays,-cppcoreguidelines-avoid-magic-numbers,-cppcoreguidelines-macro-usage,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-pro-bounds-array-to-pointer-decay,-cppcoreguidelines-pro-type-reinterpret-cast,-fuchsia-*,-google-runtime-references,-hicpp-avoid-c-arrays,-hicpp-no-array-decay,-hicpp-vararg,-modernize-avoid-c-arrays,-modernize-use-trailing-return-type,-readability-implicit-bool-conversion,-readability-magic-numbers
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

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_cppclean
cppclean_enabler(
  PATHS
    #${CONAN_BIN_DIRS}
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

# fixes oclint: error: violations exceed threshold
# see https://stackoverflow.com/a/30151220
set(oclintMaxPriority 15000)

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_oclint
oclint_enabler(
  PATHS
    #${CONAN_BIN_DIRS}
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
    # ignore system headers
    #-e /usr/*
    # ignore third-party lib
    #-e corrade/*
    # disable compiler errors and compiler warnings in my report
    #-extra-arg=-Wno-everything
    -stats
    -max-priority-1=${oclintMaxPriority}
    -max-priority-2=${oclintMaxPriority}
    -max-priority-3=${oclintMaxPriority}
    # see list of rules at
    # https://oclint-docs.readthedocs.io/en/stable/rules/
    #-disable-rule GotoStatement
    # Cyclomatic complexity of a method
    # see http://docs.oclint.org/en/stable/howto/thresholds.html#available-thresholds
    -rc CYCLOMATIC_COMPLEXITY=15
    # Number of lines for a C class or Objective-C interface, category, protocol, and implementation
    # see http://docs.oclint.org/en/stable/howto/thresholds.html#available-thresholds
    -rc LONG_CLASS=500
    # limit num. of characters in line
    # see http://docs.oclint.org/en/stable/howto/thresholds.html#available-thresholds
    -rc LONG_LINE=500
    # Number of lines for a method or function
    # see http://docs.oclint.org/en/stable/howto/thresholds.html#available-thresholds
    -rc LONG_METHOD=50
    # limit num. of characters in varibale name
    # see http://docs.oclint.org/en/stable/howto/thresholds.html#available-thresholds
    -rc LONG_VARIABLE_NAME=100
    # Number of lines for the if block that would prefer an early exists
    # see http://docs.oclint.org/en/stable/howto/thresholds.html#available-thresholds
    -rc MAXIMUM_IF_LENGTH=15
    # Count of case statements in a switch statement
    # see http://docs.oclint.org/en/stable/howto/thresholds.html#available-thresholds
    -rc MINIMUM_CASES_IN_SWITCH=3
    # NPath complexity of a method
    # see http://docs.oclint.org/en/stable/howto/thresholds.html#available-thresholds
    -rc NPATH_COMPLEXITY=20
    # Number of non-commenting source statements of a method
    # see http://docs.oclint.org/en/stable/howto/thresholds.html#available-thresholds
    -rc NCSS_METHOD=30
    # Depth of a block or compound statement
    # see http://docs.oclint.org/en/stable/howto/thresholds.html#available-thresholds
    -rc NESTED_BLOCK_DEPTH=6
    # Number of characters for a variable name
    # see http://docs.oclint.org/en/stable/howto/thresholds.html#available-thresholds
    -rc SHORT_VARIABLE_NAME=3
    # Number of fields of a class
    # see http://docs.oclint.org/en/stable/howto/thresholds.html#available-thresholds
    -rc TOO_MANY_FIELDS=50
    # Number of methods of a class
    # see http://docs.oclint.org/en/stable/howto/thresholds.html#available-thresholds
    -rc TOO_MANY_METHODS=50
    # Number of parameters of a method
    # see http://docs.oclint.org/en/stable/howto/thresholds.html#available-thresholds
    -rc TOO_MANY_PARAMETERS=10
  VERBOSE
  REQUIRED
)

## ---------------------------- iwyu -------------------------------- ##

# include-what-you-use mappings file
# Mappings file format:
#   { include: [ '@"mutt/.*"', private, '"mutt/mutt.h"', public ] },
#   { include: [ '@"conn/.*"', private, '"conn/conn.h"', public ] },
set(IWYU_IMP "${CMAKE_CURRENT_SOURCE_DIR}/cmake/iwyu/iwyu.imp")
if(NOT EXISTS ${IWYU_IMP})
  message(FATAL_ERROR "Unable to find file: ${IWYU_IMP}")
endif(NOT EXISTS ${IWYU_IMP})

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_iwyu
iwyu_enabler(
  PATHS
    #${CONAN_BIN_DIRS}
    ${CONAN_BIN_DIRS_LLVM_TOOLS}
  NO_SYSTEM_ENVIRONMENT_PATH
  NO_CMAKE_SYSTEM_PATH
  IS_ENABLED
    ${ENABLE_IWYU}
  CHECK_TARGETS
    ${LIB_NAME}
  EXTRA_OPTIONS
    # -Xiwyu --transitive_includes_only
    # pch_in_code: The file has an important header first
    # -Xiwyu --pch_in_code
    # no_comments: Don’t add notes to the output
    -Xiwyu --no_comments
    # mapping_file: lookup file
    -Xiwyu --mapping_file=${IWYU_IMP}
    -Xiwyu --max_line_length=120
    # see https://github.com/include-what-you-use/include-what-you-use/issues/760
    -Wno-unknown-warning-option
    # when sorting includes, place quoted ones first.
    -Xiwyu --quoted_includes_first
    # suggests the more concise syntax introduced in C++17
    -Xiwyu --cxx17ns
    # max_line_length: maximum line length for includes.
    # Note that this only affects comments and alignment thereof,
    # the maximum line length can still be exceeded
    # with long file names (default: 80).
    -Xiwyu --max_line_length=180
    #-Xiwyu --check_also=${CMAKE_CURRENT_SOURCE_DIR}/src/*
    #-Xiwyu --check_also=${CMAKE_CURRENT_SOURCE_DIR}/src/*/*
    #-Xiwyu --check_also=${CMAKE_CURRENT_SOURCE_DIR}/src/*/*/*
    #-Xiwyu --check_also=${CMAKE_CURRENT_SOURCE_DIR}/src/*/*/*/*
  VERBOSE
  REQUIRED
)

## ---------------------------- clang-format -------------------------------- ##

set(EXTRA_CLANG_FORMAT_FILES
  # place here files what need to be to be formatted
)

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_clang_format
clang_format_enabler(
  PATHS
    #${CONAN_BIN_DIRS}
    ${CONAN_BIN_DIRS_LLVM_TOOLS}
  NO_SYSTEM_ENVIRONMENT_PATH
  NO_CMAKE_SYSTEM_PATH
  IS_ENABLED
    ${ENABLE_CLANG_FORMAT}
  CHECK_TARGETS
    ${LIB_NAME}
  EXTRA_OPTIONS
    # ferror-limit:
    # Used only with --dry-run or -n
    --ferror-limit=9999
    # Use -style=file to load style configuration from
    # .clang-format file located in one of the parent
    # directories of the source file (or current
    # directory for stdin).
    -style=file
    #  The -i option indicates to apply the formatting options
    # to the files in-place, rather than spitting out
    # the formatted version of the files to the command line.
    -i
    ${EXTRA_CLANG_FORMAT_FILES}
  VERBOSE
  REQUIRED
)

## ---------------------------- uncrustify -------------------------------- ##

set(EXTRA_UNCRUSTIFY_FILES
  # place here files what need to be to be formatted
)

# USAGE:
# cmake -E time cmake --build . --target TARGET_NAME_uncrustify
uncrustify_enabler(
  PATHS
    #${CONAN_BIN_DIRS}
    ${CONAN_BIN_DIRS_LLVM_TOOLS}
  IS_ENABLED
    ${ENABLE_UNCRUSTIFY}
  CHECK_TARGETS
    ${LIB_NAME}
  EXTRA_OPTIONS
    --no-backup
    -c ${CMAKE_CURRENT_SOURCE_DIR}/uncrustify.cfg
    ${EXTRA_UNCRUSTIFY_FILES}
  VERBOSE
  REQUIRED
)

## ---------------------------- sanitizers -------------------------------- ##

if(ENABLE_MSAN OR ENABLE_TSAN OR ENABLE_ASAN OR ENABLE_UBSAN)
  # use llvm_tools from conan
  find_program_helper(llvm-symbolizer
    PATHS
      #${CONAN_BIN_DIRS}
      ${CONAN_BIN_DIRS_LLVM_TOOLS}
    NO_SYSTEM_ENVIRONMENT_PATH
    NO_CMAKE_SYSTEM_PATH
    ${ARGUMENTS_UNPARSED_ARGUMENTS}
    REQUIRED
    OUT_VAR LLVM_SYMBOLIZER_PROGRAM
    VERBOSE TRUE
  )

  check_sanitizer_options(
    ENABLE_TSAN ${ENABLE_TSAN}
    ENABLE_ASAN ${ENABLE_ASAN}
    ENABLE_MSAN ${ENABLE_MSAN}
    ENABLE_UBSAN ${ENABLE_UBSAN}
    LLVM_SYMBOLIZER_PROGRAM ${LLVM_SYMBOLIZER_PROGRAM}
  )
endif()

if(ENABLE_MSAN)
  message(STATUS "enabling MSAN on ${LIB_NAME}")
  add_msan_static_link(${LIB_NAME})
  add_msan_flags(${LIB_NAME})
endif(ENABLE_MSAN)

if(ENABLE_TSAN)
  message(STATUS "enabling TSAN on ${LIB_NAME}")
  add_tsan_static_link(${LIB_NAME})
  add_tsan_flags(${LIB_NAME})
endif(ENABLE_TSAN)

if(ENABLE_ASAN)
  message(STATUS "enabling ASAN on ${LIB_NAME}")
  add_asan_static_link(${LIB_NAME})
  add_asan_flags(${LIB_NAME})
endif(ENABLE_ASAN)

if(ENABLE_UBSAN)
  message(STATUS "enabling UBSAN on ${LIB_NAME}")
  add_ubsan_static_link(${LIB_NAME})
  add_ubsan_flags(${LIB_NAME})
endif(ENABLE_UBSAN)

## ---------------------------- tests -------------------------------- ##

if(ENABLE_TESTS)
  add_subdirectory( tests )
endif()

# Create a dummy target to hold various files in the project root
add_custom_target(RootFiles SOURCES
  LICENSE
  README.md
  VERSION
  conanfile.py)
