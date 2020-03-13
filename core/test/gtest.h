#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#include <gtest/gtest.h>
#pragma clang diagnostic pop


// see TYPED_TEST_CASE
#define TYPED_TEST_SUITE(CaseName, ...) \
  typedef ::testing::internal::TypeList< ::testing::Types<__VA_ARGS__> >::type GTEST_TYPE_PARAMS_(CaseName); \
  typedef ::testing::internal::NameGeneratorSelector<>::type GTEST_NAME_GENERATOR_(CaseName)
