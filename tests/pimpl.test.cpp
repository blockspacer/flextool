#include "testsCommon.h"

#if !defined(USE_CATCH_TEST)
#warning "use USE_CATCH_TEST"
// default
#define USE_CATCH_TEST 1
#endif // !defined(USE_CATCH_TEST)

#include "basis/core/pimpl.hpp"

#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

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

// Foo.h:
class Foo {
public:
  Foo();
  ~Foo();
  int foo();
private:
  pimpl::FastPimpl<
      class FooImpl
      , /*Size*/1
      , /*Alignment*/1
    > m_impl;
};

// Foo.cpp:
class FooImpl
{
 public:
  FooImpl() = default;
  ~FooImpl() = default;
  int foo();
};

int FooImpl::foo() {
  return 123;
}

Foo::Foo() = default;
Foo::~Foo() = default;
int Foo::foo() {
  return m_impl->foo();
}

SCENARIO("pimpl", "[basic]") {

  GIVEN("FastPimplExample") {
    Foo foo;

    REQUIRE(foo.foo() == 123);
  }
}
