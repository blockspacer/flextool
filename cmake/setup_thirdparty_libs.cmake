include_guard( DIRECTORY )

if(NOT TARGET CONAN_PKG::chromium_base)
  message(FATAL_ERROR "Use chromium_base from conan")
endif()

#if(NOT TARGET CONAN_PKG::clang_folly_conan)
#  message(FATAL_ERROR "Use clang_folly_conan from conan")
#endif()

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

message(STATUS "CMAKE_DL_LIBS = ${CMAKE_DL_LIBS}")

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
FROM_HERE("invalid basis_HEADER_DIR")
validate(CHECK_NOT_EMPTY ${basis_HEADER_DIR}
  TEXT "${FROM_HERE}"
)

list(APPEND USED_3DPARTY_LIBS
  ${flexlib}
  ${basis_LIB}
  #TODO: doctest
  #CONAN_PKG::doctest
  #CONAN_PKG::clang_folly_conan
  CONAN_PKG::boost
  CONAN_PKG::openssl
  #CONAN_PKG::double-conversion
  #CONAN_PKG::glog
  #CONAN_PKG::lz4
  #CONAN_PKG::lzma
  #CONAN_PKG::zstd
  #CONAN_PKG::snappy
  #CONAN_PKG::libsodium
  #CONAN_PKG::libdwarf
  #CONAN_PKG::bzip2
  #CONAN_PKG::gflags
  #CONAN_PKG::libunwind
  #CONAN_PKG::libelf
  #CONAN_PKG::xz_utils
  CONAN_PKG::corrade
  Corrade::PluginManager
  ${USED_BOOST_LIBS}
  #boost_outcome
  #microsoft_gsl
)

set(USED_SYSTEM_LIBS
    Threads::Threads # pthread, https://cmake.org/cmake/help/v3.13/module/FindThreads.html
    #${X11_LIBRARIES} # https://cmake.org/cmake/help/v3.13/module/FindX11.html
    #${CMAKE_DL_LIBS} # https://cmake.org/cmake/help/v3.13/variable/CMAKE_DL_LIBS.html
    #EXPAT::EXPAT # https://cmake.org/cmake/help/v3.13/module/FindEXPAT.html
    #ZLIB::ZLIB # https://cmake.org/cmake/help/v3.13/module/FindZLIB.html
               # Custom libs
    stdc++fs # C++17 std::filesystem
    # @note: Order matters https://stackoverflow.com/a/10269201/10904212
    #${LIBIBERTY_LIBRARY} # used by folly
    #${DOUBLE_CONVERSION_LIBRARY} # used by folly
    #${LIBEVENT_LIB} # used by folly
    #${LZ4_LIBRARY} # used by folly
    #${LIBUNWIND_LIBRARIES} # used by folly
    #${LIBLZMA_LIBRARIES} # used by folly
    CACHE INTERNAL "USED_SYSTEM_LIBS")

message(STATUS "flextool USED_3DPARTY_LIBS=${USED_3DPARTY_LIBS}")

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

set(corrade_includes
  ${CONAN_CORRADE_ROOT}/include
)
message(STATUS "corrade_includes=${corrade_includes}")

set(entt_includes
  ${CONAN_ENTT_ROOT}/include
)
message(STATUS "entt_includes=${entt_includes}")
