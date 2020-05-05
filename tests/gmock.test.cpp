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
