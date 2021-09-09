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
        # llvm package must provide "include/c++/v1"
        self.build_requires("llvm_9/master@conan/stable")

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
          llvm_root = self.deps_cpp_info["llvm_9"].rootpath
          # First, runs tool from conanfile.
          # Second, runs tool from CMake.
          env.vars["llvm_dir_flextool".upper()]=os.path.abspath(llvm_root)
          with tools.environment_append(dict(env.vars, **{\
            "llvm_dir_flextool".upper(): os.path.abspath(llvm_root)\
            #"llvm_dir_flextool".upper(): os.path.abspath("/usr/lib/llvm-9/")\
            })):
            self.output.info(os.environ)
            self.output.info('self.package_folder = %s' % (self.package_folder))
            self.output.info('self.source_folder = %s' % (self.source_folder))

            bin_path = os.path.join(self.build_folder, "bin", "flextool_test_package")
            self.run("%s -s" % bin_path, run_environment=True)

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
              #" --extra-arg=-I{}" \
              #" --extra-arg=-I{}".format(self.source_folder, \
              #os.path.join(llvm_root, "include/c++/v1"),
              #os.path.join(llvm_root, "include"),
              #os.path.join(llvm_root, "lib/clang/9.0.1/include"),
              #cling_includes

            flextool_cmd = os.environ['flextool_BIN']
            flextool_cmd = flextool_cmd +  \
              " --outdir=." \
              " --indir=." \
              " --vmodule=*=9999 --enable-logging=stderr --log-level=100"
            flextool_cmd = flextool_cmd + " --extra-arg=-std=c++17"
            #flextool_cmd = flextool_cmd + " --extra-arg=-std=gnu11"

            # flextool_cmd = flextool_cmd + " --extra-arg=-D_GNU_SOURCE=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D_LP64=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D_POSIX_SOURCE=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D_STDC_PREDEF_H=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D_XOPEN_SOURCE=700"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__ATOMIC_ACQUIRE=2"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__ATOMIC_ACQ_REL=4"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__ATOMIC_CONSUME=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__ATOMIC_RELAXED=0"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__ATOMIC_RELEASE=3"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__ATOMIC_SEQ_CST=5"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__BIGGEST_ALIGNMENT__=16"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__CHAR_BIT__=8"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__CONSTANT_CFSTRINGS__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__DBL_DECIMAL_DIG__=17"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__DBL_DENORM_MIN__=4.9406564584124654e-324"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__DBL_DIG__=15"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__DBL_EPSILON__=2.2204460492503131e-16"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__DBL_HAS_DENORM__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__DBL_HAS_INFINITY__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__DBL_HAS_QUIET_NAN__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__DBL_MANT_DIG__=53"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__DBL_MAX_10_EXP__=308"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__DBL_MAX_EXP__=1024"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__DBL_MAX__=1.7976931348623157e+308"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__DBL_MIN__=2.2250738585072014e-308"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__DEPRECATED=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__ELF__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__EXCEPTIONS=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__FINITE_MATH_ONLY__=0"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__FLT_DECIMAL_DIG__=9"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__FLT_DENORM_MIN__=1.40129846e-45F"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__FLT_DIG__=6"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__FLT_EPSILON__=1.19209290e-7F"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__FLT_EVAL_METHOD__=0"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__FLT_HAS_DENORM__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__FLT_HAS_INFINITY__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__FLT_HAS_QUIET_NAN__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__FLT_MANT_DIG__=24"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__FLT_MAX_10_EXP__=38"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__FLT_MAX_EXP__=128"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__FLT_MAX__=3.40282347e+38F"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__FLT_MIN__=1.17549435e-38F"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__FLT_RADIX__=2"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__FXSR__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GCC_ASM_FLAG_OUTPUTS__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GCC_ATOMIC_BOOL_LOCK_FREE=2"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GCC_ATOMIC_CHAR16_T_LOCK_FREE=2"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GCC_ATOMIC_CHAR32_T_LOCK_FREE=2"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GCC_ATOMIC_CHAR_LOCK_FREE=2"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GCC_ATOMIC_INT_LOCK_FREE=2"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GCC_ATOMIC_LLONG_LOCK_FREE=2"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GCC_ATOMIC_LONG_LOCK_FREE=2"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GCC_ATOMIC_POINTER_LOCK_FREE=2"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GCC_ATOMIC_SHORT_LOCK_FREE=2"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GCC_ATOMIC_TEST_AND_SET_TRUEVAL=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GCC_ATOMIC_WCHAR_T_LOCK_FREE=2"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GNUC_GNU_INLINE__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GNUC_MINOR__=2"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GNUC_PATCHLEVEL__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GNUC__=4"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GNUG__=4"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GNU_LIBRARY__=6"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GXX_ABI_VERSION=1002"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GXX_EXPERIMENTAL_CXX0X__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GXX_RTTI=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GXX_TYPEINFO_EQUALITY_INLINE=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GXX_WEAK__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT16_MAX__=32767"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT16_TYPE__=short"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT32_MAX__=2147483647"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT32_TYPE__=int"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT64_C_SUFFIX__=L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT64_MAX__=9223372036854775807L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT8_MAX__=127"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INTMAX_MAX__=9223372036854775807L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INTMAX_WIDTH__=64"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INTPTR_MAX__=9223372036854775807L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INTPTR_WIDTH__=64"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT_FAST16_MAX__=32767"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT_FAST16_TYPE__=short"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT_FAST32_MAX__=2147483647"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT_FAST32_TYPE__=int"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT_FAST64_MAX__=9223372036854775807L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT_FAST8_MAX__=127"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT_LEAST16_MAX__=32767"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT_LEAST16_TYPE__=short"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT_LEAST32_MAX__=2147483647"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT_LEAST32_TYPE__=int"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT_LEAST64_MAX__=9223372036854775807L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT_LEAST8_MAX__=127"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__INT_MAX__=2147483647"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__LDBL_DENORM_MIN__=3.64519953188247460253e-4951L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__LDBL_DIG__=18"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__LDBL_EPSILON__=1.08420217248550443401e-19L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__LDBL_HAS_DENORM__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__LDBL_HAS_INFINITY__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__LDBL_HAS_QUIET_NAN__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__LDBL_MANT_DIG__=64"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__LDBL_MAX_10_EXP__=4932"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__LDBL_MAX_EXP__=16384"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__LDBL_MAX__=1.18973149535723176502e+4932L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__LDBL_MIN__=3.36210314311209350626e-4932L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__LITTLE_ENDIAN__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__LONG_LONG_MAX__=9223372036854775807LL"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__LONG_MAX__=9223372036854775807L"
            flextool_cmd = flextool_cmd + " --extra-arg=-D__LP64__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__MMX__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__NO_MATH_INLINES=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__ORDER_BIG_ENDIAN__=4321"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__ORDER_LITTLE_ENDIAN__=1234"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__ORDER_PDP_ENDIAN__=3412"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__POINTER_WIDTH__=64"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__PRAGMA_REDEFINE_EXTNAME=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__PTRDIFF_MAX__=9223372036854775807L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__PTRDIFF_WIDTH__=64"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__REGISTER_PREFIX__="
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SCHAR_MAX__=127"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SEG_FS=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SEG_GS=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SHRT_MAX__=32767"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIG_ATOMIC_MAX__=2147483647"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIG_ATOMIC_WIDTH__=32"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZEOF_DOUBLE__=8"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZEOF_FLOAT128__=16"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZEOF_FLOAT__=4"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZEOF_INT128__=16"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZEOF_INT__=4"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZEOF_LONG_DOUBLE__=16"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZEOF_LONG_LONG__=8"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZEOF_LONG__=8"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZEOF_POINTER__=8"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZEOF_PTRDIFF_T__=8"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZEOF_SHORT__=2"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZEOF_SIZE_T__=8"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZEOF_WCHAR_T__=4"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZEOF_WINT_T__=4"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZE_MAX__=18446744073709551615UL"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZE_WIDTH__=64"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SSE2_MATH__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SSE2__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SSE_MATH__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__SSE__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__STDCPP_DEFAULT_NEW_ALIGNMENT__=16UL"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__STDC_HOSTED__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__STDC_IEC_559_COMPLEX__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__STDC_IEC_559__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__STDC_ISO_10646__=201706L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__STDC_UTF_16__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__STDC_UTF_32__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__STDC__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__UINT16_MAX__=65535"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__UINT32_MAX__=4294967295U"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__UINT64_MAX__=18446744073709551615UL"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__UINT8_MAX__=255"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__UINTMAX_MAX__=18446744073709551615UL"
            #
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__UINTPTR_MAX__=18446744073709551615UL"
            #
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__UINT_FAST16_MAX__=65535"
            #
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__UINT_FAST32_MAX__=4294967295U"
            #
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__UINT_FAST64_MAX__=18446744073709551615UL"
            #
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__UINT_FAST8_MAX__=255"
            #
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__UINT_LEAST16_MAX__=65535"
            #
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__UINT_LEAST32_MAX__=4294967295U"
            #
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__UINT_LEAST64_MAX__=18446744073709551615UL"
            #
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__UINT_LEAST8_MAX__=255"
            #
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__USER_LABEL_PREFIX__="
            #
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__WCHAR_MAX__=2147483647"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__WCHAR_TYPE__=int"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__WCHAR_WIDTH__=32"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__WINT_MAX__=4294967295U"
            #
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__WINT_WIDTH__=32"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__amd64=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__amd64__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__clang__=1"
            #
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cplusplus=201703L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_aggregate_nsdmi=201304L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_alias_templates=200704L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_attributes=200809L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_binary_literals=201304L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_constexpr=201603L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_decltype=200707L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_decltype_auto=201304L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_delegating_constructors=200604L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_digit_separators=201309L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_exceptions=199711L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_generic_lambdas=201304L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_hex_float=201603L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_inheriting_constructors=201511L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_init_captures=201304L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_initializer_lists=200806L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_lambdas=200907L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_nsdmi=200809L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_range_based_for=201603L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_raw_strings=200710L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_ref_qualifiers=200710L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_return_type_deduction=201304L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_rtti=199711L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_static_assert=201411L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_unicode_characters=200704L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_unicode_literals=200710L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_user_defined_literals=200809L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_variable_templates=201304L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__cpp_variadic_templates=200704L"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__gnu_linux__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__k8=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__k8__=1"
            flextool_cmd = flextool_cmd + " --extra-arg=-D__linux=1"
            flextool_cmd = flextool_cmd + " --extra-arg=-D__linux__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__llvm__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__private_extern__=extern"
            flextool_cmd = flextool_cmd + " --extra-arg=-D__unix=1"
            flextool_cmd = flextool_cmd + " --extra-arg=-D__unix__=1"
            flextool_cmd = flextool_cmd + " --extra-arg=-D__x86_64=1"
            flextool_cmd = flextool_cmd + " --extra-arg=-D__x86_64__=1"
            #
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__CLANG__"
            #flextool_cmd = flextool_cmd + " --extra-arg=-D__x86_64__=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D__GLIBCXX__=1"
            # #flextool_cmd = flextool_cmd + " --extra-arg=-D__GLIBCXX__=20200808"
            # #flextool_cmd = flextool_cmd + " --extra-arg=-D__GLIBC__=2"
            # # _CRT_MSVCP_CURRENT
            flextool_cmd = flextool_cmd + " --extra-arg=-D_LIBCPP_ABI_VERSION=1"
            flextool_cmd = flextool_cmd + " --extra-arg=-D_LIBCPP_BEGIN_NAMESPACE_STD='namespace std {'"
            flextool_cmd = flextool_cmd + " --extra-arg=-D_LIBCPP_END_NAMESPACE_STD='}'"

            # flextool_cmd = flextool_cmd + " --extra-arg=-D_GLIBCXX_USE_CXX11_ABI=1"
            # #flextool_cmd = flextool_cmd + " --extra-arg=-fPIC"
            #
            # # TODO: requires /usr/include/features.h
            flextool_cmd = flextool_cmd + " --extra-arg=-nostdinc++"
            # #flextool_cmd = flextool_cmd + " --extra-arg=-nostdinc"
            #
            flextool_cmd = flextool_cmd + " --extra-arg=-stdlib=libc++"
            # flextool_cmd = flextool_cmd + " --extra-arg=-stdlib=libstdc++"
            # #flextool_cmd = flextool_cmd + " --extra-arg=-stdlib=libstdc++11"
            flextool_cmd = flextool_cmd + " --extra-arg=-m64"
            #
            # flextool_cmd = flextool_cmd + " --extra-arg=-DBASE_PORT=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DBOOST_ALL_NO_LIB=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DCONAN_PERFETTO=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DCORRADE_IS_DEBUG_BUILD=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DDYNAMIC_ANNOTATIONS_ENABLED=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DHAVE_MMAP=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DLIB_ICU_I18N_STATIC=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DUCONFIG_USE_WINDOWS_LCID_MAPPING_API=0"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DUSE_CUSTOM_ICU=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DUSE_OPENSSL=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DUSE_SYMBOLIZE=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DU_CHARSET_IS_UTF8=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DU_ENABLE_DYLOAD=0"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DU_ENABLE_RESOURCE_TRACING=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DU_ENABLE_TRACING=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-D_USE_MATH_DEFINES=1"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DU_USING_ICU_NAMESPACE=0"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DU_IMPORT=U_EXPORT"
            # flextool_cmd = flextool_cmd + " --extra-arg=-DICU_UTIL_DATA_IMPL=ICU_UTIL_DATA_FILE"
            # # # path to features.h

            # flextool_cmd = flextool_cmd + " --extra-arg=-I{}".format("/usr/include/c++/9/tr1/")
            # flextool_cmd = flextool_cmd + " --extra-arg=-I{}".format("/usr/include/linux")
            # flextool_cmd = flextool_cmd + " --extra-arg=-I{}".format("/usr/include/x86_64-linux-gnu")
            # flextool_cmd = flextool_cmd + " --extra-arg=-I{}".format("/usr/lib/gcc/x86_64-linux-gnu/9/include")

            #flextool_cmd = flextool_cmd + " --extra-arg=-D__SIZE_TYPE__=long unsigned int"

            # # -m64 -stdlib=libstdc++
            # # -DBOOST_ALL_NO_LIB=1 -DCONAN_PERFETTO=1
            #flextool_cmd = flextool_cmd + " --extra-arg=-I{}".format("/mnt/hdd/usr/lib/llvm-10/lib/clang/10.0.0/include")

            flextool_cmd = flextool_cmd + " --extra-arg=-isystem{}".format(os.path.join(llvm_root, "clang/lib/Headers"))
            #flextool_cmd = flextool_cmd + " --extra-arg=-I{}".format(os.path.join(llvm_root, "lib/clang/9.0.1/include"))
            flextool_cmd = flextool_cmd + " --extra-arg=-isystem{}".format(cling_includes)

            flextool_cmd = flextool_cmd + " --extra-arg=-I{}".format(llvm_root)
            flextool_cmd = flextool_cmd + " --extra-arg=-I{}".format(os.path.join(llvm_root, "include"))
            flextool_cmd = flextool_cmd + " --extra-arg=-I{}".format(os.path.join(llvm_root, "include/c++"))
            flextool_cmd = flextool_cmd + " --extra-arg=-I{}".format(os.path.join(llvm_root, "include/c++/v1"))

            #flextool_cmd = flextool_cmd + " --extra-arg=-I{}".format(clang_includes)
            flextool_cmd = flextool_cmd + " --extra-arg=-lc++"
            flextool_cmd = flextool_cmd + " --extra-arg=-lci++abi"
            flextool_cmd = flextool_cmd + " --extra-arg=-lm"
            flextool_cmd = flextool_cmd + " --extra-arg=-lc"
            flextool_cmd = flextool_cmd + " --extra-arg=-Wno-unused-command-line-argument"
            flextool_cmd = flextool_cmd + " --extra-arg=-Wno-unknown-warning-option"
            #flextool_cmd = flextool_cmd + " --extra-arg=-L{}/lib".format(llvm_root)

            #flextool_cmd = flextool_cmd + " --extra-arg=-isysroot{}".format(llvm_root)

            #flextool_cmd = flextool_cmd + " --extra-arg=-Wl,-rpath,{}/lib".format(llvm_root)
            #flextool_cmd = flextool_cmd + " --extra-arg=-resource-dir{}/lib/clang/9.0.1".format(llvm_root)
            flextool_cmd = flextool_cmd + " {}/test_package_cling.cpp".format(self.source_folder)
            self.output.info('flextool_cmd = %s' % (flextool_cmd))
            self.run(flextool_cmd, run_environment=True)
