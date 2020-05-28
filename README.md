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

We use `buildConanThirdparty.cmake` script to download and install conan packages.

```bash
# NOTE: don't forget to re-run `conan install` or `conan create` after command below
# NOTE: change `build_type=Debug` to `build_type=Release` in production
cmake -DEXTRA_CONAN_OPTS="--profile;clang;-s;build_type=Debug;-s;cling_conan:build_type=Release;--build;missing" -P tools/buildConanThirdparty.cmake
```

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
      -s build_type=Debug -s cling_conan:build_type=Release \
      --profile clang \
          -o flextool:enable_clang_from_conan=False \
          -e flextool:enable_tests=True
```

## Standing on the Shoulders of Giants

That project possible because of [flexferrum's `autoprogrammer`](https://github.com/flexferrum/autoprogrammer).

Articles about flexferrum's `autoprogrammer` in media:

- [RUS] метаклассах по-русски https://habr.com/ru/article/448466/
- [RUS] Метаклассы в C++17 Фантастика? � еальность! https://assets.ctfassets.net/oxjq45e8ilak/55bGdX2PnYzmrpM8rwCjcE/791e7eee3236c2023e86e169faca8a0e/Sergei_Sadovnikov_Metaclasses_in_C___dream_Reality.pdf


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
    -s build_type=Debug -s cling_conan:build_type=Release \
    --profile clang \
        -o flextool:enable_clang_from_conan=False \
        -e flextool:enable_tests=True

CONAN_REVISIONS_ENABLED=1 \
CONAN_VERBOSE_TRACEBACK=1 \
CONAN_PRINT_RUN_COMMANDS=1 \
CONAN_LOGGING_LEVEL=10 \
GIT_SSL_NO_VERIFY=true \
  cmake -E time \
    conan source . --source-folder local_build

conan build . \
  --build-folder local_build

conan package . \
  --build-folder local_build \
  --package-folder local_build/package_dir
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
  --build-folder local_build

conan package . \
  --build-folder local_build \
  --package-folder local_build/package_dir
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

include(\${CMAKE_BINARY_DIR}/conanworkspace.cmake)
conan_workspace_subdirectories()

add_dependencies(flextool flexlib)
EOF

# must contain `include(${CMAKE_BINARY_DIR}/conanworkspace.cmake)` without `\`
cat CMakeLists.txt

# combines options from all projects
conan workspace install \
  ../conanws.yml --profile=clang \
  -s build_type=Debug -s cling_conan:build_type=Release \
    -o openssl:shared=True \
    -o chromium_base:use_alloc_shim=True \
    -o chromium_tcmalloc:use_alloc_shim=True \
    -o flextool:enable_clang_from_conan=False \
    -e flextool:enable_tests=True \
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

# configure via cmake
# NOTE: -DLOCAL_BUILD=TRUE
cmake -E time cmake . \
  -DLOCAL_BUILD=TRUE \
  -DENABLE_TESTS=TRUE \
  -DBUILD_SHARED_LIBS=FALSE \
  -Dflexlib_BUILD_SHARED_LIBS=FALSE \
  -Dflex_reflect_plugin_BUILD_SHARED_LIBS=TRUE \
  -DCONAN_AUTO_INSTALL=OFF \
  -DCMAKE_BUILD_TYPE=${build_type}

# remove generated files
# change paths to yours
# rm ~/flex_typeclass_plugin/build/Debug/*generated*

# optional
# rm CMakeCache.txt

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

include(\${CMAKE_BINARY_DIR}/conanworkspace.cmake)
conan_workspace_subdirectories()

add_dependencies(flextool flexlib)
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
EOF
```

add plugins options to `conan workspace install`:

```bash
# combines options from all projects
conan workspace install \
  ../conanws.yml --profile=clang \
  -s build_type=Debug -s cling_conan:build_type=Release \
    -o openssl:shared=True \
    -o chromium_base:use_alloc_shim=True \
    -o chromium_tcmalloc:use_alloc_shim=True \
    -o flextool:enable_clang_from_conan=False \
    -e flextool:enable_tests=True \
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

# configure via cmake
# NOTE: -DLOCAL_BUILD=TRUE
cmake -E time cmake . \
  -DLOCAL_BUILD=TRUE \
  -DENABLE_TESTS=TRUE \
  -DBUILD_SHARED_LIBS=FALSE \
  -Dflexlib_BUILD_SHARED_LIBS=FALSE \
  -Dflex_reflect_plugin_BUILD_SHARED_LIBS=TRUE \
  -DCONAN_AUTO_INSTALL=OFF \
  -DCMAKE_BUILD_TYPE=${build_type}

# remove generated files
# change paths to yours
# rm ~/flex_typeclass_plugin/build/Debug/*generated*

# optional
# rm CMakeCache.txt

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
