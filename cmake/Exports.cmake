include_guard( DIRECTORY )

# NOTE: preserve directory structure
install(
  DIRECTORY
    ${CMAKE_SOURCE_DIR}/include/flextool
  DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
    PATTERN "*")

install(
  TARGETS
    ${FLEXTOOL_LIB_NAME}
  EXPORT
    ${PROJECT_NAME}-config
  DESTINATION
    cmake)

install(
  EXPORT
    ${PROJECT_NAME}-config
  NAMESPACE
    ${PROJECT_NAME}::
  DESTINATION
    cmake)

set_target_properties(${FLEXTOOL_LIB_NAME}
  PROPERTIES
    EXPORT_NAME ${FLEXTOOL_LIB_NAME})

# see Package Configuration File (PCF) https://jfreeman.dev/blog/2019/05/22/trying-conan-with-modern-cmake:-dependencies/
export(
  TARGETS
    ${FLEXTOOL_LIB_NAME}
  NAMESPACE
    ${PROJECT_NAME}::
  FILE
    ${PROJECT_NAME}-config.cmake)

# Register package in user's package registry
export(PACKAGE ${PROJECT_NAME})

install(
  TARGETS
    ${FLEXTOOL_LIB_NAME}
  INCLUDES DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}
  PUBLIC_HEADER DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}
  RUNTIME DESTINATION
    ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION
    ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION
    ${CMAKE_INSTALL_LIBDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION
    ${PROJECT_VERSION}
  COMPATIBILITY
    AnyNewerVersion
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION
    "."
  COMPONENT
    Devel
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Find${PROJECT_NAME}.cmake
 ${CMAKE_CURRENT_BINARY_DIR}/Find${PROJECT_NAME}.cmake
 COPYONLY)
install(FILES
   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Find${PROJECT_NAME}.cmake
   #DESTINATION cmake
   DESTINATION "."
)

install(TARGETS ${FLEXTOOL_LIB_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")

install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/resources" # source directory
  DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" # target directory
)
