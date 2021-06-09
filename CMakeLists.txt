# Note: COMMAND_EXPAND_LISTS is keyword is only available with CMake version >= 3.8
cmake_minimum_required(VERSION 3.8)

# ------------------------------- project settings --------------------------------------

# project() must be before checks, see https://stackoverflow.com/a/26437667/10904212
set(PROJECT_NAME "flextool")

set(FLEXTOOL_LIB_NAME ${PROJECT_NAME})

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/parse_version_file.cmake)

project(${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  LANGUAGES CXX C)

# sets cmake_build_type_tolower, PROJECT_SOURCE_DIR, etc.
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/global_constants.cmake)

# ------------------------- External CMake includes ----------------------------

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_externals.cmake)

# ------------------------------- Options --------------------------------------

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Options.cmake)

# --------------------------- conan configuration ------------------------------

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch_cmake_utils.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch_conan_auto_install.cmake)

option(CONAN_AUTO_INSTALL
  "Let CMake call conan install automatically"
  OFF
)
if (CONAN_AUTO_INSTALL)
  set(CONAN_PROFILE
      "clang"
      CACHE STRING "Conan profile to use during installation")
  if (NOT cmake_build_type_tolower MATCHES "debug" )
    set(conan_build_type "Release")
  else()
    set(conan_build_type "Debug")
  endif()
  # No --build=missing cause llvm requires long build
  # You can use `EXTRA_CONAN_AUTO_INSTALL_OPTIONS` like so:
  # cmake .. -DCONAN_AUTO_INSTALL=ON -DEXTRA_CONAN_AUTO_INSTALL_OPTIONS="--build missing" -DCMAKE_BUILD_TYPE=Debug
  conan_auto_install(
    CONAN_OPTIONS "--profile=${CONAN_PROFILE} -s build_type=${conan_build_type} -s cling_conan:build_type=Release -s llvm_tools:build_type=Release -o openssl:shared=True ${EXTRA_CONAN_AUTO_INSTALL_OPTIONS}"
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

# ------------------------------- settings --------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Generate clang compilation database
# see https://stackoverflow.com/a/31086619/10904212
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(cmake_platform_detection REQUIRED)

# populates variables with TRUE or FALSE:
# PLATFORM_MOBILE | PLATFORM_WEB | PLATFORM_DESKTOP
# | TARGET_EMSCRIPTEN | TARGET_LINUX | TARGET_WINDOWS | TARGET_MACOS
# | TARGET_IOS | TARGET_ANDROID
run_cmake_platform_detection() # from cmake_platform_detection

find_package(cmake_build_options REQUIRED)

# set default cmake build type if CMAKE_BUILD_TYPE not set
setup_default_build_type(RELEASE) # from cmake_build_options

# Limits possible values of CMAKE_BUILD_TYPE
# Also populates variables with TRUE or FALSE:
# RELEASE_BUILD | DEBUG_BUILD | PROFILE_BUILD
# | MINSIZEREL_BUILD | RELWITHDEBINFO_BUILD | COVERAGE_BUILD
# | DOCS_BUILD | TEST_BUILD
setup_cmake_build_options(RELEASE DEBUG) # from cmake_build_options

message(STATUS
  "Compiler ${CMAKE_CXX_COMPILER}, version: ${CMAKE_CXX_COMPILER_VERSION}")

# CMAKE_BUILD_TYPE must be not empty
check_cmake_build_type_selected() # from Utils.cmake

# beautify compiler output
enable_colored_diagnostics() # from Utils.cmake

# log cmake vars
print_cmake_system_info() # from Utils.cmake

# limit to known platforms
check_supported_os() # from Utils.cmake

find_package(cmake_helper_utils REQUIRED)

# prefer ASCII for folder names
force_latin_paths() # from cmake_helper_utils (conan package)

# out dirs (CMAKE_*_OUTPUT_DIRECTORY) must be not empty
validate_out_dirs() # from cmake_helper_utils (conan package)

# In-source builds not allowed
validate_out_source_build(WARNING) # from cmake_helper_utils (conan package)

if(ENABLE_MSAN OR ENABLE_TSAN OR ENABLE_ASAN OR ENABLE_UBSAN)
  find_package(cmake_sanitizers REQUIRED)
endif()

if(ENABLE_MSAN)
  add_msan_flags()
endif(ENABLE_MSAN)

if(ENABLE_TSAN)
  add_tsan_flags()
endif(ENABLE_TSAN)

if(ENABLE_ASAN)
  add_asan_flags()
endif(ENABLE_ASAN)

if(ENABLE_UBSAN)
  add_ubsan_flags()
endif(ENABLE_UBSAN)

if(COMPILE_WITH_LLVM_TOOLS)
  # force change CMAKE_*_COMPILER and CMAKE_LINKER to clang from conan
  compile_with_llvm_tools() # from cmake_helper_utils (conan package)
endif(COMPILE_WITH_LLVM_TOOLS)

# Keep symbols for JIT resolution
set(LLVM_NO_DEAD_STRIP 1)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformChecks.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CommonOptions.cmake)

