#include "testing/gtest/include/gtest/gtest.h"

#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>

#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif

#include <memory>

#include <base/i18n/rtl.h>
#include <base/i18n/icu_util.h>
#include <base/i18n/message_formatter.h>
#include <base/strings/string_piece.h>
#include <base/strings/string_util.h>
#include <base/strings/utf_string_conversions.h>
#include <base/time/time.h>
#include <base/path_service.h>
#include <base/files/file_util.h>
#include "base/notreached.h"
#include "base/stl_util.h"
#include "base/logging.h"

#include <third_party/icu/source/common/unicode/unistr.h>
#include <third_party/icu/source/i18n/unicode/datefmt.h>
#include <third_party/icu/source/i18n/unicode/msgfmt.h>
#include <third_party/icu/source/common/unicode/uloc.h>

static const base::FilePath::CharType kIcuDataFileName[]
  = FILE_PATH_LITERAL("./resources/icu/optimal/icudt68l.dat");

static void initICUi18n(
  const base::FilePath::CharType icuFileName[])
{
  base::FilePath dir_exe;
  if (!base::PathService::Get(base::DIR_EXE, &dir_exe)) {
    NOTREACHED();
  }
  if(!base::PathExists(dir_exe.Append(icuFileName))) {
    LOG(WARNING)
      << "unable to find icu i18n file: "
      << dir_exe.Append(icuFileName);
  } else {
    DVLOG(9)
      << "loading icu i18n file: "
      << dir_exe.Append(icuFileName);
  }
  bool icu_initialized
    = base::i18n::InitializeICUWithPath(
        dir_exe.Append(icuFileName));
  if(!icu_initialized) {
    LOG(WARNING)
      << "unable to initialize icu i18n file: "
      << dir_exe.Append(icuFileName);
  }
}

#if !UCONFIG_NO_FORMATTING
TEST(MessageFormatterTest, PluralNumberedArgs) {
  const base::string16 pattern = base::ASCIIToUTF16(
      "{1, plural, "
      "=1 {The cert for {0} expired yesterday.}"
      "=7 {The cert for {0} expired a week ago.}"
      "other {The cert for {0} expired # days ago.}}");
  base::FilePath dir_exe;
  if (!base::PathService::Get(base::DIR_EXE, &dir_exe)) {
    NOTREACHED() << dir_exe;
  }

  if(!base::PathExists(dir_exe.Append(kIcuDataFileName))) {
    NOTREACHED() << dir_exe.Append(kIcuDataFileName);
  }

  base::i18n::AllowMultipleInitializeCallsForTesting();
  initICUi18n(kIcuDataFileName);

  base::i18n::SetICUDefaultLocale(uloc_getDefault());
  std::string result
    = base::UTF16ToASCII(
        base::i18n::MessageFormatter::FormatWithNumberedArgs(
      pattern, "example.com", 1));
  EXPECT_EQ("The cert for example.com expired yesterday.", result);
  result = base::UTF16ToASCII(
    base::i18n::MessageFormatter::FormatWithNumberedArgs(
      pattern, "example.com", 7));
  EXPECT_EQ("The cert for example.com expired a week ago.", result);
  result = base::UTF16ToASCII(
    base::i18n::MessageFormatter::FormatWithNumberedArgs(
      pattern, "example.com", 15));
  EXPECT_EQ("The cert for example.com expired 15 days ago.", result);
}
#else
#warning "UCONFIG_NO_FORMATTING"
#endif // UCONFIG_NO_FORMATTING

