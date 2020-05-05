#include "testsCommon.h"

#if !defined(USE_CATCH_TEST)
#warning "use USE_CATCH_TEST"
// default
#define USE_CATCH_TEST 1
#endif // !defined(USE_CATCH_TEST)

#include <chrono>
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

struct SomeInterface {
  virtual int foo(int) = 0;
  virtual int bar(std::string) = 0;
};

SCENARIO("utils", "[network]") {

  GIVEN("mockExample") {
    // Instantiate a mock object.
    Mock<SomeInterface> mock;
    // mock.get().bar("");

    // Setup mock behavior.
    When(Method(mock, foo)).Return(1); // Method mock.foo will return 1 once.

    // Fetch the mock instance.
    SomeInterface& i = mock.get();

    // Will print "1".
    REQUIRE(i.foo(0) == 1);
  }
}
