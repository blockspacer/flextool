# see https://github.com/chromium/chromium/blob/9db0b5162b8648833c3a6b1af044cebd9d7d3855/base/BUILD.gn

cmake_minimum_required(VERSION 3.4)

# project() must be before checks, see https://stackoverflow.com/a/26437667/10904212
set(PROJECT_NAME "flextool")
set(PROJECT_VERSION 1.0.0.1)
project(${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  LANGUAGES CXX C)

set(LIB_NAME ${PROJECT_NAME})

option(ENABLE_TESTS "Enable tests" OFF)

# Hack to depend NOT on CONAN_PKG::libname, but on libname directly
# See for details
# https://docs.conan.io/en/latest/developing_packages/workspaces.html
set(LOCAL_BUILD FALSE CACHE BOOL "LOCAL_BUILD")
message(STATUS "LOCAL_BUILD=${LOCAL_BUILD}")

set(ENABLE_CLING TRUE CACHE BOOL "ENABLE_CLING")
message(STATUS "ENABLE_CLING=${ENABLE_CLING}")

set(ENABLE_CLANG_FROM_CONAN FALSE CACHE BOOL "ENABLE_CLANG_FROM_CONAN")
message(STATUS "ENABLE_CLANG_FROM_CONAN=${ENABLE_CLANG_FROM_CONAN}")

if(ENABLE_CLANG_FROM_CONAN AND ENABLE_CLING)
  message(FATAL_ERROR
    "don't use both ENABLE_CLING and ENABLE_CLANG_FROM_CONAN at the same time. cling already provides clang libtooling")
endif()

set(CUSTOM_PLUGINS
    "${CMAKE_CURRENT_SOURCE_DIR}/custom_plugins.cmake"
    CACHE STRING
    "Path to custom plugins")
message(STATUS "CUSTOM_PLUGINS=${CUSTOM_PLUGINS}")
if(EXISTS ${CUSTOM_PLUGINS})
  include(${CUSTOM_PLUGINS})
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
if(LOCAL_BUILD)
  set(flexlib flexlib)
endif(LOCAL_BUILD)

list(APPEND USED_3DPARTY_LIBS
  ${flexlib}
  CONAN_PKG::basis
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

set_property(TARGET ${LIB_NAME} PROPERTY CXX_STANDARD 17)

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
target_link_libraries(${LIB_NAME} PUBLIC#PRIVATE
  ${USED_3DPARTY_LIBS}
  ${USED_SYSTEM_LIBS}
)

target_link_libraries( ${LIB_NAME} PRIVATE
  CONAN_PKG::entt
  #${base_LIB}
  #${build_util_LIB}
  #CONAN_PKG::corrade
  #Corrade::PluginManager
)

# without rtti you will get error:
# ERROR: boost::bad_any_cast: failed conversion using boost::any_cast
target_compile_options(${LIB_NAME} PRIVATE
  -frtti)

set(DEBUG_LIBRARY_SUFFIX "-d")
set_target_properties(${LIB_NAME}
  PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" # TODO: /lib
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" # TODO: /lib
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" # TODO: /bin
    #OUTPUT_NAME "${LIB_NAME}$<$<CONFIG:Debug>:${DEBUG_LIBRARY_SUFFIX}>"
    ENABLE_EXPORTS 1
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    CMAKE_CXX_STANDARD_REQUIRED ON
)

if(LOCAL_BUILD)
  # bin
  list(APPEND CMAKE_PROGRAM_PATH ${CMAKE_BINARY_DIR})
  list(APPEND CMAKE_PROGRAM_PATH ${CMAKE_CURRENT_BINARY_DIR})
  # lib
  #link_directories(${CMAKE_BINARY_DIR}/lib)
  #link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)
endif(LOCAL_BUILD)

# POSITION_INDEPENDENT_CODE for -fPIC
set_property(TARGET ${LIB_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

if(ENABLE_CLING)
  find_package(Cling REQUIRED)

  list(APPEND CLING_DEFINITIONS CLING_IS_ON=1)
  target_link_libraries(${LIB_NAME} PUBLIC#PRIVATE
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
  $<$<CXX_COMPILER_ID:GNU>:-Wall>)

target_compile_definitions(${LIB_NAME} PRIVATE
  ${flextool_PRIVATE_DEFINES}
  # https://stackoverflow.com/a/30877725
  BOOST_SYSTEM_NO_DEPRECATED BOOST_ERROR_CODE_HEADER_ONLY
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

if(ENABLE_TESTS)
  add_subdirectory( tests )
endif()
