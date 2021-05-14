include_guard( DIRECTORY )

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
    ${FLEXTOOL_LIB_NAME}
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
