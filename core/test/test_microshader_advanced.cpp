#include <gtest/gtest.h>

#include "core/math/advanced.h"

namespace {

TEST(MathAdvanced, TestQuadEquation) {
    dg::float2 result;

    result = dg::float2(0, 0);
    ASSERT_FALSE(math::QuadEquation(dg::float3(1.f, 2.f, 3.f), result));

    result = dg::float2(0, 0);
    ASSERT_TRUE(math::QuadEquation(dg::float3(1.f, 6.f, 9.f), result));
    ASSERT_FLOAT_EQ(result.x, -3.f);
    ASSERT_FLOAT_EQ(result.y, -3.f);

    result = dg::float2(0, 0);
    ASSERT_TRUE(math::QuadEquation(dg::float3(1.f, 2.f, -3.f), result));
    ASSERT_FLOAT_EQ(result.x, 1.f);
    ASSERT_FLOAT_EQ(result.y, -3.f);
}

}
