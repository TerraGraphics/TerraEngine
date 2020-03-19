#include <memory>
#include <random>
#include <chrono>

#include "gtest.h"
#include "test_helpers.h"
#include "core/math/types.h"
#include "core/math/intersection.h"


template<typename T> static T Vector3AbsError() {
    if constexpr (std::is_same<T, double>::value) {
        return 1.e-14;
    }

    return 1.e-5f;
}

#define ASSERT_VECTOR3_EQ(actual, expected) \
    EXPECT_NEAR(actual.x, expected.x, Vector3AbsError<decltype(actual.x)>()); \
    EXPECT_NEAR(actual.y, expected.y, Vector3AbsError<decltype(actual.y)>()); \
    EXPECT_NEAR(actual.z, expected.z, Vector3AbsError<decltype(actual.z)>())


template<typename T> static T PointOnPlaneAbsError() {
    if constexpr (std::is_same<T, double>::value) {
        return 1.e-14;
    }

    return 1.e-5f;
}

#define ASSERT_POINT_ON_PLANE(plane, point) \
    EXPECT_NEAR(dg::dot(static_cast<dg::Vector4<TypeParam>>(plane), dg::Vector4<TypeParam>(point, 1)), 0, PointOnPlaneAbsError<decltype(plane.a)>()); \


namespace {

template <typename T> class MathIntersection : public ::testing::Test {};
TYPED_TEST_SUITE(MathIntersection, double, float);

TYPED_TEST(MathIntersection, RayAndCylinderOX) {
    math::CylinderT<TypeParam> cylinder(1, 1, math::Axis::X);
    auto rayStart = dg::Vector3<TypeParam>(0, -10, 0);
    math::RayT<TypeParam> ray(rayStart, dg::Vector3<TypeParam>(0, 0, 0));

    ray.direction = dg::Vector3<TypeParam>(0, 1, 0);
    ASSERT_TRUE(math::IsIntersection(ray, cylinder));
    ray.direction = dg::Vector3<TypeParam>(0, -1, 0);
    ASSERT_FALSE(math::IsIntersection(ray, cylinder));
    ray.direction = dg::Vector3<TypeParam>(1, 0, 0);
    ASSERT_FALSE(math::IsIntersection(ray, cylinder));
    ray.direction = dg::Vector3<TypeParam>(-1, 0, 0);
    ASSERT_FALSE(math::IsIntersection(ray, cylinder));
    ray.direction = dg::Vector3<TypeParam>(0, 0, 1);
    ASSERT_FALSE(math::IsIntersection(ray, cylinder));
    ray.direction = dg::Vector3<TypeParam>(0, 0, -1);
    ASSERT_FALSE(math::IsIntersection(ray, cylinder));
}

TYPED_TEST(MathIntersection, RayAndCylinderOY) {
    math::CylinderT<TypeParam> cylinder(1, 1, math::Axis::Y);
    auto rayStart = dg::Vector3<TypeParam>(0, 0, -10);
    math::RayT<TypeParam> ray(rayStart, dg::Vector3<TypeParam>(0, 0, 0));

    ray.direction = dg::Vector3<TypeParam>(0, 0, 1);
    ASSERT_TRUE(math::IsIntersection(ray, cylinder));
    ray.direction = dg::Vector3<TypeParam>(0, 0, -1);
    ASSERT_FALSE(math::IsIntersection(ray, cylinder));
    ray.direction = dg::Vector3<TypeParam>(0, 1, 0);
    ASSERT_FALSE(math::IsIntersection(ray, cylinder));
    ray.direction = dg::Vector3<TypeParam>(0, -1, 0);
    ASSERT_FALSE(math::IsIntersection(ray, cylinder));
    ray.direction = dg::Vector3<TypeParam>(1, 0, 0);
    ASSERT_FALSE(math::IsIntersection(ray, cylinder));
    ray.direction = dg::Vector3<TypeParam>(-1, 0, 0);
    ASSERT_FALSE(math::IsIntersection(ray, cylinder));
}

TYPED_TEST(MathIntersection, RayAndCylinderOZ) {
    math::CylinderT<TypeParam> cylinder(1, 1, math::Axis::Z);
    auto rayStart = dg::Vector3<TypeParam>(-10, 0, 0);
    math::RayT<TypeParam> ray(rayStart, dg::Vector3<TypeParam>(0, 0, 0));

    ray.direction = dg::Vector3<TypeParam>(1, 0, 0);
    ASSERT_TRUE(math::IsIntersection(ray, cylinder));
    ray.direction = dg::Vector3<TypeParam>(-1, 0, 0);
    ASSERT_FALSE(math::IsIntersection(ray, cylinder));
    ray.direction = dg::Vector3<TypeParam>(0, 1, 0);
    ASSERT_FALSE(math::IsIntersection(ray, cylinder));
    ray.direction = dg::Vector3<TypeParam>(0, -1, 0);
    ASSERT_FALSE(math::IsIntersection(ray, cylinder));
    ray.direction = dg::Vector3<TypeParam>(0, 0, 1);
    ASSERT_FALSE(math::IsIntersection(ray, cylinder));
    ray.direction = dg::Vector3<TypeParam>(0, 0, -1);
    ASSERT_FALSE(math::IsIntersection(ray, cylinder));
}

TYPED_TEST(MathIntersection, RayAndPlane) {
    auto rg = GetRandomGenerator();
    auto plane = GetRandomPlane<TypeParam>(rg);
    auto gen = std::uniform_real_distribution<TypeParam>(static_cast<TypeParam>(0.01), 1);

    auto rayStart = dg::Vector3<TypeParam>(gen(rg), gen(rg), gen(rg));
    auto rayDir = dg::normalize(dg::Vector3<TypeParam>(gen(rg), gen(rg), gen(rg)));
    auto ray = math::RayT(rayStart, rayDir);

    auto result = dg::Vector3<TypeParam>(0, 0, 0);
    if (math::IsIntersection(ray, plane, result)) {
        auto dir = dg::normalize(result - rayStart);
        ASSERT_VECTOR3_EQ(dir, rayDir);
        ASSERT_POINT_ON_PLANE(plane, result);
    }
}

}
