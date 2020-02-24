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

TEST(MathAdvanced, IntersectionRayAndCylinderOX) {
    float radius = 1.f;
    float height = 1.f;
    auto axis = math::Axis::X;
    auto rayStart = dg::float3(0, -10.f, 0);
    ASSERT_TRUE(math::IntersectionRayAndCylinder(rayStart, dg::float3(0, 1.f, 0), axis, radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder(rayStart, dg::float3(0, -1.f, 0), axis, radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder(rayStart, dg::float3(1.f, 0, 0), axis, radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder(rayStart, dg::float3(-1.f, 0, 0), axis, radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder(rayStart, dg::float3(0, 0, 1.f), axis, radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder(rayStart, dg::float3(0, 0, -1.f), axis, radius, height));
}

TEST(MathAdvanced, IntersectionRayAndCylinderOY) {
    float radius = 1.f;
    float height = 1.f;
    auto axis = math::Axis::Y;
    auto rayStart = dg::float3(0, 0, -10.f);
    ASSERT_TRUE(math::IntersectionRayAndCylinder(rayStart, dg::float3(0, 0, 1.f), axis, radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder(rayStart, dg::float3(0, 0, -1.f), axis, radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder(rayStart, dg::float3(0, 1.f, 0), axis, radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder(rayStart, dg::float3(0, -1.f, 0), axis, radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder(rayStart, dg::float3(1.f, 0, 0), axis, radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder(rayStart, dg::float3(-1.f, 0, 0), axis, radius, height));
}

TEST(MathAdvanced, IntersectionRayAndCylinderOZ) {
    float radius = 1.f;
    float height = 1.f;
    auto axis = math::Axis::Z;
    auto rayStart = dg::float3(-10.f, 0, 0);
    ASSERT_TRUE(math::IntersectionRayAndCylinder(rayStart, dg::float3(1.f, 0, 0), axis, radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder(rayStart, dg::float3(-1.f, 0, 0), axis, radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder(rayStart, dg::float3(0, 1.f, 0), axis, radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder(rayStart, dg::float3(0, -1.f, 0), axis, radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder(rayStart, dg::float3(0, 0, 1.f), axis, radius, height));
    ASSERT_FALSE(math::IntersectionRayAndCylinder(rayStart, dg::float3(0, 0, -1.f), axis, radius, height));
}

}
