#pragma once

#if !defined(USE_CATCH_TEST) && !defined(USE_GTEST_TEST)
#warning "use either GTEST or CATCH"
// default to gtest
#define USE_GTEST_TEST 1
#endif // !defined(USE_CATCH_TEST) && !defined(USE_GTEST_TEST)

#if defined(USE_CATCH_TEST)
#include <catch2/catch.hpp>
// include catch2 before fakeit
#include <fakeit.hpp>
using namespace ::fakeit;
using namespace ::Catch;
using namespace ::Catch::Matchers;
#elif defined(USE_GTEST_TEST)
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#else
#error "use either GTEST or CATCH"
#endif // USE_GTEST_TEST && USE_CATCH_TEST

#if defined(USE_GTEST_TEST) && defined(USE_CATCH_TEST)
#error "can't use both GTEST and CATCH at the same time"
#endif // USE_GTEST_TEST && USE_CATCH_TEST

#if !defined(GTEST_HEADER_INCLUDE)
#define GTEST_HEADER_INCLUDE "gtest/gtest.h"
#endif // GTEST_HEADER_INCLUDE

#if !defined(GTEST_HEADER_INCLUDE)
#define GMOCK_HEADER_INCLUDE "gmock/gmock.h"
#endif // GTEST_HEADER_INCLUDE