# ------------------------------- thirdparty_libs --------------------------------------

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/setup_thirdparty_libs.cmake)

# ------------------------------- source files --------------------------------------

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProjectFiles.cmake)

add_library(${FLEXTOOL_LIB_NAME}-test-includes INTERFACE)

set(VERSION_HEADER_LOCATION
  ${flextool_include_DIR}/version.hpp)

configure_file_if_changed(
  INPUT ${flextool_include_DIR}/version.hpp.in
  OUTPUT ${VERSION_HEADER_LOCATION}
  TMP_FILE ${CMAKE_CURRENT_BINARY_DIR}/generated_version_hpp.tmp)

set_source_files_properties(${PROJECT_BINARY_DIR}/version.hpp
  PROPERTIES GENERATED 1)

# -------------------------------- Executable ------------------------------------

# Executable target definition. CMake-build-system documentation:
# https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html
add_executable(${FLEXTOOL_LIB_NAME}
  ${flextool_SOURCES} # from ProjectFiles.cmake
  ${flextool_src_DIR}/main.cc
  ${VERSION_HEADER_LOCATION}
)

# Alias target for the executable to be used outside of the project. Command:
# https://cmake.org/cmake/help/latest/command/add_executable.html
add_executable(flextool::exe
  ALIAS ${FLEXTOOL_LIB_NAME})

# -------------------------------- link_libraries ------------------------------------

target_link_libraries(${FLEXTOOL_LIB_NAME} PUBLIC
  -lncurses
  -ltinfo
  ${USED_3DPARTY_LIBS}
  ${USED_SYSTEM_LIBS}
)

target_link_libraries( ${FLEXTOOL_LIB_NAME} PRIVATE
  CONAN_PKG::entt
  chromium_icu::chromium_icu-static
)

