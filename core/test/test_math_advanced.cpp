#include <memory>
#include <random>
#include <chrono>

#include "gtest.h"
#include "test_helpers.h"
#include "core/math/types.h"
#include "core/math/advanced.h"

#define ASSERT_FLOAT3_EQ(actual, expected) \
    ASSERT_FLOAT_EQ(actual.x, expected.x); \
    ASSERT_FLOAT_EQ(actual.y, expected.y); \
    ASSERT_FLOAT_EQ(actual.z, expected.z);


namespace {

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

TEST(MathAdvanced, IntersectionRayAndPlane) {
    auto rg = GetRandomGenerator();
    auto plane = GetRandomPlane<float>(rg);
    auto gen = std::uniform_real_distribution<float>(0.01f, 1.f);

    dg::float3 rayStart = dg::float3(gen(rg), gen(rg), gen(rg));
    dg::float3 rayDir = dg::normalize(dg::float3(gen(rg), gen(rg), gen(rg)));

    auto result = dg::float3(0, 0, 0);
    if (math::IntersectionRayAndPlane(rayStart, rayDir, plane, result)) {
        auto dir = dg::normalize(result - rayStart);
        ASSERT_FLOAT3_EQ(dir, rayDir);
        ASSERT_FLOAT_EQ(dg::dot(static_cast<dg::float4>(plane), dg::float4(result, 1.f)), 0);
    }
}

}
