include_guard( DIRECTORY )

# CMake-provided scripts
include(CPack)
include(CTest)
include(CheckCXXCompilerFlag)
include(CheckSymbolExists)
include(TestBigEndian)
include(CheckIncludeFile)
include(CheckLibraryExists)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
# Helper module for selecting option based on multiple values. Module help:
# https://cmake.org/cmake/help/v3.10/module/CMakeDependentOption.html
include(CMakeDependentOption)