if(ENABLE_CLING)
  find_package(Cling REQUIRED)

  list(APPEND CLING_DEFINITIONS CLING_IS_ON=1)
  target_link_libraries(${FLEXTOOL_LIB_NAME} PUBLIC
    CONAN_PKG::cling_conan
  )

  if(MSVC)
    set_target_properties(${FLEXTOOL_LIB_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS 1)
    set_property(
      TARGET ${FLEXTOOL_LIB_NAME}
      APPEND_STRING
      PROPERTY LINK_FLAGS
               "/EXPORT:?setValueNoAlloc@internal@runtime@cling@@YAXPEAX00D_K@Z
                /EXPORT:?setValueNoAlloc@internal@runtime@cling@@YAXPEAX00DM@Z
                /EXPORT:cling_runtime_internal_throwIfInvalidPointer")
  endif()
endif(ENABLE_CLING)

# -------------------------------- Includes ------------------------------------

# $<INSTALL_INTERFACE:...> is exported using install(EXPORT)
# $<BUILD_INTERFACE:...> is exported using export(), or when the target is used by another target in the same buildsystem
macro(add_relative_include_dir TARGET VISIBILITY_BUILD VISIBILITY_INSTALL NEW_ELEM)
  target_include_directories(${TARGET}
    ${VISIBILITY_BUILD} "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${NEW_ELEM}>"
    ${VISIBILITY_INSTALL} "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}/${NEW_ELEM}>"
  )
  target_include_directories( ${FLEXTOOL_LIB_NAME}-test-includes SYSTEM INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/${NEW_ELEM} )
endmacro(add_relative_include_dir)

#add_relative_include_dir(${FLEXTOOL_LIB_NAME} PRIVATE PRIVATE "include/flextool")

add_relative_include_dir(${FLEXTOOL_LIB_NAME} PUBLIC PUBLIC "include")

# ---------------------------------- Build options -------------------------------------

# startup target for MS Visual Studio IDE
set_vs_startup_project(${FLEXTOOL_LIB_NAME}) # from Utils.cmake

if(TARGET_EMSCRIPTEN)
  # use PROPERTY CXX_STANDARD 17
else()
  target_compile_features(${FLEXTOOL_LIB_NAME}
    PUBLIC cxx_auto_type
    PRIVATE cxx_variadic_templates)
endif()

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(${FLEXTOOL_LIB_NAME}
  PUBLIC "$<$<BOOL:${MSVC}>:/permissive->")

# without rtti you will get error:
# ERROR: boost::bad_any_cast: failed conversion using boost::any_cast
#target_compile_options(${FLEXTOOL_LIB_NAME} PRIVATE
#  -frtti)

set(DEBUG_LIBRARY_SUFFIX "")
set_target_properties(${FLEXTOOL_LIB_NAME}
  PROPERTIES
    # ENABLE_EXPORTS for -rdynamic
    # i.e. export symbols from the executables
    # required for plugin system
    # see https://stackoverflow.com/a/8626922
    ENABLE_EXPORTS ON
    POSITION_INDEPENDENT_CODE ON
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" # TODO: /lib
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" # TODO: /lib
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" # TODO: /bin
    ENABLE_EXPORTS 1
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    CMAKE_CXX_STANDARD_REQUIRED ON
)

# bin dirs used by https://docs.conan.io/en/latest/developing_packages/workspaces.html
list(APPEND CMAKE_PROGRAM_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PROGRAM_PATH ${CMAKE_CURRENT_BINARY_DIR})

# POSITION_INDEPENDENT_CODE for -fPIC
set_property(TARGET ${FLEXTOOL_LIB_NAME}
  PROPERTY POSITION_INDEPENDENT_CODE ON)

# ---------------------------------- compile_definitions -------------------------------------

# see https://cmake.org/cmake/help/v3.0/module/TestBigEndian.html
test_big_endian(${FLEXTOOL_LIB_NAME}_IS_BIG_ENDIAN)
if(${${FLEXTOOL_LIB_NAME}_IS_BIG_ENDIAN})
  list(APPEND flextool_PUBLIC_DEFINES
    IS_BIG_ENDIAN=1)
endif()

if(ENABLE_CLING)
  target_compile_definitions(${FLEXTOOL_LIB_NAME} PRIVATE CLING_IS_ON=1)
endif(ENABLE_CLING)

target_compile_definitions(${FLEXTOOL_LIB_NAME} PRIVATE
  ${flextool_PRIVATE_DEFINES}
  # https://stackoverflow.com/a/30877725
  BOOST_SYSTEM_NO_DEPRECATED
  BOOST_ERROR_CODE_HEADER_ONLY
  # TODO: make program_options work without rtti (boost::bad_any_cast)
  #BOOST_NO_RTTI
  #BOOST_NO_TYPEID
  # TODO
  #BOOST_NO_EXCEPTIONS
  BOOST_ASIO_STANDALONE=1
)

target_compile_definitions(${FLEXTOOL_LIB_NAME} PUBLIC
  ${flextool_PUBLIC_DEFINES}
  ${CLING_DEFINITIONS}
)

# TODO: DISABLE_DOCTEST
target_compile_definitions(${FLEXTOOL_LIB_NAME} PUBLIC
  DISABLE_DOCTEST=1
)

# ---------------------------------- libfuzzer -------------------------------------

# TODO
# Optionally enable Clang's libfuzzer.
#if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND ${PROJECT_NAME}_ENABLE_LIBFUZZER)
#  target_compile_options(${TARGET_NAME} PRIVATE "-fsanitize=fuzzer")
#  target_compile_options(${TARGET_NAME} PRIVATE "-fsanitize-coverage=trace-cmp,trace-div,trace-gep")
#endif()

# ---------------------------------- resource dirs -------------------------------------

# remove old resources dir
#add_custom_command( TARGET ${FLEXTOOL_LIB_NAME} PRE_BUILD
#  COMMAND ${CMAKE_COMMAND} -E remove_directory
#    $<TARGET_FILE_DIR:${FLEXTOOL_LIB_NAME}>/resources )

# copy new resources
add_custom_command( TARGET ${FLEXTOOL_LIB_NAME} PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/
    $<TARGET_FILE_DIR:${FLEXTOOL_LIB_NAME}>/resources )

# -------------------------------- Install -------------------------------------

# install and export steps
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Exports.cmake)

# It is always easier to navigate in an IDE when projects are organized in folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

## ---------------------------- Link Time Optimization -------------------------------- ##

if(ENABLE_LTO)
  # Check for LTO support (needs to be after project(...) )
  find_lto(CXX)

  # enable lto if available for non-debug configurations
  target_enable_lto(${FLEXTOOL_LIB_NAME} optimized)
else(ENABLE_LTO)
  if(cmake_build_type_tolower MATCHES "release" )
    message(WARNING "Enable LTO in Release builds")
  endif()
endif(ENABLE_LTO)

## ---------------------------- warning level -------------------------------- ##

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/setup_compile_warnings.cmake)

