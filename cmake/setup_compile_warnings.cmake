include_guard( DIRECTORY )

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

# # Helper that can set default warning flags for you
target_set_warnings( # from cmake_helper_utils (conan package)
  ${LIB_NAME}
  ENABLE ALL
  AS_ERROR ALL
  DISABLE Annoying)
