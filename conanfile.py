from conans import ConanFile, CMake, tools, AutoToolsBuildEnvironment, RunEnvironment, python_requires
from conans.errors import ConanInvalidConfiguration, ConanException
from conans.tools import os_info
import os, re, stat, fnmatch, platform, glob, traceback, shutil
from functools import total_ordering

# if you using python less than 3 use from distutils import strtobool
from distutils.util import strtobool

conan_build_helper = python_requires("conan_build_helper/[~=0.0]@conan/stable")

class flextoolConan(conan_build_helper.CMakePackage):
    name = "flextool"
    version = "master"
    url = "https://CHANGE_ME"
    #repo_url = 'https://github.com/google/flatbuffers.git'
    #homepage = "https://github.com/google/flatbuffers"
    topics = ("c++")
    author = "CHANGE_ME <>"
    description = ("TODO.")
    license = "MIT" # CHANGE_ME

    # NOTE: no cmake_find_package due to custom FindXXX.cmake
    generators = "cmake", "cmake_paths", "virtualenv"
    settings = "os_build", "os", "arch", "compiler", "build_type", "arch_build"

    #exports = ["LICENSE.md", "assets"]

    # If the source code is going to be in the same repo as the Conan recipe,
    # there is no need to define a `source` method. The source folder can be
    # defined like this
    exports_sources = ("LICENSE", "VERSION", "*.md", "include/*", "src/*",
                       "cmake/*", "examples/*", "CMakeLists.txt", "tests/*", "benchmarks/*",
                       "scripts/*", "tools/*", "codegen/*", "assets/*",
                       "assets/configuration_files/*", "assets/icu/*", "assets",
                       "docs/*", "licenses/*", "patches/*", "resources/*",
                       "submodules/*", "thirdparty/*", "third-party/*",
                       "third_party/*", "flexlib/*")

    options = {
        "enable_clang_from_conan": [True, False],
        "enable_ubsan": [True, False],
        "enable_asan": [True, False],
        "enable_msan": [True, False],
        "enable_tsan": [True, False],
        "enable_valgrind": [True, False],
        "enable_gold_linker": [True, False],
        # see https://lcamtuf.coredump.cx/afl/
        "enable_fuzz_afl": [True, False],
        # see http://llvm.org/docs/LibFuzzer.html
        "enable_fuzz_libfuzzer": [True, False]
    }

    default_options = (
        #"*:shared=False",
        "enable_gold_linker=False",
        "enable_fuzz_afl=False",
        "enable_fuzz_libfuzzer=False",
        "enable_ubsan=False",
        "enable_asan=False",
        "enable_msan=False",
        "enable_tsan=False",
        "enable_valgrind=False",
        "enable_clang_from_conan=False",
        # boost
        "boost:no_rtti=False",
        "boost:no_exceptions=False",
        "boost:without_python=True",
        "boost:without_test=True",
        "boost:without_coroutine=False",
        "boost:without_stacktrace=False",
        "boost:without_math=False",
        "boost:without_wave=False",
        "boost:without_contract=False",
        "boost:without_locale=False",
        "boost:without_random=False",
        "boost:without_regex=False",
        "boost:without_mpi=False",
        "boost:without_timer=False",
        "boost:without_thread=False",
        "boost:without_chrono=False",
        "boost:without_atomic=False",
        "boost:without_system=False",
        "boost:without_program_options=False",
        "boost:without_serialization=False",
        "boost:without_log=False",
        "boost:without_type_erasure=False",
        "boost:without_graph=False",
        "boost:without_graph_parallel=False",
        "boost:without_iostreams=False",
        "boost:without_context=False",
        "boost:without_fiber=False",
        "boost:without_filesystem=False",
        "boost:without_date_time=False",
        "boost:without_exception=False",
        "boost:without_container=False",
        # llvm
        "llvm:shared=False",
        "compiler-rt:shared=False",
        "clang:shared=False",
        "llvm_headers:shared=False",
        "clang_headers:shared=False",
        "clang_executables:shared=False",
        "llvm_demangle:shared=False",
        "llvm_support:shared=False",
        "llvm_binary_format:shared=False",
        "llvm_core:shared=False",
        "llvm_mc:shared=False",
        "llvm_bit_reader:shared=False",
        "llvm_mc_parser:shared=False",
        "llvm_object:shared=False",
        "llvm_profile_data:shared=False",
        "llvm_analysis:shared=False",
        "llvm_transform_utils:shared=False",
        "llvm_instcombine:shared=False",
        "llvm_bit_writer:shared=False",
        "llvm_target:shared=False",
        "llvm_scalar_opts:shared=False",
        "llvm_option:shared=False",
        "llvm_debuginfo_codeview:shared=False",
        "llvm_codegen:shared=False",
        "llvm_x86_utils:shared=False",
        "llvm_x86_asm_printer:shared=False",
        "llvm_mc_disassembler:shared=False",
        "llvm_debuginfo_msf:shared=False",
        "llvm_global_isel:shared=False",
        "llvm_asm_printer:shared=False",
        "llvm_x86_info:shared=False",
        "llvm_x86_asm_parser:shared=False",
        "llvm_x86_desc:shared=False",
        "llvm_selection_dag:shared=False",
        "clang_lex:shared=False",
        "clang_basic:shared=False",
        "llvm_x86_codegen:shared=False",
        "clang_analysis:shared=False",
        "clang_ast:shared=False",
        # flexlib
        "flexlib:shared=False",
        "flexlib:enable_clang_from_conan=False",
        # FakeIt
        "FakeIt:integration=catch",
        # openssl
        "openssl:shared=True",
        # chromium_base
        "chromium_base:use_alloc_shim=True",
        # chromium_tcmalloc
        "chromium_tcmalloc:use_alloc_shim=True",
    )

    @property
    def _source_dir(self):
        return "."

    @property
    def _build_dir(self):
        return "."

    # installs clang 10 from conan
    def _is_llvm_tools_enabled(self):
      return self._environ_option("ENABLE_LLVM_TOOLS", default = 'false')

    def _is_lwyu_enabled(self):
      return self._environ_option("ENABLE_LWYU", default = 'false')

    def _is_coverage_enabled(self):
      return self._environ_option("USE_COVERAGE", default = 'false')

    def _is_docs_enabled(self):
      return self._environ_option("BUILD_DOXY_DOC", default = 'false')

    def _is_benchmark_enabled(self):
      return self._environ_option("ENABLE_BENCHMARK", default = 'false')

    def _is_ccache_enabled(self):
      return self._environ_option("USE_CCACHE", default = 'false')

    def _is_cppcheck_enabled(self):
      return self._environ_option("ENABLE_CPPCHECK", default = 'false')

    def _is_clang_tidy_enabled(self):
      return self._environ_option("ENABLE_CLANG_TIDY", default = 'false')

    def _is_clang_format_enabled(self):
      return self._environ_option("ENABLE_CLANG_FORMAT", default = 'false')

    def _is_uncrustify_enabled(self):
      return self._environ_option("ENABLE_UNCRUSTIFY", default = 'false')

    def _is_iwyu_enabled(self):
      return self._environ_option("ENABLE_IWYU", default = 'false')

    def _is_cppclean_enabled(self):
      return self._environ_option("ENABLE_CPPCLEAN", default = 'false')

    def _is_lto_enabled(self):
      return self._environ_option("ENABLE_LTO", default = 'false')

    # sets cmake variables required to use clang 10 from conan
    def _is_compile_with_llvm_tools_enabled(self):
      return self._environ_option("COMPILE_WITH_LLVM_TOOLS", default = 'false')

    def configure(self):
        lower_build_type = str(self.settings.build_type).lower()

        if lower_build_type != "release" and not self._is_llvm_tools_enabled():
            self.output.warn('enable llvm_tools for Debug builds')

        if self._is_iwyu_enabled() and (not self._is_llvm_tools_enabled() or not self.options['llvm_tools'].include_what_you_use):
            raise ConanInvalidConfiguration("iwyu requires llvm_tools enabled and -o llvm_tools:include_what_you_use=True")

        if self._is_compile_with_llvm_tools_enabled() and not self._is_llvm_tools_enabled():
            raise ConanInvalidConfiguration("to compile with llvm_tools you must be enable llvm_tools")

        if self.options.enable_valgrind:
            self.options["flexlib"].enable_valgrind = True
            self.options["basis"].enable_valgrind = True
            self.options["chromium_base"].enable_valgrind = True

        if self.options.enable_ubsan \
           or self.options.enable_asan \
           or self.options.enable_msan \
           or self.options.enable_tsan:
            if not self._is_llvm_tools_enabled():
                raise ConanInvalidConfiguration("sanitizers require llvm_tools")

        if self.options.enable_ubsan \
           or self.options.enable_asan \
           or self.options.enable_msan \
           or self.options.enable_tsan:
            if not self.options["boost"].no_exceptions:
                raise ConanInvalidConfiguration("sanitizers require boost without exceptions")

        if self.options.enable_ubsan:
            self.options["flexlib"].enable_ubsan = True
            self.options["basis"].enable_ubsan = True
            self.options["chromium_base"].enable_ubsan = True
            self.options["boost"].enable_ubsan = True
            if self._is_tests_enabled():
              self.options["conan_gtest"].enable_ubsan = True

        if self.options.enable_asan:
            self.options["flexlib"].enable_asan = True
            self.options["basis"].enable_asan = True
            self.options["chromium_base"].enable_asan = True
            self.options["boost"].enable_asan = True
            if self._is_tests_enabled():
              self.options["conan_gtest"].enable_asan = True

        if self.options.enable_msan:
            self.options["flexlib"].enable_msan = True
            self.options["basis"].enable_msan = True
            self.options["chromium_base"].enable_msan = True
            self.options["boost"].enable_msan = True
            if self._is_tests_enabled():
              self.options["conan_gtest"].enable_msan = True

        if self.options.enable_tsan:
            self.options["flexlib"].enable_tsan = True
            self.options["basis"].enable_tsan = True
            self.options["chromium_base"].enable_tsan = True
            self.options["boost"].enable_tsan = True
            if self._is_tests_enabled():
              self.options["conan_gtest"].enable_tsan = True

    def build_requirements(self):
        self.build_requires("cmake_platform_detection/master@conan/stable")
        self.build_requires("cmake_build_options/master@conan/stable")
        self.build_requires("cmake_helper_utils/master@conan/stable")

        if self.options.enable_tsan \
            or self.options.enable_msan \
            or self.options.enable_asan \
            or self.options.enable_ubsan:
          self.build_requires("cmake_sanitizers/master@conan/stable")

        if self._is_cppcheck_enabled():
          self.build_requires("cppcheck_installer/1.90@conan/stable")

        # provides clang-tidy, clang-format, IWYU, scan-build, etc.
        if self._is_llvm_tools_enabled():
          self.build_requires("llvm_tools/master@conan/stable")

    def requirements(self):
        if self._is_tests_enabled():
            self.requires("catch2/[>=2.1.0]@bincrafters/stable")
            self.requires("conan_gtest/release-1.10.0@conan/stable")
            self.requires("FakeIt/[>=2.0.4]@gasuketsu/stable")

        self.requires("chromium_icu/master@conan/stable")

        # TODO: support doctest
        #self.requires("doctest/[>=2.3.8]")

        self.requires("boost/1.71.0@dev/stable")

        self.requires("chromium_build_util/master@conan/stable")

        if self.options.enable_clang_from_conan:
          self.requires("llvm_support/6.0.1@Manu343726/testing")
          self.requires("libclang/6.0.1@Manu343726/testing")
          self.requires("clang_tooling/6.0.1@Manu343726/testing")
          self.requires("clang_tooling_core/6.0.1@Manu343726/testing")
          self.requires("clang_analysis/6.0.1@Manu343726/testing")
          self.requires("clang_ast/6.0.1@Manu343726/testing")
          self.requires("llvm/6.0.1@Manu343726/testing")
        else:
          self.requires("cling_conan/master@conan/stable")

        self.requires("flexlib/master@conan/stable")

        self.requires("chromium_base/master@conan/stable")

        self.requires("basis/master@conan/stable")

        self.requires("corrade/v2020.06@conan/stable")

        #self.requires("type_safe/0.2@conan/stable")

        #self.requires("double-conversion/3.1.1@bincrafters/stable")

        #self.requires("gflags/2.2.2@bincrafters/stable")

        #self.requires("glog/0.4.0@bincrafters/stable")

        # patched to support "openssl/OpenSSL_1_1_1-stable@conan/stable"
        #self.requires("libevent/2.1.11@dev/stable")

        # \note dispatcher must be thread-safe,
        # so use entt after patch https://github.com/skypjack/entt/issues/449
        # see https://github.com/skypjack/entt/commit/74f3df83dbc9fc4b43b8cfb9d71ba02234bd5c4a
        self.requires("entt/3.3.2")

        #self.requires("lz4/1.8.3@bincrafters/stable")

        # must match openssl version used in webrtc
        ##self.requires("openssl/OpenSSL_1_1_1-stable@conan/stable")

        #self.requires("OpenSSL/1.1.1c@conan/stable")

        # patched to support "openssl/OpenSSL_1_1_1-stable@conan/stable"
        #self.requires("zlib/v1.2.11@conan/stable")

        #self.requires("lzma/5.2.4@bincrafters/stable")

        #self.requires("zstd/1.3.8@bincrafters/stable")

        #self.requires("snappy/1.1.7@bincrafters/stable")

        #self.requires("bzip2/1.0.8@dev/stable")

        #self.requires("libsodium/1.0.18@bincrafters/stable")

        #self.requires("libelf/0.8.13@bincrafters/stable")

        #self.requires("libdwarf/20190505@bincrafters/stable")

        #self.requires("clang_folly_conan/v2019.01.14.00@conan/stable")

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.parallel = True
        cmake.verbose = True

        self.add_cmake_option(cmake, "ENABLE_TESTS", self._is_tests_enabled())

        self.add_cmake_option(cmake, "ENABLE_LWYU", self._is_lwyu_enabled())

        self.add_cmake_option(cmake, "USE_COVERAGE", self._is_coverage_enabled())

        self.add_cmake_option(cmake, "BUILD_DOXY_DOC", self._is_docs_enabled())

        self.add_cmake_option(cmake, "ENABLE_BENCHMARK", self._is_benchmark_enabled())

        self.add_cmake_option(cmake, "USE_CCACHE", self._is_ccache_enabled())

        self.add_cmake_option(cmake, "ENABLE_CPPCHECK", self._is_cppcheck_enabled())

        self.add_cmake_option(cmake, "ENABLE_CLANG_TIDY", self._is_clang_tidy_enabled())

        self.add_cmake_option(cmake, "ENABLE_CLANG_FORMAT", self._is_clang_format_enabled())

        self.add_cmake_option(cmake, "ENABLE_UNCRUSTIFY", self._is_uncrustify_enabled())

        self.add_cmake_option(cmake, "ENABLE_IWYU", self._is_iwyu_enabled())

        self.add_cmake_option(cmake, "ENABLE_CPPCLEAN", self._is_cppclean_enabled())

        self.add_cmake_option(cmake, "ENABLE_LTO", self._is_lto_enabled())

        self.add_cmake_option(cmake, "COMPILE_WITH_LLVM_TOOLS", self._is_compile_with_llvm_tools_enabled())

        cmake.definitions["CONAN_AUTO_INSTALL"] = 'OFF'

        if self.settings.compiler == 'gcc':
            cmake.definitions["CMAKE_C_COMPILER"] = "gcc-{}".format(
                self.settings.compiler.version)
            cmake.definitions["CMAKE_CXX_COMPILER"] = "g++-{}".format(
                self.settings.compiler.version)

        if not self.options.enable_clang_from_conan:
          cmake.definitions["ENABLE_CLING"] = 'ON'

        cmake.definitions["ENABLE_UBSAN"] = 'ON'
        if not self.options.enable_ubsan:
            cmake.definitions["ENABLE_UBSAN"] = 'OFF'

        cmake.definitions["ENABLE_ASAN"] = 'ON'
        if not self.options.enable_asan:
            cmake.definitions["ENABLE_ASAN"] = 'OFF'

        cmake.definitions["ENABLE_MSAN"] = 'ON'
        if not self.options.enable_msan:
            cmake.definitions["ENABLE_MSAN"] = 'OFF'

        cmake.definitions["ENABLE_TSAN"] = 'ON'
        if not self.options.enable_tsan:
            cmake.definitions["ENABLE_TSAN"] = 'OFF'

        cmake.definitions["ENABLE_VALGRIND"] = 'ON'
        if not self.options.enable_valgrind:
            cmake.definitions["ENABLE_VALGRIND"] = 'OFF'

        cmake.definitions["USE_LD_GOLD"] = 'ON'
        if not self.options.enable_gold_linker:
            cmake.definitions["USE_LD_GOLD"] = 'OFF'

        cmake.definitions["CMAKE_TOOLCHAIN_FILE"] = 'conan_paths.cmake'

        cmake.configure(build_folder=self._build_dir)
        return cmake

    # Importing files copies files from the local store to your project.
    def imports(self):
        dest = os.getenv("CONAN_IMPORT_PATH", "bin")
        self.copy("license*", dst=dest, ignore_case=True)
        self.copy("*.dll", dst=dest, src="bin")
        self.copy("*.so", dst=dest, src="bin")
        self.copy("*.dylib*", dst=dest, src="lib")
        self.copy("*.lib*", dst=dest, src="lib")
        self.copy("*.a*", dst=dest, src="lib")
        self.copy("assets", dst=dest, src="assets")

    def build(self):
        self.output.info('Building package \'{}\''.format(self.name))

        cmake = self._configure_cmake()

        # NOTE: make sure `protoc` can be found using PATH environment variable
        bin_path = ""
        for p in self.deps_cpp_info.bin_paths:
            bin_path = "%s%s%s" % (p, os.pathsep, bin_path)

        lib_path = ""
        for p in self.deps_cpp_info.lib_paths:
            lib_path = "%s%s%s" % (p, os.pathsep, lib_path)

        # NOTE: make sure `/lib` from `protobuf` can be found using PATH environment variable
        #for p in self.deps_cpp_info["protobuf"].lib_paths:
        #    lib_path = "%s%s%s" % (p, os.pathsep, lib_path)
        #    self.output.info('protobuf lib_path += %s' % (p))
        #    files = [f for f in glob.glob(p + "/**", recursive=True)]
        #    for f in files:
        #        self.output.info('protobuf libs: %s' % (f))

        include_path = ""
        for p in self.deps_cpp_info.includedirs:
            include_path = "%s%s%s" % (p, os.pathsep, include_path)

        # NOTE: make sure `/include` from `protobuf` can be found using PATH environment variable
        #for p in self.deps_cpp_info["protobuf"].include_paths:
        #    include_path = "%s%s%s" % (p, os.pathsep, include_path)

        # see https://docs.conan.io/en/latest/reference/build_helpers/autotools.html
        # AutoToolsBuildEnvironment sets LIBS, LDFLAGS, CFLAGS, CXXFLAGS and CPPFLAGS based on requirements
        env_build = AutoToolsBuildEnvironment(self)
        self.output.info('AutoToolsBuildEnvironment include_paths = %s' % (','.join(env_build.include_paths)))

        env = {
             "LIBS": "%s%s%s" % (env_build.vars["LIBS"] if "LIBS" in env_build.vars else "", " ", os.environ["LIBS"] if "LIBS" in os.environ else ""),
             "LDFLAGS": "%s%s%s" % (env_build.vars["LDFLAGS"] if "LDFLAGS" in env_build.vars else "", " ", os.environ["LDFLAGS"] if "LDFLAGS" in os.environ else ""),
             "CFLAGS": "%s%s%s" % (env_build.vars["CFLAGS"] if "CFLAGS" in env_build.vars else "", " ", os.environ["CFLAGS"] if "CFLAGS" in os.environ else ""),
             "CXXFLAGS": "%s%s%s" % (env_build.vars["CXXFLAGS"] if "CXXFLAGS" in env_build.vars else "", " ", os.environ["CXXFLAGS"] if "CXXFLAGS" in os.environ else ""),
             "CPPFLAGS": "%s%s%s" % (env_build.vars["CPPFLAGS"] if "CPPFLAGS" in env_build.vars else "", " ", os.environ["CPPFLAGS"] if "CPPFLAGS" in os.environ else ""),
             "PATH": "%s%s%s%s%s" % (bin_path, os.pathsep, include_path, os.pathsep, os.environ["PATH"] if "PATH" in os.environ else ""),
             "LD_LIBRARY_PATH": "%s%s%s" % (lib_path, os.pathsep, os.environ["LD_LIBRARY_PATH"] if "LD_LIBRARY_PATH" in os.environ else "")
        }

        self.output.info("=================linux environment for %s=================\n" % (self.name))
        self.output.info('PATH = %s' % (env['PATH']))
        self.output.info('LD_LIBRARY_PATH = %s' % (env['LD_LIBRARY_PATH']))
        self.output.info('')
        #with tools.environment_append(env):
        #    with tools.chdir(self._source_dir):
        #        cmake = self._configure_cmake()
        #        cmake.build()

        cpu_count = tools.cpu_count()
        self.output.info('Detected %s CPUs' % (cpu_count))

        # -j flag for parallel builds
        cmake.build(args=["--", "-j%s" % cpu_count])

    def package(self):
        self.output.info('Packaging package \'{}\''.format(self.name))

        self.copy("LICENSE", dst="licenses", src=self._source_dir)
        #self.copy("*FlatBuffers*", dst="", src=os.path.join(self._source_dir,"CMake"))

        self.output.info('Packaging package \'{}\''.format(self.name))

        self.copy(pattern="LICENSE", dst="licenses")
        self.copy('*', dst='include', src='{}/include'.format(self._source_dir))
        self.copy('*.cmake', dst='lib', src='{}/lib'.format(self._build_dir), keep_path=True)
        self.copy("*.lib", dst="lib", src="", keep_path=False)
        self.copy("*.a", dst="lib", src="", keep_path=False)
        #self.copy("*", dst="bin", src="bin")
        #self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("assets", dst="assets", keep_path=False)

        self.copy("*", dst="bin", src='{}/javascript/net/grpc/web'.format(self._source_dir))

        tools.rmdir(os.path.join(self.package_folder, "lib", "pkgconfig"))

        cmake = self._configure_cmake()
        cmake.install()

        # Local build
        # see https://docs.conan.io/en/latest/developing_packages/editable_packages.html
        if not self.in_local_cache:
            self.copy("conanfile.py", dst=".", keep_path=False)

    def package_id(self):
        del self.info.settings.compiler
        del self.info.settings.arch
        self.info.include_build_settings()

    def package_info(self):
        self.cpp_info.includedirs = ['{}/include'.format(self.package_folder)]
        self.env_info.PATH.append(os.path.join(self.package_folder, "bin"))
        self.env_info.LD_LIBRARY_PATH.append(os.path.join(self.package_folder, "lib"))
        self.env_info.PATH.append(os.path.join(self.package_folder, "lib"))
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.bindirs = ["bin"]
        # collects libupb, make sure to remove 03-simple.a
        self.cpp_info.libs = tools.collect_libs(self)

        for libpath in self.deps_cpp_info.lib_paths:
            self.env_info.LD_LIBRARY_PATH.append(libpath)

        self.env_info.flextool_ROOT = self.package_folder
        flextool = "flextool.exe" if self.settings.os_build == "Windows" else "flextool"
        self.env_info.flextool_BIN = os.path.normpath(os.path.join(self.package_folder, "bin", flextool))

        self.cpp_info.names["cmake_find_package"] = "flextool"
        self.cpp_info.names["cmake_find_package_multi"] = "flextool"
