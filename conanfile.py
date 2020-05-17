from conans import ConanFile, CMake, tools, AutoToolsBuildEnvironment, RunEnvironment, python_requires
from conans.errors import ConanInvalidConfiguration, ConanException
from conans.tools import os_info
import os, re, stat, fnmatch, platform, glob
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

    generators = "cmake", "cmake_paths", "virtualenv"#, "cmake_find_package_multi"
    settings = "os_build", "os", "arch", "compiler", "build_type", "arch_build"

    #exports = ["LICENSE.md", "assets"]

    # If the source code is going to be in the same repo as the Conan recipe,
    # there is no need to define a `source` method. The source folder can be
    # defined like this
    exports_sources = ("LICENSE", "*.md", "include/*", "src/*",
                       "cmake/*", "CMakeLists.txt", "tests/*", "benchmarks/*",
                       "scripts/*", "tools/*", "codegen/*", "assets/*",
                       "assets/configuration_files/*", "assets/icu/*", "assets",
                       "docs/*", "licenses/*", "patches/*", "resources/*",
                       "submodules/*", "thirdparty/*", "third-party/*",
                       "third_party/*", "base/*", "build/*", "flexlib/*")

    options = {
        "use_system_boost": [True, False],
        "enable_clang_from_conan": [True, False],
        "enable_sanitizers": [True, False]
    }

    default_options = (
        #"*:shared=False",
        "enable_clang_from_conan=False",
        "use_system_boost=False",
        "enable_sanitizers=False",
        # boost
        "boost:without_atomic=True",
        "boost:without_chrono=True",
        "boost:without_container=True",
        "boost:without_context=True",
        "boost:without_coroutine=True",
        "boost:without_graph=True",
        "boost:without_graph_parallel=True",
        "boost:without_log=True",
        "boost:without_math=True",
        "boost:without_mpi=True",
        "boost:without_serialization=True",
        "boost:without_test=True",
        "boost:without_timer=True",
        "boost:without_type_erasure=True",
        "boost:without_wave=True",
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

    generators = 'cmake_find_package', "cmake", "cmake_paths"

    @property
    def _source_dir(self):
        return "."

    @property
    def _build_dir(self):
        return "."

    def build_requirements(self):
        self.build_requires("cmake_platform_detection/master@conan/stable")
        self.build_requires("cmake_build_options/master@conan/stable")

    def requirements(self):
        if self._is_tests_enabled():
            self.requires("catch2/[>=2.1.0]@bincrafters/stable")
            self.requires("gtest/[>=1.8.0]@bincrafters/stable")
            self.requires("FakeIt/[>=2.0.4]@gasuketsu/stable")

        if not self.options.use_system_boost:
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

        self.requires("corrade/2019.10@magnum/stable")

        self.requires("type_safe/0.2@conan/stable")

        self.requires("double-conversion/3.1.1@bincrafters/stable")

        self.requires("gflags/2.2.2@bincrafters/stable")

        self.requires("glog/0.4.0@bincrafters/stable")

        # patched to support "openssl/OpenSSL_1_1_1-stable@conan/stable"
        #self.requires("libevent/2.1.11@dev/stable")

        # \note dispatcher must be thread-safe,
        # so use entt after patch https://github.com/skypjack/entt/issues/449
        # see https://github.com/skypjack/entt/commit/74f3df83dbc9fc4b43b8cfb9d71ba02234bd5c4a
        self.requires("entt/3.3.2")

        self.requires("lz4/1.8.3@bincrafters/stable")

        # must match openssl version used in webrtc
        ##self.requires("openssl/OpenSSL_1_1_1-stable@conan/stable")

        #self.requires("OpenSSL/1.1.1c@conan/stable")

        # patched to support "openssl/OpenSSL_1_1_1-stable@conan/stable"
        self.requires("zlib/v1.2.11@conan/stable")

        self.requires("lzma/5.2.4@bincrafters/stable")

        self.requires("zstd/1.3.8@bincrafters/stable")

        self.requires("snappy/1.1.7@bincrafters/stable")

        self.requires("bzip2/1.0.8@dev/stable")

        self.requires("libsodium/1.0.18@bincrafters/stable")

        self.requires("libelf/0.8.13@bincrafters/stable")

        self.requires("libdwarf/20190505@bincrafters/stable")

        self.requires("clang_folly_conan/v2019.01.14.00@conan/stable")

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.parallel = True
        cmake.verbose = True

        self.add_cmake_option(cmake, "ENABLE_TESTS", self._is_tests_enabled())

        cmake.definitions["CONAN_AUTO_INSTALL"] = 'OFF'

        if self.settings.compiler == 'gcc':
            cmake.definitions["CMAKE_C_COMPILER"] = "gcc-{}".format(
                self.settings.compiler.version)
            cmake.definitions["CMAKE_CXX_COMPILER"] = "g++-{}".format(
                self.settings.compiler.version)

        if not self.options.enable_clang_from_conan:
          cmake.definitions["ENABLE_CLING"] = 'ON'

        cmake.definitions["ENABLE_SANITIZERS"] = 'ON'
        if not self.options.enable_sanitizers:
            cmake.definitions["ENABLE_SANITIZERS"] = 'OFF'

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
