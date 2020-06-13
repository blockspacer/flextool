include_guard( DIRECTORY )

# Create a dummy target to hold various files in the project root (for IDE)
add_custom_target(RootFiles SOURCES
  LICENSE
  README.md
  VERSION
  conanfile.py)
