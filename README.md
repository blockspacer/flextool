# About

C++ tool

## Before installation

## Dependencies

- conan packages

```bash
# NOTE: don't forget to re-run `conan install` after command below
# NOTE: change `build_type=Debug` to `build_type=Release` in production
cmake -DEXTRA_CONAN_OPTS="--profile;clang;-s;build_type=Debug;-s;cling_conan:build_type=Release;--build;missing" -P tools/buildConanThirdparty.cmake
```

- type_safe

```bash
conan remote add Manu343726 https://api.bintray.com/conan/manu343726/conan-packages False

git clone http://github.com/foonathan/type_safe.git -b v0.2.1

cd type_safe

# NOTE: change `build_type=Debug` to `build_type=Release` in production
CONAN_REVISIONS_ENABLED=1 \
    CONAN_VERBOSE_TRACEBACK=1 \
    CONAN_PRINT_RUN_COMMANDS=1 \
    CONAN_LOGGING_LEVEL=10 \
    GIT_SSL_NO_VERIFY=true \
    conan create . conan/stable -s build_type=Debug --profile clang --build missing
```

- corrade

```bash
# NOTE: change `build_type=Debug` to `build_type=Release` in production
git clone http://github.com/mosra/corrade.git && cd corrade
CONAN_REVISIONS_ENABLED=1 \
    CONAN_VERBOSE_TRACEBACK=1 \
    CONAN_PRINT_RUN_COMMANDS=1 \
    CONAN_LOGGING_LEVEL=10 \
    GIT_SSL_NO_VERIFY=true \
    conan create . magnum/stable -s build_type=Debug --profile clang --build missing -tf package/conan/test_package
```

## Installation

```bash
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
          -o shared=False \
          -o enable_clang_from_conan=False \
          -o enable_tests=True \
          -o *:use_alloc_shim=True
```
