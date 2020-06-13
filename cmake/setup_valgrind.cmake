include_guard( DIRECTORY )

if(ENABLE_VALGRIND)
  check_valgrind_config()

  if(cmake_build_type_tolower MATCHES "release" )
    message(WARNING "Disable valgrind in Release builds")
  endif()
endif()
