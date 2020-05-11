include_guard( DIRECTORY )

list(APPEND flextool_SOURCES
  ${flextool_include_DIR}/PluginManager.hpp
  ${flextool_src_DIR}/PluginManager.cc
  ${flextool_include_DIR}/command_line_constants.hpp
  ${flextool_src_DIR}/clang_util.cc
  ${flextool_include_DIR}/clang_util.hpp
  ${flextool_src_DIR}/path_provider.cc
  ${flextool_include_DIR}/path_provider.hpp
  ${flextool_src_DIR}/command_line_constants.cc
  ${flextool_include_DIR}/boost_command_line.hpp
  ${flextool_src_DIR}/boost_command_line.cc
  ${flextool_include_DIR}/app_cmd_options.hpp
  ${flextool_src_DIR}/app_cmd_options.cc
  ${flextool_src_DIR}/clang_util.cc
  ${flextool_include_DIR}/clang_util.hpp
  ${flextool_src_DIR}/clang_tool.cc
  ${flextool_include_DIR}/clang_tool.hpp
  ${flextool_src_DIR}/app_environment.cc
  ${flextool_include_DIR}/app_environment.hpp
)
