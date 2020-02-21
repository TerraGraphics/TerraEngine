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

TEST(MathAdvanced, IntersectionRayAndCylinder0Z) {
    // rayStart, rayDir, radius, height
    float radius = 1.f;
    float height = 1.f;
    ASSERT_TRUE(math::IntersectionRayAndCylinder0Z(dg::float3(-10.f, 0, 0), dg::float3(1.f, 0, 0), radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder0Z(dg::float3(-10.f, 0, 0), dg::float3(-1.f, 0, 0), radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder0Z(dg::float3(-10.f, 0, 0), dg::float3(0, 1.f, 0), radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder0Z(dg::float3(-10.f, 0, 0), dg::float3(0, 0, 1.f), radius, height));
}

}
