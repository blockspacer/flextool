from conans import ConanFile, CMake, tools
import os

from conans import ConanFile, CMake, tools, AutoToolsBuildEnvironment, RunEnvironment, python_requires
from conans.errors import ConanInvalidConfiguration, ConanException
from conans.tools import os_info
import os, re, stat, fnmatch, platform, glob, traceback, shutil
from functools import total_ordering

# if you using python less than 3 use from distutils import strtobool
from distutils.util import strtobool

class TestPackageConan(ConanFile):
    name = "flextool_test_package"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "cmake_paths", "virtualenv", "cmake_find_package_multi"

    def build_requirements(self):
        self.build_requires("cmake_platform_detection/master@conan/stable")
        self.build_requires("cmake_build_options/master@conan/stable")
        self.build_requires("cmake_helper_utils/master@conan/stable")

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
          env = RunEnvironment(self)
          # First, runs tool from conanfile.
          # Second, runs tool from CMake.
          with tools.environment_append(env.vars):
            # self.output.info(os.environ)
            self.output.info('self.package_folder = %s' % (self.package_folder))
            self.output.info('self.source_folder = %s' % (self.source_folder))

            #
            # cling_includes must point to cling/Interpreter/RuntimeUniverse.h
            #
            cling_conan_ROOT = self.deps_cpp_info["cling_conan"].rootpath
            cling_includes = cling_conan_ROOT
            cling_includes = os.path.join(cling_includes, "include")
            self.output.info('cling_includes = %s' % (cling_includes))
            #
            # clang_includes must point to stddef.h from lib/clang/x.x.x/include
            #
            clang_includes = cling_conan_ROOT
            clang_includes = os.path.join(clang_includes, "lib")
            clang_includes = os.path.join(clang_includes, "clang")
            clang_includes = os.path.join(clang_includes, "9.0.1")
            clang_includes = os.path.join(clang_includes, "include")
            if not os.path.isdir(clang_includes):
              raise Exception('ERROR: Unable to find directory %s' % (clang_includes))
            self.output.info('clang_includes = %s' % (clang_includes))
            #
            # run executable
            # NOTE: generates file in filesystem
            #
            flextool_cmd = os.environ['flextool_BIN'] + \
              " {}/test_package.cpp" \
              " --outdir=." \
              " --indir=." \
              " --vmodule=*=9999 --enable-logging=stderr --log-level=100" \
              " --extra-arg=-I{}" \
              " --extra-arg=-I{}".format(self.source_folder, \
              cling_includes, clang_includes)
            self.output.info('flextool_cmd = %s' % (flextool_cmd))
            self.run(flextool_cmd, run_environment=True)

            bin_path = os.path.join(self.build_folder, "bin", "flextool_test_package")
            self.run("%s -s" % bin_path, run_environment=True)
