&nbsp;
<p align="center">
  <a href="https://blockspacer.github.io/flex_docs">
    <img src="https://blockspacer.github.io/flex_docs/images/logo.png" width="100px" alt="flextool" />
  </a>
</p>
<h3 align="center">Add custom features to C++ language, like <a href="https://www.fluentcpp.com/2018/03/09/c-metaclasses-proposal-less-5-minutes/">metaclasses</a>, <a href="https://twitter.com/TartanLlama/status/1159457033441165313">Rust-like traits</a>, reflection and many more</h3>
<p align="center">A fully open source, powerful solution for modification and generation of C++ source code. Reduce the amount of boilerplate code in your C++ projects.</p>
<hr />

﻿![Open Source Love](https://img.shields.io/badge/Open%20Source-%E2%9D%A4-pink.svg)
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
- [RUS] Метаклассы в C++17 Фантастика? Реальность! https://assets.ctfassets.net/oxjq45e8ilak/55bGdX2PnYzmrpM8rwCjcE/791e7eee3236c2023e86e169faca8a0e/Sergei_Sadovnikov_Metaclasses_in_C___dream_Reality.pdf

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

### Contributors List 👌

#### A

##### Anindita Basu

- Technical writer
- [twitter](https://twitter.com/anindita_basu)
  [github](https://github.com/AninditaBasu)

##### Anna Hayhurst

- Junior Developer
- [github](https://github.com/annahayhurst)

#### C

##### Cássio Espíndola

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