## ---------------------------- valgrind -------------------------------- ##

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/setup_valgrind.cmake)

## ---------------------------- cppcheck -------------------------------- ##

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/setup_cppcheck.cmake)

## ---------------------------- clang_tidy -------------------------------- ##

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/setup_clang_tidy.cmake)

## ---------------------------- cppclean -------------------------------- ##

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/setup_cppclean.cmake)

## ---------------------------- oclint -------------------------------- ##

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/setup_oclint.cmake)

## ---------------------------- iwyu -------------------------------- ##

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/setup_iwyu.cmake)

## ---------------------------- clang_format -------------------------------- ##

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/setup_clang_format.cmake)

## ---------------------------- uncrustify -------------------------------- ##

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/setup_uncrustify.cmake)

## ---------------------------- sanitizers -------------------------------- ##

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/setup_sanitizers.cmake)

## ---------------------------- gold linker -------------------------------- ##

# add_gold_linker
if(USE_LD_GOLD)
  add_gold_linker() # from cmake_helper_utils (conan package)
endif(USE_LD_GOLD)

## ---------------------------- ccache -------------------------------- ##

if(USE_CCACHE)
  add_ccache()
  target_ccache_summary(${FLEXTOOL_LIB_NAME}) # from cmake_helper_utils (conan package)
endif(USE_CCACHE)

## ---------------------------- coverage -------------------------------- ##

if(USE_COVERAGE)
  add_coverage() # from cmake_helper_utils (conan package)
endif(USE_COVERAGE)

## ---------------------------- RULE_MESSAGES property -------------------------------- ##

# controls RULE_MESSAGES property
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/setup_compile_log_level.cmake)

## ---------------------------- docs -------------------------------- ##

if(BUILD_DOXY_DOC)
  add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/docs )
endif()

## ---------------------------- tests -------------------------------- ##

if(ENABLE_TESTS)
  enable_testing()
  message( "${PROJECT_NAME} testing enabled" )
  add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/tests )
endif()

# --------------------------------- Benchmark ----------------------------------

# TODO: https://github.com/jar-git/cmake-template/blob/b7fbb0d465fbbfd0a0e80977d9563f042314bbb3/example_app/CMakeLists.txt#L70
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/bench)

## ---------------------------- Uninstall -------------------------------- ##

if(ENABLE_UNINSTALL)
  # add `uninstall` cmake target
  addinstallManifest() # from cmake_helper_utils (conan package)
endif()

## ---------------------------- various files -------------------------------- ##

# Create a dummy target to hold various files in the project root (for IDE)
if (CONAN_AUTO_INSTALL)
  # NOTE: enable only when project is open in IDE
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/setup_misc_files.cmake)
endif (CONAN_AUTO_INSTALL)
