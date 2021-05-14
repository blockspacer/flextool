flextool_test_gtest(${ROOT_PROJECT_NAME}-gmock "gmock.test.cpp")

flextool_test_gtest(${ROOT_PROJECT_NAME}-i18n "i18n.test.cpp")

# "i18n" is one of test program names
add_custom_command( TARGET ${ROOT_PROJECT_NAME}-i18n POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_CURRENT_SOURCE_DIR}/data
                        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} )

list(APPEND flextool_unittests
  #annotations/asio_guard_annotations_unittest.cc
)
list(APPEND flextool_unittest_utils
  #"allocator/partition_allocator/arm_bti_test_functions.h"
)

list(REMOVE_DUPLICATES flextool_unittests)
list(TRANSFORM flextool_unittests PREPEND ${FLEXTOOL_SOURCES_PATH})

list(REMOVE_DUPLICATES flextool_unittest_utils)
list(FILTER flextool_unittest_utils EXCLUDE REGEX ".*_unittest.cc$")
list(TRANSFORM flextool_unittest_utils PREPEND ${FLEXTOOL_SOURCES_PATH})

foreach(FILEPATH ${flextool_unittests})
  set(test_sources
    "${FILEPATH}"
    ${flextool_unittest_utils}
  )
  list(REMOVE_DUPLICATES flextool_unittest_utils)
  get_filename_component(FILENAME_WITHOUT_EXT ${FILEPATH} NAME_WE)
  flextool_test_gtest(${ROOT_PROJECT_NAME}-flextool-${FILENAME_WITHOUT_EXT}
    "${test_sources}")
endforeach()
