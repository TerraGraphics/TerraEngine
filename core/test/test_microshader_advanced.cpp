#include <memory>
#include <random>
#include <chrono>
#include <gtest/gtest.h>

#include "core/math/basic.h"
#include "core/math/advanced.h"

#define ASSERT_FLOAT4_EQ(actual, expected) \
    ASSERT_FLOAT_EQ(actual.x, expected.x); \
    ASSERT_FLOAT_EQ(actual.y, expected.y); \
    ASSERT_FLOAT_EQ(actual.z, expected.z); \
    ASSERT_FLOAT_EQ(actual.w, expected.w);

#define EXPECT_FLOAT4_NEAR(actual, expected, abs_error) \
    EXPECT_NEAR(actual.x, expected.x, abs_error); \
    EXPECT_NEAR(actual.y, expected.y, abs_error); \
    EXPECT_NEAR(actual.z, expected.z, abs_error); \
    EXPECT_NEAR(actual.w, expected.w, abs_error);

namespace {

TEST(MathAdvanced, QuadEquation) {
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

TEST(MathAdvanced, QuadEquationXIsZero) {
    dg::float2 result;

    result = dg::float2(0, 0);
    ASSERT_TRUE(math::QuadEquation(dg::float3(0.f, 2.f, 4.f), result));
    ASSERT_FLOAT_EQ(result.x, -2.f);
    ASSERT_FLOAT_EQ(result.y, -2.f);
}

TEST(MathAdvanced, QuadEquationXAndYIsZero) {
    dg::float2 result;

    result = dg::float2(0, 0);
    ASSERT_FALSE(math::QuadEquation(dg::float3(0.f, 0.f, 4.f), result));
}

static dg::float3 GetPointInPlane(dg::float4 plane, float x, float y) {
    return dg::float3(x, y, - (plane.x * x + plane.y * y + plane.w) / plane.z);
}

static std::mt19937_64 GetRandomGenerator() {
    auto seed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    return std::mt19937_64(static_cast<std::mt19937_64::result_type>(seed));
}

TEST(MathAdvanced, GetPlaneByPointAndNormal) {
    auto rg = GetRandomGenerator();
    auto gen = std::uniform_real_distribution<float>(0.01f, 1.f);

    dg::float4 plane(1.f, gen(rg), gen(rg), gen(rg));
    dg::float3 point0 = GetPointInPlane(plane, gen(rg), gen(rg));
    dg::float3 n = dg::float3(plane.x, plane.y, plane.z);

    auto planeActual = math::GetPlaneByPointAndNormal(point0, n);
    planeActual /= planeActual.x;

    EXPECT_FLOAT4_NEAR(planeActual, plane, 0.001f);
}

TEST(MathAdvanced, GetPlaneByPointAndTwoVec) {
    auto rg = GetRandomGenerator();
    auto gen = std::uniform_real_distribution<float>(0.01f, 1.f);

    dg::float4 plane(1.f, gen(rg), gen(rg), gen(rg));
    dg::float3 point0 = GetPointInPlane(plane, gen(rg), gen(rg));
    dg::float3 point1 = GetPointInPlane(plane, gen(rg), gen(rg));
    dg::float3 point2 = GetPointInPlane(plane, gen(rg), gen(rg));

    auto planeActual = math::GetPlaneByPointAndTwoVec(point0, point1 - point0, point2 - point0);
    planeActual /= planeActual.x;

    EXPECT_FLOAT4_NEAR(planeActual, plane, 0.001f);
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
