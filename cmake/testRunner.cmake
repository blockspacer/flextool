# Load addional modules
include(ProcessorCount)
ProcessorCount(NUM_PROCESSORS)
set(NUM_PROCESSORS ${NUM_PROCESSORS} CACHE STRING "Processor count")
message("CMAKE_CTEST_COMMAND=${CMAKE_CTEST_COMMAND}")

macro(add_test_command_targets target)
  # Run tests with print on failure
  add_custom_target(${target}_check
      ${CMAKE_COMMAND} -E echo CWD=${CMAKE_BINARY_DIR}
      COMMAND ${CMAKE_COMMAND} -E echo CMD=${CMAKE_CTEST_COMMAND} -C $<CONFIG>
      COMMAND ${CMAKE_COMMAND} -E echo ----------------------------------
      COMMAND ${CMAKE_COMMAND} -E env CTEST_OUTPUT_ON_FAILURE=1 ${CMAKE_CTEST_COMMAND}
        --repeat-until-fail 1 # Require each test to run <n> times without failing in order to pass.
        --build-target ${target}
        --target ${target}
        --config $<CONFIG>
      COMMAND ${CMAKE_COMMAND} -E echo ----------------------------------
      #DEPENDS ${target} gloer
  )

  # Run tests with verbose output
  add_custom_target(${target}_check_verbose
      COMMAND ${CMAKE_COMMAND} -E echo ----------------------------------
      COMMAND ${CMAKE_CTEST_COMMAND}
          --repeat-until-fail 1 # Require each test to run <n> times without failing in order to pass.
          --build-target ${target}
          --force-new-ctest-process
          --verbose
          --output-on-failure
          --target ${target}
          --config $<CONFIG>
          --parallel ${NUM_PROCESSORS}
          #--test-action test
      COMMAND ${CMAKE_COMMAND} -E echo ----------------------------------
      #DEPENDS ${target} gloer
  )

  list(APPEND ALL_TESTS_LIST ${target})
  list(APPEND ALL_TESTS_LIST_NORMAL ${target}_check)
  list(APPEND ALL_TESTS_LIST_VERBOSE ${target}_check_verbose)

endmacro()

macro(add_test_autostarter target)
  # Force running test after build (after each change in source files)
  add_custom_command(TARGET ${target}
    POST_BUILD
    COMMAND  ${CMAKE_COMMAND} --build . --target ${target}_check_verbose --config $<CONFIG>
    #DEPENDS ${target} gloer
  )
endmacro()
