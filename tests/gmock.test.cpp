#include "testsCommon.h"

#if !defined(USE_GTEST_TEST)
#warning "use USE_GTEST_TEST"
// default
#define USE_GTEST_TEST 1
#endif // !defined(USE_GTEST_TEST)

#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>

#include <base/threading/thread.h>
#include <base/logging.h>

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

class Producer {
public:
  virtual ~Producer(){}
  virtual std::string getDomainFromUrl(const std::string & url) const = 0;
};

class Consumer {
public:
  Consumer(Producer * p): producer(p){}
  int countLevelOfDomain(const std::string & url) const {
    std::string domain = producer->getDomainFromUrl(url);
    return std::count(domain.begin(), domain.end(), '.') + 1;
  }
private:
  Producer * producer;
};

class ProducerMock : public Producer {
public:
  MOCK_CONST_METHOD1( getDomainFromUrl, std::string(const std::string & url) );
};

TEST(Consumer, CalculateDomainLevel) {
  const std::string url = "http://www.fantasticdomain.com/site/index.html";
  std::string domain = "fantasticdomain.com";
  ProducerMock mock;
  Consumer consumer(&mock);
  EXPECT_CALL(mock, getDomainFromUrl(url))
        .WillOnce(::testing::Return(domain));
  int domainLevel = consumer.countLevelOfDomain(url);
  EXPECT_EQ(domainLevel, 2);
}

TEST(ToolsSanityTest, DISABLED_AddressSanitizerTest) {
  // Intentionally crash to make sure AddressSanitizer is instrumenting
  // the local variables.
  // This test should not be ran on bots.
  int array[5];
  // Work around the OOB warning reported by Clang.
  int* volatile access = &array[5];
  *access = 43;
}

TEST(ToolsSanityTest, DISABLED_MemorySanitizerTest) {
  // Without the |volatile|, clang optimizes away the next two lines.
  int* volatile leak = new int[256];  // Leak some memory intentionally.
  leak[4] = 1;  // Make sure the allocated memory is used.
}

TEST(ToolsSanityTest, DISABLED_ValgrindTest) {
  // Without the |volatile|, clang optimizes away the next two lines.
  int* volatile leak = new int[256];  // Leak some memory intentionally.
  leak[4] = 1;  // Make sure the allocated memory is used.
}

TEST(ToolsSanityTest, DISABLED_UndefinedBehaviorSanitizerTest) {
  // divide by zero
  int n = 42;
  int d = 0;
  auto f = n/d;
  EXPECT_NE(f, 2); // do not optimize out
}

#if defined(OS_POSIX)
namespace {

typedef std::map<std::string, std::string> map_t;

void *threadfunc(void *p) {
  map_t& m = *(map_t*)p;
  m["foo"] = "bar";
  return 0;
}

}  // namespace

#include <pthread.h>
#include <stdio.h>
#include <string>
#include <map>

TEST(ToolsSanityTest, DISABLED_ThreadSanitizerTest) {
  // Example of a data race that can lead to crashes and memory corruptions
  // See https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual
  map_t m;
  pthread_t t;
  pthread_create(&t, 0, threadfunc, &m);
  printf("foo=%s\n", m["foo"].c_str());
  pthread_join(t, 0);
}
#endif // OS_POSIX
