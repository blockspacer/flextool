from conans import ConanFile, CMake, tools, AutoToolsBuildEnvironment, RunEnvironment
from conans.errors import ConanInvalidConfiguration, ConanException
from conans.tools import os_info
import os, re, stat, fnmatch, platform, glob
from functools import total_ordering

class TestPackageConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "cmake_paths", "virtualenv", "cmake_find_package_multi"

    topics = ('c++')

    def build(self):

        bin_path = ""
        for p in self.deps_cpp_info.bin_paths:
            bin_path = "%s%s%s" % (p, os.pathsep, bin_path)

        lib_path = ""
        for p in self.deps_cpp_info.lib_paths:
            lib_path = "%s%s%s" % (p, os.pathsep, lib_path)

        env = {
             "PATH": "%s:%s" % (bin_path, os.environ['PATH']),
             "LD_LIBRARY_PATH": "%s:%s" % (lib_path, os.environ['LD_LIBRARY_PATH'])
        }

        self.output.info("=================linux environment for %s=================\n" % (self.name))
        self.output.info('PATH = %s' % (env['PATH']))
        self.output.info('LD_LIBRARY_PATH = %s' % (env['LD_LIBRARY_PATH']))
        self.output.info('')

        with tools.environment_append(env):
            cmake = CMake(self)
            cmake.configure()
            cmake.build()

    def test(self):
        if not tools.cross_building(self.settings):
            self.output.info('self.source_folder = %s' % (self.source_folder))
            #
            # cling_includes must point to cling/Interpreter/RuntimeUniverse.h
            #
            cling_conan_ROOT = self.deps_cpp_info["cling_conan"].rootpath
            cling_includes = cling_conan_ROOT
            cling_includes = os.path.join(cling_includes, "include")
            self.output.info('cling_includes = %s' % (cling_includes))
            #
            # clang_includes must point to stddef.h from lib/clang/5.0.0/include
            #
            clang_includes = cling_conan_ROOT
            clang_includes = os.path.join(clang_includes, "lib")
            clang_includes = os.path.join(clang_includes, "clang")
            clang_includes = os.path.join(clang_includes, "5.0.0")
            clang_includes = os.path.join(clang_includes, "include")
            self.output.info('clang_includes = %s' % (clang_includes))
            # NOTE: must use `--extra-arg`, not `-extra-arg`
            test_invalid_arg="-extra-arg=-Itest_INVALID_argument"
            #
            # run executable
            # NOTE: generates file in filesystem
            #
            flextool_cmd = "flextool" \
              " --outdir=." \
              " --indir=." \
              " --vmodule=*=100 --enable-logging=stderr --log-level=100" \
              " --extra-arg=-I{}" \
              " {}" \
              " --extra-arg=-I{}" \
              " {}/main.cpp".format(
              cling_includes, test_invalid_arg, clang_includes, self.source_folder)
            self.output.info('flextool_cmd = %s' % (flextool_cmd))
            self.run(flextool_cmd, run_environment=True)
