include_guard( DIRECTORY )

list(APPEND ClangErrorFlags
  -Werror=thread-safety
  -Werror=thread-safety-analysis
)

list(APPEND GCCAndClangErrorFlags
  -Werror=float-equal
  -Werror=missing-braces
  -Werror=init-self
  -Werror=logical-op
  -Werror=write-strings
  -Werror=address
  -Werror=array-bounds
  -Werror=char-subscripts
  -Werror=enum-compare
  -Werror=implicit-int
  -Werror=empty-body
  -Werror=main
  -Werror=aggressive-loop-optimizations
  -Werror=nonnull
  -Werror=parentheses
  -Werror=pointer-sign
  -Werror=return-type
  -Werror=sequence-point
  -Werror=uninitialized
  -Werror=volatile-register-var
  -Werror=ignored-qualifiers
  -Werror=missing-parameter-type
  -Werror=old-style-declaration
  -Werror=sign-compare
  -Werror=conditional-uninitialized
  -Werror=date-time
  -Werror=switch
)

list(APPEND GCCAndClangWarningFlags
  -Wrange-loop-analysis
  -Wvla
  -Wswitch
  -Wformat-security
  -Wredundant-decls
  -Wunused-variable
  -Wdate-time
  -Wconditional-uninitialized
  -Wsign-compare
  -Wsuggest-override
  -Wunused-parameter
  -Wself-assign
  -Wunused-local-typedef
  -Wimplicit-fallthrough
  -Wdeprecated-copy
  # disable warning: designated initializers are a C99 feature
  -Wno-c99-extensions
  -Wno-error=c99-extensions
  # see https://kristerw.blogspot.com/2017/09/useful-gcc-warning-options-not-enabled.html
  -Wduplicated-cond
  # see https://kristerw.blogspot.com/2017/09/useful-gcc-warning-options-not-enabled.html
  -Wduplicated-branches
  # see https://kristerw.blogspot.com/2017/09/useful-gcc-warning-options-not-enabled.html
  -Wlogical-op
  # see https://kristerw.blogspot.com/2017/09/useful-gcc-warning-options-not-enabled.html
  -Wrestrict
  # see https://kristerw.blogspot.com/2017/09/useful-gcc-warning-options-not-enabled.html
  -Wnull-dereference
  # see https://kristerw.blogspot.com/2017/09/useful-gcc-warning-options-not-enabled.html
  -Wold-style-cast
  # see https://kristerw.blogspot.com/2017/09/useful-gcc-warning-options-not-enabled.html
  -Wuseless-cast
  # see https://kristerw.blogspot.com/2017/09/useful-gcc-warning-options-not-enabled.html
  -Wjump-misses-init
  # see https://kristerw.blogspot.com/2017/09/useful-gcc-warning-options-not-enabled.html
  -Wdouble-promotion
  # see https://kristerw.blogspot.com/2017/09/useful-gcc-warning-options-not-enabled.html
  -Wshadow
  # see https://kristerw.blogspot.com/2017/09/useful-gcc-warning-options-not-enabled.html
  -Wformat=2
)

# -ftrapv generates traps for signed overflow on addition, subtraction, multiplication operations
# see https://clang.llvm.org/docs/ClangCommandLineReference.html#cmdoption-clang-ftrapv
# NOTE: ftrapv affects performance, so use only in debug builds
if (cmake_build_type_tolower MATCHES "debug" )
  check_cxx_compiler_flag(-ftrapv CC_HAS_FTRAPV)
  if(CC_HAS_FTRAPV)
    target_compile_options(${FLEXTOOL_LIB_NAME} PRIVATE
      # NOTE: SIGABRT to be raised
      # that will normally abort your program on overflow
      -ftrapv
    )
  endif()
endif()

target_compile_options(${FLEXTOOL_LIB_NAME} PRIVATE
  # NOTE: explicitly select the "C++ Core Check Lifetime Rules" (or "Microsoft All Rules") in order to enable the lifetime checks.
  # see https://devblogs.microsoft.com/cppblog/c-core-guidelines-checker-in-visual-studio-2017/
  # see https://www.modernescpp.com/index.php/c-core-guidelines-lifetime-safety
  $<$<CXX_COMPILER_ID:MSVC>:
    /W3 # Set warning level
    /Wall
    /analyze
  >
  # see https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
  $<$<CXX_COMPILER_ID:GNU>:
    ${GCCAndClangErrorFlags}
    ${GCCAndClangWarningFlags}
    -Wall
    -W
    -Wextra
    -Wpedantic
    -Wdeprecated-register
    -Wnon-virtual-dtor
  >
  $<$<CXX_COMPILER_ID:Clang>:
    ${GCCAndClangErrorFlags}
    ${GCCAndClangWarningFlags}
    ${ClangErrorFlags}
    # see https://pspdfkit.com/blog/2020/the-cpp-lifetime-profile/
    # TODO: only special branch of Clang currently https://github.com/mgehre/llvm-project
    #-Wlifetime
    # see http://clang.llvm.org/docs/ThreadSafetyAnalysis.html
    # see https://github.com/isocpp/CppCoreGuidelines/blob/master/docs/Lifetime.pdf
    -Wthread-safety-analysis
    -Wall
    -W
    -Wextra
    -Wpedantic
    -Wdeprecated-register
    -Wnon-virtual-dtor
    # Negative requirements are an experimental feature
    # which will produce many warnings in existing code
    -Wno-thread-safety-negative
  >
)

# # Helper that can set default warning flags for you
target_set_warnings( # from cmake_helper_utils (conan package)
  ${FLEXTOOL_LIB_NAME}
  ENABLE ALL
  DISABLE Annoying)
