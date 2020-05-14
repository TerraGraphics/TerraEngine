#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wctor-dtor-privacy"
#endif
#include <gtest/gtest.h>
#pragma GCC diagnostic pop


// see TYPED_TEST_CASE
#define TYPED_TEST_SUITE(CaseName, ...) \
  typedef ::testing::internal::TypeList< ::testing::Types<__VA_ARGS__> >::type GTEST_TYPE_PARAMS_(CaseName); \
  typedef ::testing::internal::NameGeneratorSelector<>::type GTEST_NAME_GENERATOR_(CaseName)
