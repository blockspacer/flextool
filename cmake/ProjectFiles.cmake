include_guard( DIRECTORY )

list(APPEND flextool_SOURCES
  ${flextool_include_DIR}/command_line_constants.hpp
  ${flextool_src_DIR}/clang_util.cc
  ${flextool_include_DIR}/clang_util.hpp
  ${flextool_src_DIR}/command_line_constants.cc
  ${flextool_include_DIR}/app_cmd_options.hpp
  ${flextool_src_DIR}/app_cmd_options.cc
  ${flextool_src_DIR}/clang_util.cc
  ${flextool_include_DIR}/clang_util.hpp
  ${flextool_src_DIR}/clang_tool.cc
  ${flextool_include_DIR}/clang_tool.hpp
  ${flextool_src_DIR}/cmd_environment.cc
  ${flextool_include_DIR}/cmd_environment.hpp
  ${flextool_src_DIR}/plugin_environment.cc
  ${flextool_include_DIR}/plugin_environment.hpp
  ${flextool_src_DIR}/clang_environment.cc
  ${flextool_include_DIR}/clang_environment.hpp
  ${flextool_src_DIR}/cling_environment.cc
  ${flextool_include_DIR}/cling_environment.hpp
)
