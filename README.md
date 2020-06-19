&nbsp;
<p align="center">
  <a href="https://blockspacer.github.io/flex_docs">
    <img src="https://blockspacer.github.io/flex_docs/images/logo.png" width="100px" alt="flextool" />
  </a>
</p>
<h3 align="center">Add custom features to C++ language, like <a href="https://www.fluentcpp.com/2018/03/09/c-metaclasses-proposal-less-5-minutes/">metaclasses</a>, <a href="https://twitter.com/TartanLlama/status/1159457033441165313">Rust-like traits</a>, reflection and many more</h3>
<p align="center">A fully open source, powerful solution for modification and generation of C++ source code. Reduce the amount of boilerplate code in your C++ projects.</p>
<hr />

![Open Source Love](https://img.shields.io/badge/Open%20Source-%E2%9D%A4-pink.svg)
![First Timers Only](https://img.shields.io/badge/first--timers--only-friendly-blue.svg?style=flat)
![Up For Grabs](https://img.shields.io/badge/up--for--grabs-friendly-green.svg?style=flat)
![GitHub](https://img.shields.io/github/license/blockspacer/flextool.svg)
![GitHub forks](https://img.shields.io/github/forks/blockspacer/flextool.svg)
![GitHub issues](https://img.shields.io/github/issues/blockspacer/flextool.svg)
![GitHub pull requests](https://img.shields.io/github/issues-pr/blockspacer/flextool.svg)
![GitHub contributors](https://img.shields.io/github/contributors/blockspacer/flextool.svg)
![GitHub commit activity the past week, 4 weeks, year](https://img.shields.io/github/commit-activity/w/blockspacer/flextool.svg)
![GitHub last commit](https://img.shields.io/github/last-commit/blockspacer/flextool.svg)
![GitHub top language](https://img.shields.io/github/languages/top/blockspacer/flextool.svg)
![GitHub language count](https://img.shields.io/github/languages/count/blockspacer/flextool.svg)
[![Project Status: WIP - Initial development is in progress, but there has not yet been a stable, usable release suitable for the public.](http://www.repostatus.org/badges/latest/wip.svg)](http://www.repostatus.org/#wip)
[![license](https://img.shields.io/github/license/blockspacer/flextool.svg?style=flat-square)](https://github.com/blockspacer/flextool/master/LICENSE)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/bsamseth/cpp-project.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/blockspacer/flextool/alerts/)
[![Lines of Code](https://tokei.rs/b1/github/blockspacer/flextool)](https://github.com/blockspacer/flextool)
[![Average time to resolve an issue](http://isitmaintained.com/badge/resolution/blockspacer/flextool.svg)](http://isitmaintained.com/project/blockspacer/flextool "Average time to resolve an issue")
[![Percentage of issues still open](http://isitmaintained.com/badge/open/blockspacer/flextool.svg)](http://isitmaintained.com/project/blockspacer/flextool "Percentage of issues still open")

Main project page: https://blockspacer.github.io/flex_docs/

## Tutorials and examples

- [Tutorial](https://blockspacer.github.io/flex_docs/tutorial/)
- [Usage](https://blockspacer.github.io/flex_docs/building_projects/)
- [Plugins](https://blockspacer.github.io/flex_docs/adding_plugins/)
- [Tracing and log levels](https://blockspacer.github.io/flex_docs/bug_report/)

## Before installation

- [installation guide](https://blockspacer.github.io/flex_docs/download/)

- conan packages

NOTE: cling with LLVM build may take couple of hours.

We use `buildConanThirdparty.cmake` script to download and install conan packages.

```bash
# NOTE: don't forget to re-run `conan install` or `conan create` after command below
# NOTE: change `build_type=Debug` to `build_type=Release` in production
cmake -DEXTRA_CONAN_OPTS="--profile;clang;-s;build_type=Debug;-s;cling_conan:build_type=Release;--build;missing" -P tools/buildConanThirdparty.cmake
```

- llvm_tools package

NOTE: `llvm_tools` package is optional, you can skip it using `enable_llvm_tools=False` like so: `-e flextool:enable_llvm_tools=False -e basis:enable_llvm_tools=False -e chromium_base:enable_llvm_tools=False`

NOTE: LLVM build may take couple of hours.

You can install `llvm_tools` like so:

```bash
git clone https://github.com/blockspacer/llvm_tools.git
cd llvm_tools
conan create . conan/stable -s build_type=Release --profile clang --build missing
```

See for up-to-date instructions [https://github.com/blockspacer/llvm_tools](https://github.com/blockspacer/llvm_tools)

## Installation

Command below uses `--profile clang`. Before creation of conan profile file see https://docs.conan.io/en/latest/using_packages/using_profiles.html

```bash
# Tested with clang 6.0
export CXX=clang++-6.0
export CC=clang-6.0

# NOTE: change `build_type=Debug` to `build_type=Release` in production
# NOTE: use --build=missing if you got error `ERROR: Missing prebuilt package`
CONAN_REVISIONS_ENABLED=1 \
CONAN_VERBOSE_TRACEBACK=1 \
CONAN_PRINT_RUN_COMMANDS=1 \
CONAN_LOGGING_LEVEL=10 \
GIT_SSL_NO_VERIFY=true \
    cmake -E time \
      conan create . conan/stable \
      -s build_type=Debug \
      -s cling_conan:build_type=Release \
      -s llvm_tools:build_type=Release \
      --profile clang \
          -e flextool:enable_tests=True \
          -e flextool:enable_llvm_tools=True
```

## Standing on the Shoulders of Giants

That project possible because of [flexferrum's `autoprogrammer`](https://github.com/flexferrum/autoprogrammer).

Articles about flexferrum's `autoprogrammer` in media:

- [RUS] (article) metaclasses https://habr.com/ru/article/448466/
- [RUS] (slides) C++17 metaclasses https://assets.ctfassets.net/oxjq45e8ilak/55bGdX2PnYzmrpM8rwCjcE/791e7eee3236c2023e86e169faca8a0e/Sergei_Sadovnikov_Metaclasses_in_C___dream_Reality.pdf

## For contibutors: conan editable mode

With the editable packages, you can tell Conan where to find the headers and the artifacts ready for consumption in your local working directory.
There is no need to run `conan create` or `conan export-pkg`.

See for details [https://docs.conan.io/en/latest/developing_packages/editable_packages.html](https://docs.conan.io/en/latest/developing_packages/editable_packages.html)

Build locally:

```bash
CONAN_REVISIONS_ENABLED=1 \
CONAN_VERBOSE_TRACEBACK=1 \
CONAN_PRINT_RUN_COMMANDS=1 \
CONAN_LOGGING_LEVEL=10 \
GIT_SSL_NO_VERIFY=true \
  cmake -E time \
    conan install . \
    --install-folder local_build \
    -s build_type=Debug \
    -s cling_conan:build_type=Release \
    -s llvm_tools:build_type=Release \
    --profile clang \
        -e flextool:enable_tests=True \
        -e flextool:enable_llvm_tools=True

CONAN_REVISIONS_ENABLED=1 \
CONAN_VERBOSE_TRACEBACK=1 \
CONAN_PRINT_RUN_COMMANDS=1 \
CONAN_LOGGING_LEVEL=10 \
GIT_SSL_NO_VERIFY=true \
  cmake -E time \
    conan source . --source-folder local_build

conan build . \
  --build-folder local_build \
  --source-folder local_build

conan package . \
  --build-folder local_build \
  --package-folder local_build/package_dir \
  --source-folder local_build
```

Set package to editable mode:

```bash
conan editable add local_build/package_dir \
  flextool/master@conan/stable
```

Note that `conanfile.py` modified to detect local builds via `self.in_local_cache`

After change source in folder local_build (run commands in source package folder):

```
conan build . \
  --build-folder local_build \
  --source-folder local_build

conan package . \
  --build-folder local_build \
  --package-folder local_build/package_dir \
  --source-folder local_build
```

Build your test project

In order to revert the editable mode just remove the link using:

```bash
conan editable remove \
  flextool/master@conan/stable
```

## For contibutors: conan workspace

Allows to build multiple projects at once, it just creates `CMakeLists.txt` with `add_subdirectory` pointing to each package folder.

NOTE: You can open workspace in IDE as usual CMake based project (change build directory to WorkspaceProject path)!

See for details [https://docs.conan.io/en/latest/developing_packages/workspaces.html](https://docs.conan.io/en/latest/developing_packages/workspaces.html)

For example, we want to build both flextool and flexlib at the same time (flextool requires flexlib).

```bash
# change ~ to desired build folder
cd ~

# Replace paths to yours!
# Make sure each project in NOT in editable mode!
cat <<EOF > ~/conanws.yml
editables:
    flexlib/master@conan/stable:
        path: /......../flexlib
    flextool/master@conan/stable:
        path: /......../flextool
layout: layout_flex
workspace_generator: cmake
root:
    - flextool/master@conan/stable
EOF

cat <<EOF > ~/layout_flex
# This helps to define the location of CMakeLists.txt within package
[source_folder]
.

# This defines where the conanbuildinfo.cmake will be written to
[build_folder]
build/{{settings.build_type}}
EOF
```

```bash
# Tested with clang 6.0
export CXX=clang++-6.0
export CC=clang-6.0

mkdir build_flex

cd build_flex

cat <<EOF > CMakeLists.txt
cmake_minimum_required(VERSION 3.0)

project(WorkspaceProject)

set(flexlib_LOCAL_BUILD TRUE CACHE BOOL "flexlib_LOCAL_BUILD")

# path to Find*.cmake file
list(APPEND CMAKE_MODULE_PATH /........./flexlib/cmake)

set(flextool_LOCAL_BUILD TRUE CACHE BOOL "flextool_LOCAL_BUILD")

# path to Find*.cmake file
list(APPEND CMAKE_MODULE_PATH /........./flextool/cmake)

include(\${CMAKE_BINARY_DIR}/conanworkspace.cmake)
conan_workspace_subdirectories()

add_dependencies(flextool flexlib)
EOF

# must contain `include(${CMAKE_BINARY_DIR}/conanworkspace.cmake)` without `\`
cat CMakeLists.txt

# combines options from all projects
conan workspace install \
  ../conanws.yml --profile=clang \
  -s build_type=Debug \
  -s cling_conan:build_type=Release \
  -s llvm_tools:build_type=Release \
    -o openssl:shared=True \
    -e basis:enable_tests=True \
    -o chromium_base:use_alloc_shim=True \
    -o chromium_tcmalloc:use_alloc_shim=True \
    -e flextool:enable_tests=True \
    -e flextool:enable_llvm_tools=True \
    -o flexlib:shared=False \
    -o flexlib:enable_clang_from_conan=False \
    -e flexlib:enable_tests=True
```

Build into folder created by `conan workspace install`:

```bash
export CXX=clang++-6.0
export CC=clang-6.0

# NOTE: change `build_type=Debug` to `build_type=Release` in production
build_type=Debug

# optional
# rm CMakeCache.txt

# configure via cmake
cmake -E time cmake . \
  -DENABLE_TESTS=TRUE \
  -DBUILD_SHARED_LIBS=FALSE \
  -DCONAN_AUTO_INSTALL=OFF \
  -DCMAKE_BUILD_TYPE=${build_type}

# remove generated files
# change paths to yours
# rm ~/flex_typeclass_plugin/build/Debug/*generated*

# build code
cmake -E time cmake --build . \
  --config ${build_type} \
  -- -j8

# run unit tests for flexlib
cmake -E time cmake --build . \
  --config ${build_type} \
  --target flexlib_run_all_tests

# run unit tests for flextool
cmake -E time cmake --build . \
  --config ${build_type} \
  --target flextool_run_all_tests
```

Workspace allows to make quick changes in existing source files.

We use `self.in_local_cache` to detect conan editable mode:

```python
# Local build
# see https://docs.conan.io/en/latest/developing_packages/editable_packages.html
if not self.in_local_cache:
    self.copy("conanfile.py", dst=".", keep_path=False)
```

Make sure that all targets have globally unique names.

For example, you can not have in each project target with same name like "test". You can solve that issue by adding project-specific prefix to name of each target like "${ROOT_PROJECT_NAME}-test_main_gtest".

Because `CMAKE_BINARY_DIR` will point to folder created by `conan workspace install` - make sure that you prefer `CMAKE_CURRENT_BINARY_DIR` to `CMAKE_BINARY_DIR` etc.

## For contibutors: conan workspace with plugins

add plugins to yml file:

```yml
editables:
    basis/master@conan/stable:
        path: /........./basis
    flex_support_headers/master@conan/stable:
        path: /........./flex_support_headers
    flexlib/master@conan/stable:
        path: /........./flexlib
    flextool/master@conan/stable:
        path: /........./flextool
    flex_reflect_plugin/master@conan/stable:
        path: /........./flex_reflect_plugin
    squarets/master@conan/stable:
        path: /........./squarets
    flex_squarets_plugin/master@conan/stable:
        path: /........./flex_squarets_plugin
    flex_typeclass_plugin/master@conan/stable:
        path: /........./flex_typeclass_plugin
    flex_pimpl_plugin/master@conan/stable:
        path: /........./flex_pimpl_plugin
    flex_meta_plugin/master@conan/stable:
        path: /........./flex_meta_plugin
    flex_meta_demo/master@conan/stable:
        path: /........./flex_meta_demo
layout: layout_flex
workspace_generator: cmake
root:
    - flex_reflect_plugin/master@conan/stable
    - squarets/master@conan/stable
    - flex_squarets_plugin/master@conan/stable
    - flex_typeclass_plugin/master@conan/stable
    - flex_pimpl_plugin/master@conan/stable
    - flex_meta_plugin/master@conan/stable
    - flex_meta_demo/master@conan/stable
```

Use `add_dependencies` in `CMakeLists.txt`:

```bash
# Tested with clang 6.0
export CXX=clang++-6.0
export CC=clang-6.0

mkdir build_flex

cd build_flex

cat <<EOF > CMakeLists.txt
cmake_minimum_required(VERSION 3.0)

project(WorkspaceProject)

set(basis_LOCAL_BUILD TRUE CACHE BOOL "basis_LOCAL_BUILD")

# path to Find*.cmake file
list(APPEND CMAKE_MODULE_PATH /........./basis/cmake)

set(flex_support_headers_LOCAL_BUILD TRUE CACHE BOOL "flex_support_headers_LOCAL_BUILD")

# path to Find*.cmake file
list(APPEND CMAKE_MODULE_PATH /........./flex_support_headers/cmake)

set(flexlib_LOCAL_BUILD TRUE CACHE BOOL "flexlib_LOCAL_BUILD")

# path to Find*.cmake file
list(APPEND CMAKE_MODULE_PATH /........./flexlib/cmake)

set(flextool_LOCAL_BUILD TRUE CACHE BOOL "flextool_LOCAL_BUILD")

# path to Find*.cmake file
list(APPEND CMAKE_MODULE_PATH /........./flextool/cmake)

set(flex_reflect_plugin_LOCAL_BUILD TRUE CACHE BOOL "flex_reflect_plugin_LOCAL_BUILD")

# path to Find*.cmake file
list(APPEND CMAKE_MODULE_PATH /........./flex_reflect_plugin/cmake)

set(squarets_LOCAL_BUILD TRUE CACHE BOOL "squarets_LOCAL_BUILD")

# path to Find*.cmake file
list(APPEND CMAKE_MODULE_PATH /........./squarets/cmake)

set(flex_squarets_plugin_LOCAL_BUILD TRUE CACHE BOOL "flex_squarets_plugin_LOCAL_BUILD")

# path to Find*.cmake file
list(APPEND CMAKE_MODULE_PATH /........./flex_squarets_plugin/cmake)

set(flex_typeclass_plugin_LOCAL_BUILD TRUE CACHE BOOL "flex_typeclass_plugin_LOCAL_BUILD")

# path to Find*.cmake file
list(APPEND CMAKE_MODULE_PATH /........./flex_typeclass_plugin/cmake)

set(flex_meta_plugin_LOCAL_BUILD TRUE CACHE BOOL "flex_meta_plugin_LOCAL_BUILD")

# path to Find*.cmake file
list(APPEND CMAKE_MODULE_PATH /........./flex_meta_plugin/cmake)

set(flex_meta_demo_LOCAL_BUILD TRUE CACHE BOOL "flex_meta_demo_LOCAL_BUILD")

# path to Find*.cmake file
list(APPEND CMAKE_MODULE_PATH /........./flex_meta_demo/cmake)

set(flex_pimpl_plugin_LOCAL_BUILD TRUE CACHE BOOL "flex_pimpl_plugin_LOCAL_BUILD")

# path to Find*.cmake file
list(APPEND CMAKE_MODULE_PATH /........./flex_pimpl_plugin/cmake)

include(${CMAKE_BINARY_DIR}/conanworkspace.cmake)
conan_workspace_subdirectories()

add_dependencies(flexlib basis)
add_dependencies(flextool flexlib basis flex_support_headers)
add_dependencies(flex_reflect_plugin flextool)
add_dependencies(flex_squarets_plugin squarets)
add_dependencies(flex_squarets_plugin flextool)
add_dependencies(flex_pimpl_plugin flextool)
add_dependencies(flex_pimpl_plugin flex_reflect_plugin)
add_dependencies(flex_pimpl_plugin flex_squarets_plugin)
add_dependencies(flex_typeclass_plugin flextool)
add_dependencies(flex_typeclass_plugin flex_squarets_plugin)
add_dependencies(flex_meta_demo flex_meta_plugin)
add_dependencies(flex_meta_demo flex_typeclass_plugin)
add_dependencies(flex_meta_demo flex_pimpl_plugin)
add_dependencies(flex_meta_demo flex_squarets_plugin)
EOF
```

add plugins options to `conan workspace install`:

```bash
# combines options from all projects
conan workspace install \
  ../conanws.yml --profile=clang \
  -s build_type=Debug \
    -s cling_conan:build_type=Release \
    -s llvm_tools:build_type=Release \
    -e basis:enable_tests=True \
    -o openssl:shared=True \
    -o chromium_base:use_alloc_shim=True \
    -o chromium_tcmalloc:use_alloc_shim=True \
    -e flextool:enable_tests=True \
    -e flextool:enable_llvm_tools=True \
    -o flexlib:shared=False \
    -o flexlib:enable_clang_from_conan=False \
    -e flexlib:enable_tests=True \
    -o flex_reflect_plugin:shared=True \
    -o flex_reflect_plugin:enable_clang_from_conan=False \
    -e flex_reflect_plugin:enable_tests=True \
    -o squarets:shared=False \
    -o squarets:enable_clang_from_conan=False \
    -e squarets:enable_tests=True \
    -o flex_squarets_plugin:shared=False \
    -o flex_squarets_plugin:enable_clang_from_conan=False \
    -e flex_squarets_plugin:enable_tests=True \
    -o flex_meta_plugin:shared=False \
    -o flex_meta_plugin:enable_clang_from_conan=False \
    -e flex_meta_plugin:enable_tests=True \
    -o flex_typeclass_plugin:shared=False \
    -o flex_typeclass_plugin:enable_clang_from_conan=False \
    -e flex_typeclass_plugin:enable_tests=True \
    -o flex_pimpl_plugin:shared=False \
    -o flex_pimpl_plugin:enable_clang_from_conan=False \
    -e flex_pimpl_plugin:enable_tests=True \
    -o flex_meta_demo:enable_clang_from_conan=False \
    -e flex_meta_demo:enable_tests=True
```

build and test workspace

```bash
export CXX=clang++-6.0
export CC=clang-6.0

# NOTE: change `build_type=Debug` to `build_type=Release` in production
build_type=Debug

# optional
# rm CMakeCache.txt

# configure via cmake
cmake -E time cmake . \
  -DENABLE_TESTS=TRUE \
  -DBUILD_SHARED_LIBS=FALSE \
  -DCONAN_AUTO_INSTALL=OFF \
  -DCMAKE_BUILD_TYPE=${build_type}

# remove generated files
# change paths to yours
# rm ~/flex_typeclass_plugin/build/Debug/*generated*

# build code
cmake -E time cmake --build . \
  --config ${build_type} \
  -- -j8

# run unit tests for flexlib
cmake -E time cmake --build . \
  --config ${build_type} \
  --target flexlib_run_all_tests

# run unit tests for flextool
cmake -E time cmake --build . \
  --config ${build_type} \
  --target flextool_run_all_tests

# run unit tests for flex_reflect_plugin
cmake -E time cmake --build . \
  --config ${build_type} \
  --target flex_reflect_plugin_run_all_tests

# run unit tests for squarets
cmake -E time cmake --build . \
  --config ${build_type} \
  --target squarets_run_all_tests

# run unit tests for flex_squarets_plugin
cmake -E time cmake --build . \
  --config ${build_type} \
  --target flex_squarets_plugin_run_all_tests

# run unit tests for flex_squarets_plugin
cmake -E time cmake --build . \
  --config ${build_type} \
  --target flex_meta_plugin_run_all_tests

# run unit tests for flex_squarets_plugin
cmake -E time cmake --build . \
  --config ${build_type} \
  --target flex_typeclass_plugin_run_all_tests

# run unit tests for flex_squarets_plugin
cmake -E time cmake --build . \
  --config ${build_type} \
  --target flex_pimpl_plugin_run_all_tests

# run unit tests for flex_squarets_plugin
cmake -E time cmake --build . \
  --config ${build_type} \
  --target flex_meta_demo_run_all_tests
```

## For contibutors: cppcheck

Make sure you use `Debug` build with `-e flextool:enable_llvm_tools=True`

Install cppcheck via conan:

```bash
cd /tmp

git clone -b testing/1.90 https://github.com/bincrafters/conan-cppcheck_installer.git

cd conan-cppcheck_installer

# NOTE: change `build_type=Debug` to `build_type=Release` in production
# NOTE: use --build=missing if you got error `ERROR: Missing prebuilt package`
CONAN_REVISIONS_ENABLED=1 \
CONAN_VERBOSE_TRACEBACK=1 \
CONAN_PRINT_RUN_COMMANDS=1 \
CONAN_LOGGING_LEVEL=10 \
GIT_SSL_NO_VERIFY=true \
    cmake -E time \
      conan create . conan/stable \
      -s build_type=Release
```

Usage (runs cmake with `-DENABLE_CPPCHECK=ON`):

```bash
# NOTE: -DCLEAN_OLD="OFF" to keep generated html report
cmake -DCPPCHECK="ON" -DCLEAN_OLD="OFF" -P tools/run_tool.cmake
```

```bash
# `index.html` must exist
# find $PWD -name index.html
```

Open 'index.html' to see the results.

## For contibutors: vue ui

Desktop application that provides a single context to manage and run multiple scripts.

Simplifies setup of local dev environment. Allows to avoid opening a lot of shell terminals.

You can add custom commands in `package.json` or `plugin/vue-cli-plugin/ui.js`

```bash
# Before: Install Latest Node.js and NPM
# see https://tecadmin.net/install-latest-nodejs-npm-on-ubuntu/
node --version
npm --version
```

Build your project via npm:

```bash
npm install
```

```bash
sudo npm install -g @vue/cli
sudo vue ui --dev --port 8061
```

Open `http://localhost:8061/`

Import project directory.

Select `Tasks`, like build/test...

Tasks may be changed in `package.json`

NOTE: We use `package.json` only for tasks. Conan is controlled by a file called conanfile.txt.

Useful links:

- https://cli.vuejs.org/dev-guide/ui-api.html#ui-files
- https://github.com/Dewep/GCE
- https://github.com/supnate/command-pad

## For contibutors: gdbinit

We use `.gdbinit` add 'add-auto-load-safe-path .'

Read for more details about `.gdbinit` [https://metricpanda.com/tips-for-productive-debugging-with-gdb](https://metricpanda.com/tips-for-productive-debugging-with-gdb)

Read for more details about `gdb` [http://www.yolinux.com/TUTORIALS/GDB-Commands.html](http://www.yolinux.com/TUTORIALS/GDB-Commands.html)

GDB debugging session can be automated like so:

```bash
# see https://gist.github.com/williballenthin/8bd6e29ad8504b9cb308039f675ee889
gdb \
  -ex "run" \
  -ex "set pagination off" \
  -ex "bt" \
  -ex "set confirm off" \
  -ex "quit" \
  --args \
  ${APP_EXE} \
  ${APP_CMD_ARGS}
```

## For contibutors: valgrind

Make sure you use `Debug` build with `-e flextool:enable_llvm_tools=True`

See for details:

* [https://www.jetbrains.com/help/clion/memory-profiling-with-valgrind.html](https://www.jetbrains.com/help/clion/memory-profiling-with-valgrind.html)
* [https://heeris.id.au/2016/valgrind-gdb/](https://heeris.id.au/2016/valgrind-gdb/)

Install valgrind:

```bash
sudo apt install valgrind  # Ubuntu, Debian, etc.
# OR
sudo yum install valgrind  # RHEL, CentOS, Fedora, etc.
```

NOTE: make sure you set `use_alloc_shim=False` and `enable_valgrind=True` (see below)

Run valgrind via cmake:

```bash
# NOTE: set `use_alloc_shim=False` and `enable_valgrind=True` for valgrind support
CONAN_REVISIONS_ENABLED=1 \
CONAN_VERBOSE_TRACEBACK=1 \
CONAN_PRINT_RUN_COMMANDS=1 \
CONAN_LOGGING_LEVEL=10 \
GIT_SSL_NO_VERIFY=true \
  cmake -E time \
    conan install . \
    --install-folder local_build \
    -s build_type=Debug \
    -s cling_conan:build_type=Release \
    -s llvm_tools:build_type=Release \
    --profile clang \
        -o flextool:enable_valgrind=True \
        -e flextool:enable_tests=True \
        -e flextool:enable_llvm_tools=True \
        -o chromium_base:enable_valgrind=True \
        -e chromium_base:enable_llvm_tools=True \
        -o chromium_base:use_alloc_shim=False \
        -o basis:enable_valgrind=True \
        -e basis:enable_llvm_tools=True \
        -o flexlib:enable_valgrind=True \
        -e flexlib:enable_llvm_tools=True \
        -o flexlib:enable_clang_from_conan=False \
        -o chromium_tcmalloc:use_alloc_shim=False \
        --build chromium_base \
        --build chromium_tcmalloc \
        --build basis \
        --build flexlib

cd ~/flextool

# see section about `conan editable mode`
cd local_build

# remove old CMakeCache
(rm CMakeCache.txt || true)

# remove old build artifacts
rm -rf flextool
rm -rf bin
find . -iname '*.o' -exec rm {} \;
find . -iname '*.a' -exec rm {} \;
find . -iname '*.dll' -exec rm {} \;
find . -iname '*.lib' -exec rm {} \;

# NOTE: -DENABLE_VALGRIND=ON
cmake .. \
  -DENABLE_VALGRIND=ON \
  -DENABLE_TESTS=TRUE \
  -DENABLE_VALGRIND_TESTS=TRUE \
  -DBUILD_SHARED_LIBS=FALSE \
  -DCONAN_AUTO_INSTALL=OFF \
  -DCMAKE_BUILD_TYPE=Debug

# NOTE: to run some tests under valgrind
# use `-DENABLE_VALGRIND_TESTS=TRUE`
cmake -E time cmake --build . \
  --target flextool-gmock_run_valgrind

# Cmake will print valgrind command that was executed.
# You can copy executed command and add custom command-line arguments:
#   --gtest_filter=ToolsSanityTest.DISABLED_ValgrindTest \
#   --gtest_also_run_disabled_tests

# search for valgrind log file
find $PWD -name *valgrind*.log
```

To find leaks you can seach for `definitely lost` in log file.

NOTE: you can add valgrind suppressions in `cmake/valgrind.cfg`

NOTE: compile program with a debug flag to run under valgrind

NOTE: use `valgrind --tool=helgrind` to detect potential deadlocks and data races

NOTE: use `valgrind --tool=massif --massif-out-file=massif_file --stacks=true` to measure size of heap. See also https://kde.org/applications/development/org.kde.massif-visualizer

See for details https://stackoverflow.com/a/44989219

TODO: try to build with clang 10 https://stackoverflow.com/questions/40509986/valgrind-reporting-mismatched-free-delete-delete

TODO: valgrind may not support chromium base, FIXME. And remove GTEST_NO_SUITE

## For contibutors: clang-tidy

Make sure you use `Debug` build with `-e flextool:enable_llvm_tools=True`

Install clang-tidy:

```bash
sudo apt-get install clang-tidy  # Ubuntu, Debian, etc.
# OR
sudo yum install clang-tidy  # RHEL, CentOS, Fedora, etc.
```

Usage (runs cmake with `-DENABLE_CLANG_TIDY=ON`):

```bash
cmake -DCLANG_TIDY="ON" -DCLEAN_OLD="ON" -P tools/run_tool.cmake
```

## For contibutors: scan-build

Make sure you use `Debug` build with `-e flextool:enable_llvm_tools=True`

See for details [https://chromium.googlesource.com/chromium/src.git/+/master/docs/clang_static_analyzer.md](https://chromium.googlesource.com/chromium/src.git/+/master/docs/clang_static_analyzer.md) and [https://clang-analyzer.llvm.org/scan-build.html](https://clang-analyzer.llvm.org/scan-build.html)

The program ccc-analyzer acts like a fake compiler, forwarding its command line arguments over to the compiler to perform regular compilation and clang to perform static analysis.

Running configure typically generates makefiles that have hardwired paths to the compiler, and by running configure through scan-build that path is set to ccc-analyzer.

```bash
# must exist
ccc-analyzer -v

# must exist
c++-analyzer -v

# must exist
scan-build -v

CONAN_REVISIONS_ENABLED=1 \
CONAN_VERBOSE_TRACEBACK=1 \
CONAN_PRINT_RUN_COMMANDS=1 \
CONAN_LOGGING_LEVEL=10 \
GIT_SSL_NO_VERIFY=true \
  cmake -E time \
    conan install . \
    --install-folder local_build \
    -s build_type=Debug \
    -s cling_conan:build_type=Release \
    -s llvm_tools:build_type=Release \
    --profile clang \
        -e flextool:enable_tests=True \
        -e flextool:enable_llvm_tools=True

CONAN_REVISIONS_ENABLED=1 \
CONAN_VERBOSE_TRACEBACK=1 \
CONAN_PRINT_RUN_COMMANDS=1 \
CONAN_LOGGING_LEVEL=10 \
GIT_SSL_NO_VERIFY=true \
  cmake -E time \
    conan source . --source-folder local_build

# see section about `conan editable mode`
cd local_build

export CXX=clang++-6.0
export CC=clang-6.0

# NOTE: change `build_type=Debug` to `build_type=Release` in production
build_type=Debug

# remove old CMakeCache
(rm CMakeCache.txt || true)

# NOTE: changed CMAKE_C_COMPILER to ccc-analyzer (!!!)
# configure via cmake
scan-build \
  --use-cc=clang-6.0 \
  --use-c++=clang++-6.0 \
  -o ./scanbuildout/ \
  cmake .. \
  -DCMAKE_C_COMPILER=ccc-analyzer \
  -DCMAKE_CXX_COMPILER=c++-analyzer \
  -DENABLE_TESTS=FALSE \
  -DBUILD_SHARED_LIBS=FALSE \
  -DCONAN_AUTO_INSTALL=OFF \
  -DCMAKE_BUILD_TYPE=${build_type}

# remove old build artifacts
(make clean || true)
rm -rf bin

# NOTE: requires project configured in debug build
# disable other static analyzers
# run from build directory
scan-build \
  -maxloop 8 \
  -enable-checker alpha.security.taint.TaintPropagation \
  -enable-checker alpha.core.BoolAssignment \
  -enable-checker alpha.core.CastSize \
  -enable-checker alpha.core.DynamicTypeChecker \
  -enable-checker alpha.core.FixedAddr \
  -enable-checker alpha.core.IdenticalExpr \
  -enable-checker alpha.core.PointerArithm \
  -enable-checker alpha.core.PointerSub \
  -enable-checker alpha.core.SizeofPtr \
  -enable-checker alpha.core.TestAfterDivZero \
  -enable-checker alpha.deadcode.UnreachableCode \
  -enable-checker alpha.security.ArrayBoundV2 \
  -enable-checker alpha.security.MallocOverflow \
  -enable-checker alpha.security.ReturnPtrRange \
  -enable-checker alpha.unix.PthreadLock \
  -enable-checker alpha.unix.Stream \
  -enable-checker alpha.unix.cstring.BufferOverlap \
  -enable-checker alpha.unix.cstring.NotNullTerminated \
  -enable-checker alpha.unix.cstring.OutOfBounds \
  -enable-checker nullability.NullableDereferenced \
  -enable-checker optin.performance.Padding \
  -enable-checker security.insecureAPI.rand \
  -enable-checker security.insecureAPI.strcpy \
  --use-cc=clang-6.0 \
  --use-c++=clang++-6.0 \
  -o ./scanbuildout/ \
  make \
  -j8
```

Open resulting `scanbuildout/...../index.html` file

## For contibutors: cppclean

Make sure you use `Debug` build with `-e flextool:enable_llvm_tools=True`

See for details [https://github.com/myint/cppclean](https://github.com/myint/cppclean)

Installation:

```bash
pip install --index-url=https://pypi.python.org/simple/ --trusted-host pypi.org --trusted-host pypi.python.org --trusted-host files.pythonhosted.org --upgrade cppclean
```

Usage (runs cmake with `-DENABLE_CPPCLEAN=ON`):

```bash
cmake -DCPPCLEAN="ON" -DCLEAN_OLD="ON" -P tools/run_tool.cmake
```

NOTE: cppclean requires file encoding to be: `UTF-8 without BOM` (ascii)

## For contibutors: IWYU

Make sure you use `Debug` build with `-e flextool:enable_llvm_tools=True`

include-what-you-use (IWYU) is a project intended to optimise includes.

It will calculate the required headers and add and remove includes as appropriate.

See for details [https://include-what-you-use.org/](https://include-what-you-use.org/)

Usage (runs cmake with `-DENABLE_IWYU=ON`):

```bash
cmake -DIWYU="ON" -DCLEAN_OLD="ON" -P tools/run_tool.cmake
```

CODESTYLE: use `// IWYU pragma: associated` in C++ source files.

NOTE: Read about IWYU Pragmas [https://github.com/include-what-you-use/include-what-you-use/blob/master/docs/IWYUPragmas.md](https://github.com/include-what-you-use/include-what-you-use/blob/master/docs/IWYUPragmas.md)

NOTE: don`t use "bits/" or "/details/*" includes, add them to mappings file (.imp)

See for details:

* https://llvm.org/devmtg/2010-11/Silverstein-IncludeWhatYouUse.pdf
* https://github.com/include-what-you-use/include-what-you-use/tree/master/docs
* https://github.com/hdclark/Ygor/blob/master/artifacts/20180225_include-what-you-use/iwyu_how-to.txt

## For contibutors: ccache

Use `-DUSE_CCACHE=ON`

```bash
gcc -v
export CC=gcc
export CXX=g++
# NOTE: -DUSE_CCACHE=ON
cmake .. \
  -DUSE_CCACHE=ON \
  -DENABLE_TESTS=FALSE \
  -DBUILD_SHARED_LIBS=FALSE \
  -DCONAN_AUTO_INSTALL=OFF \
  -DCMAKE_BUILD_TYPE=Debug
```

See for details [https://www.virag.si/2015/07/use-ccache-with-cmake-for-faster-compilation/](https://www.virag.si/2015/07/use-ccache-with-cmake-for-faster-compilation/)

## For contibutors: GOLD linker

Installation:

```bash
sudo apt-get install ccache

# On OS X use homebrew:
# brew install ccache

ccache --version
```

Use `-DUSE_LD_GOLD=ON`

```bash
gcc -v
export CC=gcc
export CXX=g++
# NOTE: -DUSE_LD_GOLD=ON
cmake .. \
  -DUSE_LD_GOLD=ON \
  -DENABLE_TESTS=FALSE \
  -DBUILD_SHARED_LIBS=FALSE \
  -DCONAN_AUTO_INSTALL=OFF \
  -DCMAKE_BUILD_TYPE=Debug
```

See for details [https://cristianadam.eu/20170709/speeding-up-cmake/](https://cristianadam.eu/20170709/speeding-up-cmake/)

NOTE: gold not threaded by default, configure with "--enable-threads"

NOTE: lld threaded by default, may be faster than gold

## For contibutors: oclint

Make sure you use `Debug` build with `-e flextool:enable_llvm_tools=True`

See for details: https://oclint-docs.readthedocs.io/en/stable/devel/codingstandards.html

Installation:

```bash
cd ~
wget --no-check-certificate https://github.com/oclint/oclint/releases/download/v0.13.1/oclint-0.13.1-x86_64-linux-4.4.0-112-generic.tar.gz
# mirror 1: http://github.strcpy.cn/oclint/oclint/releases/download/v0.13.1/oclint-0.13.1-x86_64-linux-4.4.0-112-generic.tar.gz
# mirror 2: http://archives.oclint.org/releases/0.8/oclint-0.8.1-x86_64-linux-3.13.0-35-generic.tar.gz
tar -xzvf oclint-0.13.1-x86_64-linux-4.4.0-112-generic.tar.gz
rm -rf oclint-0.13.1-x86_64-linux-4.4.0-112-generic.tar.gz
export OCLINT_HOME=~/oclint-0.13.1
export PATH=$OCLINT_HOME/bin:$PATH
oclint -version
```

Usage (runs cmake with `-DENABLE_OCLINT=ON`):

```bash
# NOTE: -DCLEAN_OLD="OFF" to keep generated html report
cmake -DOCLINT="ON" -DCLEAN_OLD="OFF" -P tools/run_tool.cmake
```

```bash
# `report.html` must exist
# find $PWD -name report.html
```

Open report.html

See oclint tutorial: [http://docs.oclint.org/en/stable/intro/tutorial.html](http://docs.oclint.org/en/stable/intro/tutorial.html)

See list of oclint rules at [https://oclint-docs.readthedocs.io/en/stable/rules/](https://oclint-docs.readthedocs.io/en/stable/rules/)

NOTE: you can suppress oclint warnings [http://docs.oclint.org/en/stable/howto/suppress.html#oclint-comment](http://docs.oclint.org/en/stable/howto/suppress.html#oclint-comment)

## For contibutors: clang-format

See for details [https://clang.llvm.org/docs/ClangFormat.html](https://clang.llvm.org/docs/ClangFormat.html)

Usage (runs cmake with `-DENABLE_CLANG_FORMAT=ON`):

```bash
cmake -DCLANG_FORMAT="ON" -DCLEAN_OLD="ON" -P tools/run_tool.cmake
```

We use `.clang-format` file. See for details [https://clang.llvm.org/docs/ClangFormatStyleOptions.html](https://clang.llvm.org/docs/ClangFormatStyleOptions.html)

NOTE: we use `DisableFormat`, so clang-format will change only include order based on `SortIncludes`.

Unfortunately, `clang-format` is not configurable enough, so it can be used only to sort includes. See https://stackoverflow.com/a/32191189

## For contibutors: uncrustify

We use uncrustify bacause clang-format and astyle [do not support a lot of options](https://dev.to/voins/does-anybody-know-a-good-working-c-formatting-tool-2lpi).

See for details [https://patrickhenson.com/2018/06/07/uncrustify-configuration.html](https://patrickhenson.com/2018/06/07/uncrustify-configuration.html)

Installation:

```bash
cd ~
git clone https://github.com/uncrustify/uncrustify.git
cd uncrustify
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
export UNCRUSTIFY_HOME=~/uncrustify/build
export PATH=$UNCRUSTIFY_HOME:$PATH
# OR sudo make install

uncrustify -version
```

Usage (runs cmake with `-DENABLE_UNCRUSTIFY=ON`):

```bash
cmake -DUNCRUSTIFY="ON" -DCLEAN_OLD="ON" -P tools/run_tool.cmake
```

We use `uncrustify.cfg` file. See for details [https://patrickhenson.com/2018/06/07/uncrustify-configuration.html](https://patrickhenson.com/2018/06/07/uncrustify-configuration.html)

To get a list of all available options use:

```bash
uncrustify --show-config
```

Uncrustify has a lot of configurable options. You'll probably need Universal Indent GUI (in Konstantin's reply) as well to configure it: http://universalindent.sourceforge.net/

Use comments containing `/* *INDENT-OFF* */` and `/* *INDENT-ON* */` to disable processing of parts of the source file.

See `disable_processing_cmt` from `uncrustify.cfg`:

```ini
# Specify the marker used in comments to disable processing of part of the
# file.
# The comment should be used alone in one line.
#
# Default:  *INDENT-OFF*
disable_processing_cmt          = " *INDENT-OFF*"      # string

# Specify the marker used in comments to (re)enable processing in a file.
# The comment should be used alone in one line.
#
# Default:  *INDENT-ON*
enable_processing_cmt           = " *INDENT-ON*"     # string
```

You can integrate `uncrustify` with IDE:

* QT Creator [https://doc.qt.io/qtcreator/creator-beautifier.html](https://doc.qt.io/qtcreator/creator-beautifier.html)
* Visual Studio Code [https://marketplace.visualstudio.com/items?itemName=LaurentTreguier.uncrustify](https://marketplace.visualstudio.com/items?itemName=LaurentTreguier.uncrustify)

## For contibutors: Thread Sanitizer

NOTE: Libc/libstdc++ static linking is not supported.

NOTE: Disable custom memory allocation functions. This can hide memory access bugs and prevent the detection of memory access errors.

NOTE: Non-position-independent executables are not supported.
Therefore, the fsanitize=thread flag will cause Clang to act as though the -fPIE flag had been supplied if compiling without -fPIC, and as though the -pie flag had been supplied if linking an executable.

Set env. var.:

```bash
# see https://github.com/google/sanitizers/wiki/ThreadSanitizerFlags
export TSAN_OPTIONS="handle_segv=0:disable_coredump=0:abort_on_error=1:report_thread_leaks=0"

# make sure that env. var. TSAN_SYMBOLIZER_PATH points to llvm-symbolizer
# conan package llvm_tools provides llvm-symbolizer
# and prints its path during cmake configure step
# echo $TSAN_SYMBOLIZER_PATH
```

Requires `enable_llvm_tools=True` and `llvm_tools:build_type=Release`:

```bash
-s llvm_tools:build_type=Release \
  -e flextool:enable_llvm_tools=True
```

You must build project and some deps with `enable_tsan=True`:

```bash
CONAN_REVISIONS_ENABLED=1 \
CONAN_VERBOSE_TRACEBACK=1 \
CONAN_PRINT_RUN_COMMANDS=1 \
CONAN_LOGGING_LEVEL=10 \
GIT_SSL_NO_VERIFY=true \
  cmake -E time \
    conan install . \
    --install-folder local_build \
    -s build_type=Debug \
    -s cling_conan:build_type=Release \
    -s llvm_tools:build_type=Release \
    --profile clang \
        -e flextool:enable_tests=True \
        -e flextool:enable_llvm_tools=True \
        -o flextool:enable_tsan=True \
        -o chromium_base:enable_tsan=True \
        -e chromium_base:enable_llvm_tools=True \
        -o chromium_base:use_alloc_shim=False \
        -o basis:enable_tsan=True \
        -e basis:enable_llvm_tools=True \
        -o flexlib:enable_tsan=True \
        -e flexlib:enable_llvm_tools=True \
        -o flexlib:enable_clang_from_conan=False \
        -o chromium_tcmalloc:use_alloc_shim=False \
        --build chromium_base \
        --build chromium_tcmalloc \
        --build basis \
        --build flexlib

# remove old CMakeCache
(rm local_build/CMakeCache.txt || true)

# see section about `conan editable mode`
conan build . \
  --build-folder local_build \
  --source-folder local_build

# you can run some tests to check sanitizers
./local_build/bin/Debug/tests/flextool-gmock \
  --gtest_filter=ToolsSanityTest.DISABLED_ThreadSanitizerTest \
  --gtest_also_run_disabled_tests
```

NOTE: Change of options may require rebuild of some deps (`--build=missing`).

Read TSAN manuals:

* [https://clang.llvm.org/docs/ThreadSanitizer.html](https://clang.llvm.org/docs/ThreadSanitizer.html)
* [https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual](https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual)
* [https://www.jetbrains.com/help/clion/google-sanitizers.html#TSanChapter](https://www.jetbrains.com/help/clion/google-sanitizers.html#TSanChapter)
* [https://www.chromium.org/developers/testing/threadsanitizer-tsan-v2](https://www.chromium.org/developers/testing/threadsanitizer-tsan-v2)

NOTE: ThreadSanitizer generally requires all code to be compiled with -fsanitize=thread.
If some code (e.g. dynamic libraries) is not compiled with the flag, it can lead to false positive race reports, false negative race reports and/or missed stack frames in reports depending on the nature of non-instrumented code.

Usage (`-DENABLE_TSAN=ON`):

```bash
cd ~/flextool

# see section about `conan editable mode`
cd local_build

# remove old CMakeCache
(rm CMakeCache.txt || true)

# NOTE: -DENABLE_TSAN=ON
cmake .. \
  -DENABLE_TSAN=ON \
  -DENABLE_TESTS=FALSE \
  -DBUILD_SHARED_LIBS=FALSE \
  -DCONAN_AUTO_INSTALL=OFF \
  -DCMAKE_BUILD_TYPE=Debug

# remove old build artifacts
rm -rf flextool
rm -rf bin
find . -iname '*.o' -exec rm {} \;
find . -iname '*.a' -exec rm {} \;
find . -iname '*.dll' -exec rm {} \;
find . -iname '*.lib' -exec rm {} \;

# build code
cmake -E time cmake --build . \
  --config Debug \
  -- -j8
```

NOTE: you can create blacklist file, see:

* [https://clang.llvm.org/docs/SanitizerSpecialCaseList.html](https://clang.llvm.org/docs/SanitizerSpecialCaseList.html)
* [https://www.mono-project.com/docs/debug+profile/clang/blacklists/](https://www.mono-project.com/docs/debug+profile/clang/blacklists/)

## For contibutors: Address Sanitizer

See for details:

* [http://btorpey.github.io/blog/2014/03/27/using-clangs-address-sanitizer/](http://btorpey.github.io/blog/2014/03/27/using-clangs-address-sanitizer/)
* [https://genbattle.bitbucket.io/blog/2018/01/05/Dev-Santa-Claus-Part-1/](https://genbattle.bitbucket.io/blog/2018/01/05/Dev-Santa-Claus-Part-1/)

NOTE: Disable custom memory allocation functions. This can hide memory access bugs and prevent the detection of memory access errors.

Set env. var.:

```bash
# see https://github.com/google/sanitizers/wiki/AddressSanitizerFlags
export ASAN_OPTIONS="fast_unwind_on_malloc=0:strict_init_order=1:check_initialization_order=true:symbolize=1:handle_segv=0:detect_leaks=1:detect_stack_use_after_return=1:disable_coredump=0:abort_on_error=1"
# you can also set LSAN_OPTIONS=suppressions=suppr.txt

# make sure that env. var. ASAN_SYMBOLIZER_PATH points to llvm-symbolizer
# conan package llvm_tools provides llvm-symbolizer
# and prints its path during cmake configure step
# echo $ASAN_SYMBOLIZER_PATH
```

Requires `enable_llvm_tools=True` and `llvm_tools:build_type=Release`:

```bash
-s llvm_tools:build_type=Release \
  -e flextool:enable_llvm_tools=True
```

You must build project and some deps with `enable_asan=True`:

```bash
CONAN_REVISIONS_ENABLED=1 \
CONAN_VERBOSE_TRACEBACK=1 \
CONAN_PRINT_RUN_COMMANDS=1 \
CONAN_LOGGING_LEVEL=10 \
GIT_SSL_NO_VERIFY=true \
  cmake -E time \
    conan install . \
    --install-folder local_build \
    -s build_type=Debug \
    -s cling_conan:build_type=Release \
    -s llvm_tools:build_type=Release \
    --profile clang \
        -e flextool:enable_tests=True \
        -e flextool:enable_llvm_tools=True \
        -o flextool:enable_asan=True \
        -o chromium_base:enable_asan=True \
        -e chromium_base:enable_llvm_tools=True \
        -o chromium_base:use_alloc_shim=False \
        -o basis:enable_asan=True \
        -e basis:enable_llvm_tools=True \
        -o flexlib:enable_asan=True \
        -e flexlib:enable_llvm_tools=True \
        -o flexlib:enable_clang_from_conan=False \
        -o chromium_tcmalloc:use_alloc_shim=False \
        --build chromium_base \
        --build chromium_tcmalloc \
        --build basis \
        --build flexlib

# remove old CMakeCache
(rm local_build/CMakeCache.txt || true)

# see section about `conan editable mode`
conan build . \
  --build-folder local_build \
  --source-folder local_build

# you can run some tests to check sanitizers
./local_build/bin/Debug/tests/flextool-gmock \
  --gtest_filter=ToolsSanityTest.DISABLED_AddressSanitizerTest \
  --gtest_also_run_disabled_tests
# must print `AddressSanitizer: stack-buffer-overflow on address...`
```

NOTE: Change of options may require rebuild of some deps (`--build=missing`).

Read ASAN manuals:

* [https://clang.llvm.org/docs/AddressSanitizer.html](https://clang.llvm.org/docs/AddressSanitizer.html)
* [https://www.chromium.org/developers/testing/addresssanitizer](https://www.chromium.org/developers/testing/addresssanitizer)
* [https://www.jetbrains.com/help/clion/google-sanitizers.html#AsanChapter](https://www.jetbrains.com/help/clion/google-sanitizers.html#AsanChapter)
* [https://github.com/google/sanitizers/wiki](https://github.com/google/sanitizers/wiki)

NOTE: `detect_leaks=1` enables Leak Sanitizer, see [https://sites.google.com/a/chromium.org/dev/developers/testing/leaksanitizer](https://sites.google.com/a/chromium.org/dev/developers/testing/leaksanitizer)

Usage (`-DENABLE_ASAN=ON`):

```bash
cd ~/flextool

# see section about `conan editable mode`
cd local_build

# remove old CMakeCache
(rm CMakeCache.txt || true)

# NOTE: -DENABLE_ASAN=ON
cmake .. \
  -DENABLE_ASAN=ON \
  -DENABLE_TESTS=FALSE \
  -DBUILD_SHARED_LIBS=FALSE \
  -DCONAN_AUTO_INSTALL=OFF \
  -DCMAKE_BUILD_TYPE=Debug

# remove old build artifacts
rm -rf flextool
rm -rf bin
find . -iname '*.o' -exec rm {} \;
find . -iname '*.a' -exec rm {} \;
find . -iname '*.dll' -exec rm {} \;
find . -iname '*.lib' -exec rm {} \;

# build code
cmake -E time cmake --build . \
  --config Debug \
  -- -j8
```

NOTE: you can create blacklist file, see:

* [https://clang.llvm.org/docs/SanitizerSpecialCaseList.html](https://clang.llvm.org/docs/SanitizerSpecialCaseList.html)
* [https://www.mono-project.com/docs/debug+profile/clang/blacklists/](https://www.mono-project.com/docs/debug+profile/clang/blacklists/)

## For contibutors: Memory Sanitizer

NOTE: MemorySanitizer requires that all program code is instrumented.
This also includes any libraries that the program depends on, even libc. Failing to achieve this may result in false reports.
For the same reason you may need to replace all inline assembly code that writes to memory with a pure C/C++ code.

NOTE: Disable custom memory allocation functions. This can hide memory access bugs and prevent the detection of memory access errors.

Set env. var.:

```bash
# see https://github.com/google/sanitizers/wiki/SanitizerCommonFlags
export MSAN_OPTIONS="poison_in_dtor=1:fast_unwind_on_malloc=0:check_initialization_order=true:handle_segv=0:detect_leaks=1:detect_stack_use_after_return=1:disable_coredump=0:abort_on_error=1"
# you can also set LSAN_OPTIONS=suppressions=suppr.txt

# make sure that env. var. MSAN_SYMBOLIZER_PATH points to llvm-symbolizer
# conan package llvm_tools provides llvm-symbolizer
# and prints its path during cmake configure step
# echo $MSAN_SYMBOLIZER_PATH
```

Requires `enable_llvm_tools=True` and `llvm_tools:build_type=Release`:

```bash
-s llvm_tools:build_type=Release \
  -e flextool:enable_llvm_tools=True
```

If you want MemorySanitizer to work properly and not produce any false positives, you must ensure that all the code in your program and in libraries it uses is instrumented (i.e. built with `-fsanitize=memory`).
In particular, you would need to link against MSan-instrumented C++ standard library. See [https://github.com/google/sanitizers/wiki/MemorySanitizerLibcxxHowTo](https://github.com/google/sanitizers/wiki/MemorySanitizerLibcxxHowTo)
You need to re-build both C++ standard library and googletest (and other dependencies) with MemorySanitizer.

Re-build all required projects with `-o llvm_tools:enable_msan=True`, `enable_llvm_tools=True`, `compile_with_llvm_tools=True` and `llvm_tools:build_type=Release` to use MSan-instrumented C++ standard library.

NOTE: re-build some deps with custom MSan-instrumented C++ standard library. For that you can add to `CMAKE_C_FLAGS` and `CMAKE_CXX_FLAGS`: `-fsanitize=memory -stdlib=libc++ -L/usr/src/libcxx_msan/lib -lc++abi -I/usr/src/libcxx_msan/include -I/usr/src/libcxx_msan/include/c++/v1` (replace paths to yours). Usually dependency will have option like `-o *:enable_msan=True` to achieve same effect.

You must build project and some deps with `enable_msan=True`:

```bash
# NOTE: Re-build all deps with MSan-instrumented C++ standard library.
# Usually it may be achieved using `*:compile_with_llvm_tools=True` and `-o llvm_tools:enable_msan=True`.

CONAN_REVISIONS_ENABLED=1 \
CONAN_VERBOSE_TRACEBACK=1 \
CONAN_PRINT_RUN_COMMANDS=1 \
CONAN_LOGGING_LEVEL=10 \
GIT_SSL_NO_VERIFY=true \
  cmake -E time \
    conan install . \
    --install-folder local_build \
    -s build_type=Debug \
    -s cling_conan:build_type=Release \
    -s llvm_tools:build_type=Release \
    -s compiler=clang \
    -s compiler.version=10 \
    -s compiler.libcxx=libstdc++11 \
    -s llvm_tools:compiler=clang \
    -s llvm_tools:compiler.version=6.0 \
    -s llvm_tools:compiler.libcxx=libstdc++11 \
    --profile clang \
        -o llvm_tools:enable_msan=True \
        -o llvm_tools:include_what_you_use=False \
        -e flextool:compile_with_llvm_tools=True \
        -e flextool:enable_tests=True \
        -e flextool:enable_llvm_tools=True \
        -o flextool:enable_msan=True \
        -o chromium_base:enable_msan=True \
        -e chromium_base:enable_llvm_tools=True \
        -o chromium_base:use_alloc_shim=False \
        -o basis:enable_msan=True \
        -e basis:enable_llvm_tools=True \
        -o flexlib:enable_msan=True \
        -e flexlib:enable_llvm_tools=True \
        -o flexlib:enable_clang_from_conan=False \
        -o chromium_tcmalloc:use_alloc_shim=False \
        --build chromium_base \
        --build chromium_tcmalloc \
        --build basis \
        --build flexlib

# remove old CMakeCache
(rm local_build/CMakeCache.txt || true)

# see section about `conan editable mode`
conan build . \
  --build-folder local_build \
  --source-folder local_build

# you can run some tests to check sanitizers
./local_build/bin/Debug/tests/flextool-gmock \
  --gtest_filter=ToolsSanityTest.DISABLED_MemorySanitizerTest \
  --gtest_also_run_disabled_tests
```

NOTE: Change of options may require rebuild of some deps (`--build=missing`).

Read MSAN manuals:

* [https://www.chromium.org/developers/testing/memorysanitizer](https://www.chromium.org/developers/testing/memorysanitizer)
* [https://github.com/google/sanitizers/wiki](https://github.com/google/sanitizers/wiki)
* [https://www.jetbrains.com/help/clion/google-sanitizers.html#MSanChapter](https://www.jetbrains.com/help/clion/google-sanitizers.html#MSanChapter)
* [https://static.googleusercontent.com/media/research.google.com/en//pubs/archive/43308.pdf](https://static.googleusercontent.com/media/research.google.com/en//pubs/archive/43308.pdf)

NOTE: `detect_leaks=1` enables Leak Sanitizer, see [https://sites.google.com/a/chromium.org/dev/developers/testing/leaksanitizer](https://sites.google.com/a/chromium.org/dev/developers/testing/leaksanitizer)

Usage (`-DENABLE_MSAN=ON`):

```bash
cd ~/flextool

# see section about `conan editable mode`
cd local_build

# remove old CMakeCache
(rm CMakeCache.txt || true)

# NOTE: -DENABLE_MSAN=ON
cmake .. \
  -DENABLE_MSAN=ON \
  -DENABLE_TESTS=FALSE \
  -DBUILD_SHARED_LIBS=FALSE \
  -DCONAN_AUTO_INSTALL=OFF \
  -DCMAKE_BUILD_TYPE=Debug

# remove old build artifacts
rm -rf flextool
rm -rf bin
find . -iname '*.o' -exec rm {} \;
find . -iname '*.a' -exec rm {} \;
find . -iname '*.dll' -exec rm {} \;
find . -iname '*.lib' -exec rm {} \;

# build code
cmake -E time cmake --build . \
  --config Debug \
  -- -j8
```

NOTE: you can create blacklist file, see:

* [https://clang.llvm.org/docs/SanitizerSpecialCaseList.html](https://clang.llvm.org/docs/SanitizerSpecialCaseList.html)
* [https://www.mono-project.com/docs/debug+profile/clang/blacklists/](https://www.mono-project.com/docs/debug+profile/clang/blacklists/)

FAQ:

* How to use MemorySanitizer (MSAN) with `syscall(SYS_getrandom, ..., ..., ...)`?
  Use __msan_unpoison:
  ```cpp
  #if defined(__has_feature)
  #if __has_feature(memory_sanitizer)
          /* MemorySanitizer (MSAN) does not support syscall(SYS_getrandom, ..., ..., ...):
           * Use __msan_unpoison to make MSAN understand how many bytes that have been
           * written to ent32.
           *
           * __msan_unpoison does not change the actual memory content, but only MSAN's
           * perception of the memory content.
           *
           * See https://github.com/google/sanitizers/issues/852 ("memory sanitizer: not
           * tracking memory initialization with getrandom") for details.
           */
          __msan_unpoison(ent32, rv);
  #endif
  #endif
  ```

## For contibutors: Undefined Behavior Sanitizer

NOTE: Disable custom memory allocation functions. This can hide memory access bugs and prevent the detection of memory access errors.

Set env. var.:

```bash
# see https://github.com/google/sanitizers/wiki/SanitizerCommonFlags
export UBSAN_OPTIONS="fast_unwind_on_malloc=0:handle_segv=0:disable_coredump=0:halt_on_error=1:print_stacktrace=1"

# make sure that env. var. UBSAN_SYMBOLIZER_PATH points to llvm-symbolizer
# conan package llvm_tools provides llvm-symbolizer
# and prints its path during cmake configure step
# echo $UBSAN_SYMBOLIZER_PATH
```

Requires `enable_llvm_tools=True` and `llvm_tools:build_type=Release`:

```bash
-s llvm_tools:build_type=Release \
  -e flextool:enable_llvm_tools=True
```

NOTE: Make sure to use clang++ (not ld) as a linker, so that your executable is linked with proper UBSan runtime libraries.

You must build project and some deps with `enable_ubsan=True`:

```bash
CONAN_REVISIONS_ENABLED=1 \
CONAN_VERBOSE_TRACEBACK=1 \
CONAN_PRINT_RUN_COMMANDS=1 \
CONAN_LOGGING_LEVEL=10 \
GIT_SSL_NO_VERIFY=true \
  cmake -E time \
    conan install . \
    --install-folder local_build \
    -s build_type=Debug \
    -s cling_conan:build_type=Release \
    -s llvm_tools:build_type=Release \
    --profile clang \
        -e flextool:enable_tests=True \
        -e flextool:enable_llvm_tools=True \
        -o flextool:enable_ubsan=True \
        -o chromium_base:enable_ubsan=True \
        -e chromium_base:enable_llvm_tools=True \
        -o chromium_base:use_alloc_shim=False \
        -o basis:enable_ubsan=True \
        -e basis:enable_llvm_tools=True \
        -o flexlib:enable_ubsan=True \
        -e flexlib:enable_llvm_tools=True \
        -o flexlib:enable_clang_from_conan=False \
        -o chromium_tcmalloc:use_alloc_shim=False \
        --build chromium_base \
        --build chromium_tcmalloc \
        --build basis \
        --build flexlib

# remove old CMakeCache
(rm local_build/CMakeCache.txt || true)

# see section about `conan editable mode`
conan build . \
  --build-folder local_build \
  --source-folder local_build

# you can run some tests to check sanitizers
./local_build/bin/Debug/tests/flextool-gmock \
  --gtest_filter=ToolsSanityTest.DISABLED_UndefinedBehaviorSanitizerTest \
  --gtest_also_run_disabled_tests
```

NOTE: Use `_Nonnull` annotation, see [https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html#ubsan-checks](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html#ubsan-checks)

`_Nonnull` annotation requires `-fsanitize=nullability-*`, so we enable them by default:

```bash
  -fsanitize=nullability-arg \
  -fsanitize=nullability-assign \
  -fsanitize=nullability-return \
```

```cpp
Violation of the nonnull Parameter Attribute in C
In the following example, the call to the has_nonnull_argument function breaks the nonnull attribute of the parameter p.

void has_nonnull_argument(__attribute__((nonnull)) int *p) {
     // ...
}
has_nonnull_argument(NULL); // Error: nonnull parameter attribute violation
```

NOTE: Change of options may require rebuild of some deps (`--build=missing`).

Read UBSAN manuals:

* [https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)
* [https://www.jetbrains.com/help/clion/google-sanitizers.html#UbSanChapter](https://www.jetbrains.com/help/clion/google-sanitizers.html#UbSanChapter)
* [https://github.com/google/sanitizers/wiki](https://github.com/google/sanitizers/wiki)
* [https://www.chromium.org/developers/testing/undefinedbehaviorsanitizer](https://www.chromium.org/developers/testing/undefinedbehaviorsanitizer)

Usage (`-DENABLE_UBSAN=ON`):

```bash
cd ~/flextool

# see section about `conan editable mode`
cd local_build

# remove old CMakeCache
(rm CMakeCache.txt || true)

# NOTE: -DENABLE_UBSAN=ON
cmake .. \
  -DENABLE_UBSAN=ON \
  -DENABLE_TESTS=FALSE \
  -DBUILD_SHARED_LIBS=FALSE \
  -DCONAN_AUTO_INSTALL=OFF \
  -DCMAKE_BUILD_TYPE=Debug

# remove old build artifacts
rm -rf flextool
rm -rf bin
find . -iname '*.o' -exec rm {} \;
find . -iname '*.a' -exec rm {} \;
find . -iname '*.dll' -exec rm {} \;
find . -iname '*.lib' -exec rm {} \;

# build code
cmake -E time cmake --build . \
  --config Debug \
  -- -j8
```

NOTE: you can create blacklist file, see:

* [https://clang.llvm.org/docs/SanitizerSpecialCaseList.html](https://clang.llvm.org/docs/SanitizerSpecialCaseList.html)
* [https://www.mono-project.com/docs/debug+profile/clang/blacklists/](https://www.mono-project.com/docs/debug+profile/clang/blacklists/)

## For contibutors: build using clang 10 from conan

Requires `enable_llvm_tools=True` and `compile_with_llvm_tools=True` and `llvm_tools:build_type=Release`:

```bash
-s llvm_tools:build_type=Release \
  -e flextool:enable_llvm_tools=True \
  -e flextool:compile_with_llvm_tools=True \
  -o llvm_tools:include_what_you_use=True \
  ...
```

* `enable_llvm_tools` installs clang 10 from conan
* `compile_with_llvm_tools` sets cmake variables required to use clang 10 from conan

run `conan install` or `conan create` with

```bash
# OR create conan profile https://docs.conan.io/en/latest/reference/profiles.html
-s compiler=clang \
  -s compiler.version=10 \
  -s compiler.libcxx=libstdc++11
```

NOTE: Change of compiler may require rebuild of all deps (`--build=missing`).

Example in case of local build:

```bash
CONAN_REVISIONS_ENABLED=1 \
CONAN_VERBOSE_TRACEBACK=1 \
CONAN_PRINT_RUN_COMMANDS=1 \
CONAN_LOGGING_LEVEL=10 \
GIT_SSL_NO_VERIFY=true \
  cmake -E time \
    conan install . \
    --install-folder local_build \
    -s build_type=Debug \
    -s cling_conan:build_type=Release \
    -s llvm_tools:build_type=Release \
    -s llvm_tools:compiler=clang \
    -s llvm_tools:compiler.version=6.0 \
    -s llvm_tools:compiler.libcxx=libstdc++11 \
    --profile clang \
        -e flextool:enable_tests=True \
        -e flextool:enable_llvm_tools=True \
        -e flextool:compile_with_llvm_tools=True \
        -o llvm_tools:include_what_you_use=True \
        -s compiler=clang \
        -s compiler.version=10 \
        -s compiler.libcxx=libstdc++11

CONAN_REVISIONS_ENABLED=1 \
CONAN_VERBOSE_TRACEBACK=1 \
CONAN_PRINT_RUN_COMMANDS=1 \
CONAN_LOGGING_LEVEL=10 \
GIT_SSL_NO_VERIFY=true \
  cmake -E time \
    conan source . --source-folder local_build

# remove old CMakeCache
(rm local_build/CMakeCache.txt || true)

# see section about `conan editable mode`
conan build . \
  --build-folder local_build \
  --source-folder local_build
```

## For contibutors: doxygen

`MCSS_ROOT_DIR_FOR_DOCS` must point to `m.css` sources like below:

```bash
cd ~

git clone https://github.com/mosra/m.css.git

pip3 install jinja2 Pygments

sudo apt install \
    texlive-base \
    texlive-latex-extra \
    texlive-fonts-extra \
    texlive-fonts-recommended

sudo apt-get install doxygen

sudo apt install python3-pip

# /usr/bin/python must point to python3
/usr/bin/python --version

# NOTE: switch to python3 for doxygen or use -DPYTHON_EXECUTABLE=/usr/bin/python3
alias python='/usr/bin/python3'

# You may need sudo here
pip3 install jinja2 Pygments

sudo apt install \
    texlive-base \
    texlive-latex-extra \
    texlive-fonts-extra \
    texlive-fonts-recommended
```

Use cmake build with '--target doxyDoc' and `-DBUILD_DOXY_DOC=ON`

```bash
cd ~/flextool

# see section about `conan editable mode`
cd local_build

# remove old CMakeCache
(rm CMakeCache.txt || true)

# remove old build artifacts
rm -rf flextool
rm -rf bin
find . -iname '*.o' -exec rm {} \;
find . -iname '*.a' -exec rm {} \;
find . -iname '*.dll' -exec rm {} \;
find . -iname '*.lib' -exec rm {} \;

# remove old build docs
rm -rf doc-mcss
rm -rf docs

cmake -E make_directory "doc-mcss"

# NOTE: you can change python version like so: -DPYTHON_EXECUTABLE=/usr/bin/python3
cmake .. \
  -DMCSS_ROOT_DIR_FOR_DOCS=$HOME/m.css \
  -DPYTHON_EXECUTABLE=/usr/bin/python3 \
  -DENABLE_TESTS=TRUE \
  -DBUILD_DOXY_DOC=ON \
  -DBUILD_SHARED_LIBS=FALSE \
  -DCONAN_AUTO_INSTALL=OFF \
  -DCMAKE_BUILD_TYPE=Debug \
  -DDOXY_ROOT_DIR=$PWD/doc-mcss \
  -DDOXY_DOC_COMMON_IMG_PATH=$PWD/.. \
  -DPROJECT_SOURCES_DIR_FOR_DOCS=$PWD/../src

cmake -E time cmake --build . \
  --target doxyDoc_notheme

cmake -E time cmake --build . \
  --target doxyDoc

# Use to find index.html
find $PWD -name *.html
```

open doxyDoc/html/index.html

NOTE: Document namespaces in docs/namespaces.dox

NOTE: [Files, directories and symbols with no documentation are not present in the output at all](https://mcss.mosra.cz/doxygen/#troubleshooting)

Used [comments style](https://mcss.mosra.cz/doxygen/):

```bash
/**
 * @brief Path utils
 *
 * Example usage:
 *
 * @code{.cpp}
 * const ::fs::path workdir = storage::getThisBinaryDirectoryPath();
 * @endcode
 **/
```

See:

- [doxygen cheatsheet](http://www.agapow.net/programming/tools/doxygen-cheatsheet/)
- [doxygen coding style](https://doc.magnum.graphics/magnum/coding-style.html#coding-style-documentation)

## For contibutors: Fuzzing with AFL

See for details [https://afl-1.readthedocs.io/en/latest/index.html](https://afl-1.readthedocs.io/en/latest/index.html)

NOTE: prefer github.com/google/AFL or aflplus.plus to not updated AFL from `lcamtuf.coredump.cx/afl`

Fuzzing is a Black Box software testing technique.

Fuzzing consists in finding implementation bugs using malformed/semi-malformed data injection in an automated fashion.

Fuzzer tries to modify the input so that it can reach as much lines of the program code as possible.

Therefore, fuzzing allows the discovery of vulnerabilities lying in code paths that are hard to reach by normal usage.

Install compiling the source code using following commands:

```bash
# optional
# sudo apt-get update

# optional
# sudo apt-get -y install autoconf automake bison build-essential \
ca-certificates llvm-dev libtool libtool-bin \
libglib2.0-dev make nasm wget

# Tested with clang 6.0 and gcc 7
sudo apt-get -y install clang-6.0 g++-7 gcc-7

export CXX=g++-7
export CC=gcc-7
export PATH=/usr/bin/:$PATH
$CC -v


# llvm-config binary that coresponds to the same clang you are using to compile
export LLVM_CONFIG=/usr/bin/llvm-config-6.0
$LLVM_CONFIG --cxxflags

cd ~
git clone -b v2.56b https://github.com/google/AFL.git --recursive

# NOTE: original AFL not updated since November 2017,
# so prefer `google/AFL.git` to `lcamtuf.coredump.cx/afl`
# wget http://lcamtuf.coredump.cx/afl/releases/afl-latest.tgz
# tar -xf afl-latest.tgz
# rm afl-latest.tgz
# cd afl*

cd AFL
make
# build llvm using the sample compiler as afl code uses
# see https://groups.google.com/forum/#!topic/afl-users/1WqZpGXvYY0
make \
  -C llvm_mode \
  LLVM_CONFIG=$LLVM_CONFIG \
  CC=clang-6.0 \
  CXX=clang++-6.0
#
# optional
# cd qemu_mode
# ./build_qemu_support.sh
# cd ..
#
make \
  -C libdislocator
make \
  -C libtokencap
sudo make install

# OR add to PATH via export PATH=$PATH:...

# do not forget to reset CC and LLVM_CONFIG
unset CXX
unset CC
unset LLVM_CONFIG
```

We compile code using the AFL compiler using `-DCMAKE_C_COMPILER=afl-clang-fast`, `-DCMAKE_CXX_COMPILER=afl-clang-fast++`, `-DCMAKE_LINKER=afl-clang-fast`.

You can verify if binary uses the AFL compiler using `nm BINARY_PATH | grep afl`

Build application with some sanitizers and debug information enabled (`-DENABLE_ASAN=ON` etc.):

NOTE: There are some things to consider when using Address Sanitizer. Even if ASan finds a memory access violation it doesn't automatically crash the application. This is a problem when using automated fuzzing tools, because they usually try to detect segfaults by checking the return code. We can however force ASan to crash software when an error happens with the environment variable ASAN_OPTIONS like this before fuzzing: `export ASAN_OPTIONS='abort_on_error=1'`

NOTE: Disable custom memory allocation functions. This can hide memory access bugs and prevent the detection of memory access errors.

```bash
# see https://afl-1.readthedocs.io/en/latest/instrumenting.html

# Setting AFL_HARDEN automatically adds code hardening options
# when invoking the downstream compiler.
# This includes -D_FORTIFY_SOURCE=2 and -fstack-protector-all.
# NOTE: _FORTIFY_SOURCE not compatible with ASAN
# export AFL_HARDEN=1

# see https://aflplus.plus/docs/env_variables/
export AFL_EXIT_WHEN_DONE=1

# By default, the wrapper appends -O3 to optimize builds.
export AFL_DONT_OPTIMIZE=1

# or AFL_USE_MSAN, etc.
# READ https://aflplus.plus/docs/notes_for_asan/
NOTE: if you run several slaves only one should run the
# target compiled with ASAN (and UBSAN, CFISAN),
# the others should run the target with no sanitizers compiled in.
export AFL_USE_UBSAN=1
export AFL_USE_ASAN=1

# AFL_PRELOAD causes AFL to set LD_PRELOAD for the target binary
# without disrupting the afl-fuzz process itself.
# This is useful, among other things, for bootstrapping libdislocator.so.

# see __AFL_LOOP
# export AFL_PERSISTENT=1

# make sure you compile app with `-fsanitize=address` or `-fsanitize=memory` etc.
```

NOTE: Use `AFL_DONT_OPTIMIZE=1`, read http://moyix.blogspot.com/2016/07/fuzzing-with-afl-is-an-art.html

See for details `Using ASAN with AFL` at [https://afl-1.readthedocs.io/en/latest/notes_for_asan.html](https://afl-1.readthedocs.io/en/latest/notes_for_asan.html)

Before fuzzing the program, we may require switching to root user to arrange the core_pattern. Login as root and type the following command

```bash
# see https://afl-1.readthedocs.io/en/latest/tips.html#check-os-configuration
sudo su
# disable core dumps and CPU frequency scaling on your system (AFL will warn you if you should do this)
echo core >/proc/sys/kernel/core_pattern
# afl-fuzz will usually complain that you should change your `CPUFREQ` settings to performance because the automatic frequency scaling by the Linux kernel doesn't work well with afl.
echo performance | tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
exit
```

NOTE: Do not run the fuzzer with root access

NOTE: Get a solid environment for the fuzzer, never run the fuzzer on low configured hypervisors

`afl-fuzz` is used to run AFL, the actual syntax is as follows

```bash
# see https://afl-1.readthedocs.io/en/latest/fuzzing.html#fuzzing-binaries
# -i is a directory of files to use as fuzz input "seeds"
# -o is a directory to write the results (including inputs that provoke crashes or hangs)
# -m is the memory allowed to use. Example: -m500
# You can use -m none to disable memory limit
# -t is the maximum time that a run is allowed to take before being declared a "hang"
# Timeout of 10 seconds:  -t 10000
# @@ is fuzzer input file name
# if you skip @@ it will pass the fuzzed file on the standard input
AFL_PERSISTENT=1 afl-fuzz -i [TESTCASE DIR] -o [RESULT_DIR] [TARGET_BINARY] [BINARY_PARAMS] @@

# Example 1: runs `tar` with arguments `xfJ @@ -C fuzz-garbage/ --force-local`
# where @@ is fuzzer input file name
 ./afl-1.56b/afl-fuzz -i fuzz-input/ -o fuzz-state/ -t 10000 ~/tar-1.28/src/tar xfJ @@ -C fuzz-garbage/ --force-local

# Example 2: server is dual core, so we can run one AFL instance per core
AFL_PERSISTENT=1 afl-fuzz -i inputs -o multi_sync -M master ./fuzz_capstone
# In another terminal
AFL_PERSISTENT=1 afl-fuzz -i inputs -o multi_sync -S slave1 ./fuzz_capstone
```

To understand AFL status screen read [https://afl-1.readthedocs.io/en/latest/user_guide.html#status-screen](https://afl-1.readthedocs.io/en/latest/user_guide.html#status-screen)

NOTE: If `total paths` stays at 1 you probably have set up something wrong.

NOTE: Prefer `-m none`. We use AddressSanitizer that maps a lot of pages for the shadow memory so we have to remove the memory limit to have it up and running.

NOTE: With `-m none` your fuzzed software may actually try to really allocate and use a lot of memory due to your fuzzed samples. This may lead to random crashes in your system. You shouldn't do any important work while doing so.

NOTE: you can try `ASAN_OPTIONS=hard_rss_limit_mb=2000` to avoid `-m none`. See https://countuponsecurity.com/category/fuzzing/

You can write custom binary that will run using `afl-fuzz`. It may wrap function that you want to test like so:

```cpp
// harness is simply a C program that makes use of certain methods from
// a library, allowing you to indirectly fuzz it

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <ifstream>

int main(int argc, char *argv[]) {
{
  // init resources here
  if (argc > 1) {
    std::ifstream fin;
    fin.open(argv[1]);
    parse(fin); // custom logic
  } else {
    /// \note requires AFL_PERSISTENT=1
    // __AFL_LOOP is the way that we have to tell AFL
    // that we want persistent mode.
    // Each fuzzing iteration,
    // instead of to fork and re-execute the target with a different input,
    // is just an execution of this loop.
    // Force AFL to run 1000 times,
    // with 1000 different inputs fed to the library.
    // After that, the process is restarted by AFL.
    // This ensures we regularly replace the process to avoid memory leaks.
    // see https://toastedcornflakes.github.io/articles/fuzzing_capstone_with_afl.html
    while (__AFL_LOOP(1000)) {
      parse(std::cin); // custom logic
    }
  }
  // free resources here
  return 0;
}
```

NOTE: __AFL_LOOP(), allows AFL to perform the fuzzing of the binary in process through some memory wizardry, as opposed to starting up a new process for every new testcase we want to test. Requires `AFL_PERSISTENT=1`.

By default, AFL forks a process every time it tests a different input. We can control AFL to run multiple fuzz cases in a single instance of the program, rather than reverting the program state back for every test sample. This will reduce the time spent in the kernel space and improve the fuzzing speed. This is called AFL_PERSISTENT mode. We can do that by including the __AFL_LOOP(1000) macro within our test harness.

NOTE: you may be interested in __AFL_INIT, see for details [https://robertheaton.com/2019/07/08/how-to-write-an-afl-wrapper-for-any-language/](https://robertheaton.com/2019/07/08/how-to-write-an-afl-wrapper-for-any-language/)

Let the fuzzer run for few hours or days as it generates maximum code execution paths based on the test cases provided.

Stop fuzzing issuing `ctrl+c’ observing `total paths` and `uniq crashes` in the section `overall results` of AFL statistics screen.

Create dictionary that takes all of the constants and strings found in the program binary and adds them to the dictionary. See for script code [http://moyix.blogspot.com/2016/07/fuzzing-with-afl-is-an-art.html](http://moyix.blogspot.com/2016/07/fuzzing-with-afl-is-an-art.html)

```bash
#!/bin/bash

# see http://moyix.blogspot.com/2016/07/fuzzing-with-afl-is-an-art.html

objdump -d "${1}" | grep -Eo '\$0x[0-9a-f]+' | cut -c 2- | sort -u | while read const; do echo $const | python -c 'import sys, struct; sys.stdout.write("".join(struct.pack("<I" if len(l) <= 11 else "<Q", int(l,0)) for l in sys.stdin.readlines()))' > testcases/$const; done
i=0; strings "${1}"| while read line; do echo -n "$line" > testcases/string_${i} ; i=$[ $i + 1 ] ; done
```

You need to create a dictionary in one of the two formats discussed in dictionaries/README.dictionaries; and then point the fuzzer to it via the -x option in the command line. Read [https://afl-1.readthedocs.io/en/latest/fuzzing.html#fuzzing-binaries](https://afl-1.readthedocs.io/en/latest/fuzzing.html#fuzzing-binaries) and [https://github.com/mirrorer/afl/blob/master/dictionaries/README.dictionaries](https://github.com/mirrorer/afl/blob/master/dictionaries/README.dictionaries)

You can also use `libtokencap` to create a dictionary, see [https://github.com/mirrorer/afl/blob/master/libtokencap/README.tokencap](https://github.com/mirrorer/afl/blob/master/libtokencap/README.tokencap)

We can find the test cases which cause the crash, in the `results’ folder which we have created. On navigating to the folder `results`, we observe few folders gets generated

NOTE: Keep the input data files small. Under 1 kB is ideal

Use `afl-cmin` to minimize number of input data files.

Use `afl-tmin` to minimize each input data file (removes any bytes that do not affect the code paths taken).

Use `afl-ptmin` to run `afl-tmin` in parallel. See for details [https://foxglovesecurity.com/2016/03/15/fuzzing-workflows-a-fuzz-job-from-start-to-finish/](https://foxglovesecurity.com/2016/03/15/fuzzing-workflows-a-fuzz-job-from-start-to-finish/)

To reproduce found crash you can use `crashwalk` (it is gdb plugin), see [https://ritcsec.wordpress.com/2018/05/10/vulnerability-discovery-by-fuzzing/](https://ritcsec.wordpress.com/2018/05/10/vulnerability-discovery-by-fuzzing/)

```bash
apt-get install gdb golang
mkdir src
cd src
git clone https://github.com/jfoote/exploitable.git
cd && mkdir go
export GOPATH=~/go
# crashwalk installed in $GOPATH/bin/
go get -u github.com/bnagy/crashwalk/cmd/...

# USAGE
~/go/bin/cwtriage -root syncdir/fuzzer1/crashes/ -match id -- ~/parse @@
```

When you can not reproduce a crash found by afl-fuzz, the most likely cause is that you are not setting the same memory limit as used by the tool. Read [https://afl-1.readthedocs.io/en/latest/fuzzing.html#fuzzing-binaries](https://afl-1.readthedocs.io/en/latest/fuzzing.html#fuzzing-binaries)

NOTE: You can use `afl-cov` to quantify how well you are exercising the available code paths in the binary. See for details [https://foxglovesecurity.com/2016/03/15/fuzzing-workflows-a-fuzz-job-from-start-to-finish/](https://foxglovesecurity.com/2016/03/15/fuzzing-workflows-a-fuzz-job-from-start-to-finish/)

NOTE: putting the AFL working directory on a RAM disk, you can potentially gain some additional speed and avoid wearing out the disks at the same time. See for details [https://bananamafia.dev/post/gb-fuzz/](https://bananamafia.dev/post/gb-fuzz/)

```bash
# Fuzzing involves billions of reads and writes to the filesystem (!!!)
# Use RAMdisks for input since, we don't want to destroy harddrives
# Make a 1GB ramdisk file from which AFL can read input
sudo mkdir -p /mnt/inputfiles
sudo chown -R $USER:$(id -gn $USER) /mnt/inputfiles
sudo mount -t tmpfs -o size=1024M tmpfs /mnt/inputfiles/
```

NOTE: `-fvisibility-inlines-hidden` flag MAY BREAK AFL INSTRUMENTATION

AFL provides a crash exploration script in `experimental/crash_triage/triage_crashes.sh`

When run the triage script will cycle through each crash file in /out/crashes directory and print the resulting crash data to the screen.

`triage_crashes` usage: `./triage_crashes.sh ~/targets/out/ ~/targets/target-app/target-app_binary` See for details [https://research.aurainfosec.io/hunting-for-bugs-101/](https://research.aurainfosec.io/hunting-for-bugs-101/)

NOTE: Instrument with AFL just the libraries you actually want to stress-test right now, one at a time. Let the program use system-wide, non-instrumented libraries for any functionality you don’t actually want to fuzz.

NOTE: you can enable `crash exploration mode` via `-C`, see https://lcamtuf.blogspot.com/2014/11/afl-fuzz-crash-exploration-mode.html

NOTE: AFL detects faults by checking for the first spawned process dying due to a signal (SIGSEGV, SIGABRT, etc). Programs that install custom handlers for these signals may need to have the relevant code commented out

See for details:

* https://github.com/mykter/afl-training
* https://www.loginsoft.com/blog/2018/02/02/discovering-vulnerabilities-with-afl-fuzzer/
* https://www.youtube.com/watch?v=vzfhHjnycnE
* https://gamozolabs.github.io/fuzzing/2018/09/16/scaling_afl.html
* https://ritcsec.wordpress.com/2018/05/10/vulnerability-discovery-by-fuzzing/
* https://github.com/Dor1s/libfuzzer-workshop/blob/master/lessons/01/Modern_Fuzzing_of_C_C%2B%2B_projects_slides_1-23.pdf
* https://github.com/hbowden/nextgen/blob/master/CMakeLists.txt#L92
* https://foxglovesecurity.com/2016/03/15/fuzzing-workflows-a-fuzz-job-from-start-to-finish/
* https://cs.anu.edu.au/courses/csprojects/19S1/reports/u6759601_report.pdf
* https://www.fastly.com/blog/how-fuzz-server-american-fuzzy-lop

## For contibutors: Fuzzing with libFuzzer

libFuzzer is part of the LLVM compiler infrastructure project and comes built-in with the clang compiler.

Then libFuzzer can be linked to the desired driver by passing in `-fsanitize=fuzzer` during the linking stage.

NOTE: `-fsanitize=fuzzer` links in the libFuzzer’s main() symbol.

NOTE: In most cases you may want to combine libFuzzer with AddressSanitizer (ASAN), UndefinedBehaviorSanitizer (UBSAN), or both. You can also build with MemorySanitizer (MSAN), but support is experimental: `-fsanitize=undefined,address,fuzzer`.

Executable repeatedly calls function:

```cpp
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  // DoStuffWithYourAPI(Data, Size);
  return 0;
}
```

Use `-fsanitize=address,fuzzer`. Note that you can change sanitizer (address, memory, thread, etc.)

NOTE: if you suspect memory leaks in your target you should run libFuzzer with `-runs=N` or `-max_total_time=N`. If your target has massive leaks you will eventually run out of RAM. To protect your machine from OOM death you may use e.g. `ASAN_OPTIONS=hard_rss_limit_mb=2000` (with AddressSanitizer).

libFuzzer requires seed corpus, see for details [https://github.com/google/fuzzing/blob/master/tutorial/libFuzzerTutorial.md](https://github.com/google/fuzzing/blob/master/tutorial/libFuzzerTutorial.md)

See for details:

* https://llvm.org/docs/LibFuzzer.html
* https://medium.com/@levwalkin/compile-llvm-clang-libfuzzer-b61e82718430
* https://github.com/Dor1s/libfuzzer-workshop
* [https://github.com/google/fuzzing/blob/master/tutorial/libFuzzerTutorial.md](https://github.com/google/fuzzing/blob/master/tutorial/libFuzzerTutorial.md)

## For contibutors: Prefer Clang To GCC

* Clang supports thread safety annotations (GUARDED_BY)
  1. Enable `-Wthread-safety`
  2. Use `base/thread_annotations.h` https://github.com/chromium/chromium/blob/master/base/thread_annotations.h
  See for details:
  - http://clang.llvm.org/docs/ThreadSafetyAnalysis.html
  - see https://github.com/isocpp/CppCoreGuidelines/blob/master/docs/Lifetime.pdf
  - https://insights.sei.cmu.edu/sei_blog/2014/10/thread-safety-analysis-in-c-and-c.html

* Clang fas first-class support sanitizers
  See details about MSAN, ASAN, TSAN, etc. in docs.

* Clang can be build with lifetime profile:
  1. Build special branch of Clang https://github.com/mgehre/llvm-project
  2. Enable `-Wlifetime`
  See for details:
  - https://pspdfkit.com/blog/2020/the-cpp-lifetime-profile/
  - https://herbsutter.com/2018/09/20/lifetime-profile-v1-0-posted/

## LICENSE for open source components

All the open source components are used under their associated open source licences.

Used open source components:
* icu
* ced
* boost
* harfbuzz
* boost.outcome
* chromium (base)
* libevent
* modp_b64
* tcmalloc
* xdg_mime
* xdg_user_dirs
* dynamic_annotations
* (Facebook) Folly
* (Microsoft) GSL

See LICENSE files

## LICENSE

the MIT license

See LICENSE for the full content of the licenses.

### Contributors List: Example Profile

- I'm an example that you can copy, if you want :)
- I work on many things like...
- My hobbies include...

### Contributors List

#### A

##### Anindita Basu

- Technical writer
- [twitter](https://twitter.com/anindita_basu)
  [github](https://github.com/AninditaBasu)

##### Anna Hayhurst

- Junior Developer
- [github](https://github.com/annahayhurst)

#### C

##### Cassio Espindola

- Analyst Developer
- [github](https://github.com/cassioesp)

#### D

##### Denis trofimov

- C++ Developer
- [github](https://github.com/blockspacer)
- [github](https://github.com/derofim)
- [linkedin](https://www.linkedin.com/in/denis-trofimov-4335bb13b/)

#### M

##### Mohib Qureshi

- Software Development Enthusiast
- [github](https://github.com/mohibqureshi)

#### Y

##### Yousif Alebyary

- Web/Mobile Developer.
- [github](https://github.com/yousifm)

##### Yves Mancera

- Software Engineer
- [github](https://github.com/yvesmh)

## TODOs

```bash
        # TODO: https://github.com/opcm/pcm
        # TODO: https://github.com/dekimir/RamFuzz
        # TODO: https://mull.readthedocs.io/en/latest/HelloWorld.html
        # TODO: CPack https://github.com/mzdun/dashcam-gps/blob/3f6187acf66c16d1ce17db83fa328909e1dfbd68/cmake/packing.cmake
        # TODO: travis / github actions / Jenkinsfile
        # TODO: zzuf https://fuzzing-project.org/tutorial1.html
        # TODO: -fsanitize=fuzzer
        # TODO: Windows support via clang-cl
        # TODO: http://klee.github.io/
        # TODO: Manul https://hakin9.org/manul-fuzzer-for-open-source-and-blackbox-binaries-on-windows-linux-and-macos/
        # TODO: https://github.com/akumuli/Akumuli/tree/master/fuzzers
        # TODO: Dr Memory
        # TODO: http://sandsoftwaresound.net/perf/perf-tutorial-hot-spots/
        # TODO: bench
        # TODO: codacy.com
        # TODO: coveralls.io
        # TODO: gperftools with tcmalloc
        # TODO: gprof
        # TODO: cachegrind and KCachegrind
        # TODO: Massif-Visualizer
        # TODO: pprof https://github.com/google/pprof
        # TODO: Tools for tracing syscalls and libc library calls: strace and ltrace
        # TODO: scan.coverity.com
        # TODO: https://github.com/KDAB/hotspot/
        # TODO: https://oscarforner.com/blog/rr-debugger-and-improvement-over-gdb/
        # TODO: https://github.com/lally/ppt
        # TODO: http://www.brendangregg.com/linuxperf.html
        # TODO: https://github.com/KDE/heaptrack
        # TODO: git-update-ghpages to upload the documentation to gh-pages
        # TODO: https://github.com/skywinder/gitlab-changelog-generator
        # TODO: Probot for automating maintainer tasks such as closing stale issues, requesting missing information, or detecting toxic comments.
        # TODO: Travis for continuous integration on Linux
        # TODO: lcov integration
        # TODO: https://www.sourcemeter.com/
        # TODO: Bloaty McBloatface https://github.com/google/bloaty
        # TODO: https://metrixplusplus.github.io/home.html
        # TODO: https://github.com/TheLartians/PackageProject.cmake
        # TODO: https://github.com/namhyung/uftrace
        # TODO: https://lefticus.gitbooks.io/cpp-best-practices/content/02-Use_the_Tools_Available.html
```
