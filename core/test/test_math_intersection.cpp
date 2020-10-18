#include <memory>
#include <random>
#include <cstdint>

#include "test/test.h"
#include "test_helpers.h"
#include "core/math/types.h"
#include "core/math/intersection.h"


// Ray and Cylinder
#define ASSERT_RAY_AND_CYLINDER_INTERSECTION_TRUE(start, dir, cylinder) \
    ASSERT_TRUE(math::IsIntersection(math::RayT<TypeParam>(start, dg::Vector3<TypeParam>dir), cylinder))

#define ASSERT_RAY_AND_CYLINDER_INTERSECTION_FALSE(start, dir, cylinder) \
    ASSERT_FALSE(math::IsIntersection(math::RayT<TypeParam>(start, dg::Vector3<TypeParam>dir), cylinder))

// Ray and Torus
template<typename T> static T TorusAbsError() {
    if constexpr (std::is_same_v<T, double>) {
        return 1.e-10;
    }

    return 1.e-5f;
}

// Ray and Torus OX
template<typename T> static T PointOnTorusOX(const math::TorusT<T>& torus, const dg::Vector3<T>& point) {
    auto tmp = point.x*point.x + point.y*point.y + point.z*point.z +
        torus.majorRadius*torus.majorRadius - torus.minorRadius*torus.minorRadius;
    return tmp * tmp - 4*torus.majorRadius*torus.majorRadius*(point.z*point.z + point.y*point.y);
}

#define ASSERT_POINT_ON_TORUS_OX(torus, point) \
    EXPECT_NEAR(PointOnTorusOX(torus, point), 0, TorusAbsError<decltype(point.x)>())

#define ASSERT_RAY_AND_TORUS_OX_INTERSECTION(start_ray, dir_ray, torus, expectedNumber) { \
    math::RayT<TypeParam> ray(dg::Vector3<TypeParam>start_ray, dir_ray); \
    dg::Vector3<decltype(ray.start.x)> results[4]; \
    auto actualIntersectionNumber = static_cast<uint32_t>(math::Intersection(ray, torus, results)); \
    ASSERT_EQ(actualIntersectionNumber, static_cast<uint32_t>(expectedNumber)) << "incorrect count intersection"; \
    ASSERT_EQ(math::IsIntersection(ray, torus), expectedNumber != 0); \
    for (uint8_t i=0; i!=actualIntersectionNumber; ++i) { \
        ASSERT_POINT_ON_TORUS_OX(torus, results[i]); \
    }}

// Ray and Torus OY
template<typename T> static T PointOnTorusOY(const math::TorusT<T>& torus, const dg::Vector3<T>& point) {
    auto tmp = point.x*point.x + point.y*point.y + point.z*point.z +
        torus.majorRadius*torus.majorRadius - torus.minorRadius*torus.minorRadius;
    return tmp * tmp - 4*torus.majorRadius*torus.majorRadius*(point.x*point.x + point.z*point.z);
}

#define ASSERT_POINT_ON_TORUS_OY(torus, point) \
    EXPECT_NEAR(PointOnTorusOY(torus, point), 0, TorusAbsError<decltype(point.x)>())

#define ASSERT_RAY_AND_TORUS_OY_INTERSECTION(start_ray, dir_ray, torus, expectedNumber) { \
    math::RayT<TypeParam> ray(dg::Vector3<TypeParam>start_ray, dir_ray); \
    dg::Vector3<decltype(ray.start.x)> results[4]; \
    auto actualIntersectionNumber = static_cast<uint32_t>(math::Intersection(ray, torus, results)); \
    ASSERT_EQ(actualIntersectionNumber, static_cast<uint32_t>(expectedNumber)) << "incorrect count intersection"; \
    ASSERT_EQ(math::IsIntersection(ray, torus), expectedNumber != 0); \
    for (uint8_t i=0; i!=actualIntersectionNumber; ++i) { \
        ASSERT_POINT_ON_TORUS_OY(torus, results[i]); \
    }}

// Ray and Torus OZ
template<typename T> static T PointOnTorusOZ(const math::TorusT<T>& torus, const dg::Vector3<T>& point) {
    auto tmp = point.x*point.x + point.y*point.y + point.z*point.z +
        torus.majorRadius*torus.majorRadius - torus.minorRadius*torus.minorRadius;
    return tmp * tmp - 4*torus.majorRadius*torus.majorRadius*(point.x*point.x + point.y*point.y);
}

#define ASSERT_POINT_ON_TORUS_OZ(torus, point) \
    EXPECT_NEAR(PointOnTorusOZ(torus, point), 0, TorusAbsError<decltype(point.x)>())

#define ASSERT_RAY_AND_TORUS_OZ_INTERSECTION(start_ray, dir_ray, torus, expectedNumber) { \
    math::RayT<TypeParam> ray(dg::Vector3<TypeParam>start_ray, dir_ray); \
    dg::Vector3<decltype(ray.start.x)> results[4]; \
    auto actualIntersectionNumber = static_cast<uint32_t>(math::Intersection(ray, torus, results)); \
    ASSERT_EQ(actualIntersectionNumber, static_cast<uint32_t>(expectedNumber)) << "incorrect count intersection"; \
    ASSERT_EQ(math::IsIntersection(ray, torus), expectedNumber != 0); \
    for (uint8_t i=0; i!=actualIntersectionNumber; ++i) { \
        ASSERT_POINT_ON_TORUS_OZ(torus, results[i]); \
    }}

// Ray And Plane
template<typename T> static T Vector3AbsError() {
    if constexpr (std::is_same_v<T, double>) {
        return 1.e-14;
    }

    return 1.e-5f;
}

#define ASSERT_VECTOR3_EQ(actual, expected) \
    EXPECT_NEAR(actual.x, expected.x, Vector3AbsError<decltype(actual.x)>()); \
    EXPECT_NEAR(actual.y, expected.y, Vector3AbsError<decltype(actual.y)>()); \
    EXPECT_NEAR(actual.z, expected.z, Vector3AbsError<decltype(actual.z)>())


template<typename T> static T PointOnPlaneAbsError() {
    if constexpr (std::is_same_v<T, double>) {
        return 1.e-14;
    }

    return 1.e-5f;
}

#define ASSERT_POINT_ON_PLANE(plane, point) \
    EXPECT_NEAR(dg::dot(static_cast<dg::Vector4<TypeParam>>(plane), dg::Vector4<TypeParam>(point, 1)), 0, PointOnPlaneAbsError<decltype(plane.a)>()); \


namespace {

template <typename T> class MathIntersection : public ::testing::Test {};
TYPED_SUITE(MathIntersection, double, float);

TYPED_TEST(MathIntersection, RayAndCylinderOX) {
    math::CylinderT<TypeParam> cylinder(1, 1, math::Axis::X);
    auto start = dg::Vector3<TypeParam>(0, -10, 0);

    ASSERT_RAY_AND_CYLINDER_INTERSECTION_TRUE( start, ( 0,  1,  0), cylinder);
    ASSERT_RAY_AND_CYLINDER_INTERSECTION_FALSE(start, ( 0, -1,  0), cylinder);
    ASSERT_RAY_AND_CYLINDER_INTERSECTION_FALSE(start, ( 1,  0,  0), cylinder);
    ASSERT_RAY_AND_CYLINDER_INTERSECTION_FALSE(start, (-1,  0,  0), cylinder);
    ASSERT_RAY_AND_CYLINDER_INTERSECTION_FALSE(start, ( 0,  0,  1), cylinder);
    ASSERT_RAY_AND_CYLINDER_INTERSECTION_FALSE(start, ( 0,  0, -1), cylinder);
}

TYPED_TEST(MathIntersection, RayAndCylinderOY) {
    math::CylinderT<TypeParam> cylinder(1, 1, math::Axis::Y);
    auto start = dg::Vector3<TypeParam>(0, 0, -10);

    ASSERT_RAY_AND_CYLINDER_INTERSECTION_TRUE( start, ( 0,  0,  1), cylinder);
    ASSERT_RAY_AND_CYLINDER_INTERSECTION_FALSE(start, ( 0,  0, -1), cylinder);
    ASSERT_RAY_AND_CYLINDER_INTERSECTION_FALSE(start, ( 0,  1,  0), cylinder);
    ASSERT_RAY_AND_CYLINDER_INTERSECTION_FALSE(start, ( 0, -1,  0), cylinder);
    ASSERT_RAY_AND_CYLINDER_INTERSECTION_FALSE(start, ( 1,  0,  0), cylinder);
    ASSERT_RAY_AND_CYLINDER_INTERSECTION_FALSE(start, (-1,  0,  0), cylinder);
}

TYPED_TEST(MathIntersection, RayAndCylinderOZ) {
    math::CylinderT<TypeParam> cylinder(1, 1, math::Axis::Z);
    auto start = dg::Vector3<TypeParam>(-10, 0, 0);

    ASSERT_RAY_AND_CYLINDER_INTERSECTION_TRUE( start, ( 1,  0,  0), cylinder);
    ASSERT_RAY_AND_CYLINDER_INTERSECTION_FALSE(start, (-1,  0,  0), cylinder);
    ASSERT_RAY_AND_CYLINDER_INTERSECTION_FALSE(start, ( 0,  1,  0), cylinder);
    ASSERT_RAY_AND_CYLINDER_INTERSECTION_FALSE(start, ( 0, -1,  0), cylinder);
    ASSERT_RAY_AND_CYLINDER_INTERSECTION_FALSE(start, ( 0,  0,  1), cylinder);
    ASSERT_RAY_AND_CYLINDER_INTERSECTION_FALSE(start, ( 0,  0, -1), cylinder);
}

TYPED_TEST(MathIntersection, RayAndTorusOX) {
    TypeParam r = 1;
    TypeParam R = 10;
    auto rdt = r + static_cast<TypeParam>(.1);
    TypeParam Rdt = R + r + 1;
    math::TorusT<TypeParam> torus(r, R, math::Axis::X);

    const auto dirZ = dg::Vector3<TypeParam>(0, 0, 1);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((  0, 0, -Rdt), dirZ, torus, 4);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((rdt, 0, -Rdt), dirZ, torus, 0);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((  0, 0,   -R), dirZ, torus, 3);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((  0, 0,    0), dirZ, torus, 2);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((  0, 0,    R), dirZ, torus, 1);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((  0, 0,  Rdt), dirZ, torus, 0);

    const auto dirY = dg::Vector3<TypeParam>(0, 1, 0);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((  0, -Rdt, 0), dirY, torus, 4);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((rdt, -Rdt, 0), dirY, torus, 0);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((  0,   -R, 0), dirY, torus, 3);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((  0,    0, 0), dirY, torus, 2);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((  0,    R, 0), dirY, torus, 1);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((  0,  Rdt, 0), dirY, torus, 0);

    const auto dirX = dg::Vector3<TypeParam>(1, 0, 0);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((-rdt, 0, -Rdt), dirX, torus, 0);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((-rdt, 0,   -R), dirX, torus, 2);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((-rdt, 0,    0), dirX, torus, 0);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((-rdt, 0,    R), dirX, torus, 2);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((-rdt, 0,  Rdt), dirX, torus, 0);

    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((0, 0, -Rdt), dirX, torus, 0);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((0, 0,   -R), dirX, torus, 1);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((0, 0,    0), dirX, torus, 0);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((0, 0,    R), dirX, torus, 1);
    ASSERT_RAY_AND_TORUS_OX_INTERSECTION((0, 0,  Rdt), dirX, torus, 0);
}

TYPED_TEST(MathIntersection, RayAndTorusOY) {
    TypeParam r = 1;
    TypeParam R = 10;
    auto rdt = r + static_cast<TypeParam>(.1);
    TypeParam Rdt = R + r + 1;
    math::TorusT<TypeParam> torus(r, R, math::Axis::Y);

    const auto dirX = dg::Vector3<TypeParam>(1, 0, 0);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((-Rdt,   0, 0), dirX, torus, 4);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((-Rdt, rdt, 0), dirX, torus, 0);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((  -R,   0, 0), dirX, torus, 3);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((   0,   0, 0), dirX, torus, 2);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((   R,   0, 0), dirX, torus, 1);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION(( Rdt,   0, 0), dirX, torus, 0);

    const auto dirZ = dg::Vector3<TypeParam>(0, 0, 1);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((0,   0, -Rdt), dirZ, torus, 4);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((0, rdt, -Rdt), dirZ, torus, 0);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((0,   0,   -R), dirZ, torus, 3);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((0,   0,    0), dirZ, torus, 2);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((0,   0,    R), dirZ, torus, 1);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((0,   0,  Rdt), dirZ, torus, 0);

    const auto dirY = dg::Vector3<TypeParam>(0, 1, 0);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((-Rdt, -rdt, 0), dirY, torus, 0);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((  -R, -rdt, 0), dirY, torus, 2);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((   0, -rdt, 0), dirY, torus, 0);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((   R, -rdt, 0), dirY, torus, 2);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION(( Rdt, -rdt, 0), dirY, torus, 0);

    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((-Rdt, 0, 0), dirY, torus, 0);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((  -R, 0, 0), dirY, torus, 1);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((   0, 0, 0), dirY, torus, 0);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION((   R, 0, 0), dirY, torus, 1);
    ASSERT_RAY_AND_TORUS_OY_INTERSECTION(( Rdt, 0, 0), dirY, torus, 0);
}

TYPED_TEST(MathIntersection, RayAndTorusOZ) {
    TypeParam r = 1;
    TypeParam R = 10;
    auto rdt = r + static_cast<TypeParam>(.1);
    TypeParam Rdt = R + r + 1;
    math::TorusT<TypeParam> torus(r, R, math::Axis::Z);

    const auto dirX = dg::Vector3<TypeParam>(1, 0, 0);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((-Rdt, 0,   0), dirX, torus, 4);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((-Rdt, 0, rdt), dirX, torus, 0);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((  -R, 0,   0), dirX, torus, 3);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((   0, 0,   0), dirX, torus, 2);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((   R, 0,   0), dirX, torus, 1);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION(( Rdt, 0,   0), dirX, torus, 0);

    const auto dirY = dg::Vector3<TypeParam>(0, 1, 0);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((0, -Rdt,   0), dirY, torus, 4);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((0, -Rdt, rdt), dirY, torus, 0);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((0,   -R,   0), dirY, torus, 3);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((0,    0,   0), dirY, torus, 2);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((0,    R,   0), dirY, torus, 1);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((0,  Rdt,   0), dirY, torus, 0);

    const auto dirZ = dg::Vector3<TypeParam>(0, 0, 1);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((-Rdt, 0, -rdt), dirZ, torus, 0);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((  -R, 0, -rdt), dirZ, torus, 2);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((   0, 0, -rdt), dirZ, torus, 0);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((   R, 0, -rdt), dirZ, torus, 2);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION(( Rdt, 0, -rdt), dirZ, torus, 0);

    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((-Rdt, 0, 0), dirZ, torus, 0);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((  -R, 0, 0), dirZ, torus, 1);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((   0, 0, 0), dirZ, torus, 0);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION((   R, 0, 0), dirZ, torus, 1);
    ASSERT_RAY_AND_TORUS_OZ_INTERSECTION(( Rdt, 0, 0), dirZ, torus, 0);
}

TYPED_TEST(MathIntersection, RayAndPlane) {
    auto rg = GetRandomGenerator();
    auto plane = GetRandomPlane<TypeParam>(rg);
    auto gen = std::uniform_real_distribution<TypeParam>(static_cast<TypeParam>(0.01), 1);

    auto rayStart = dg::Vector3<TypeParam>(gen(rg), gen(rg), gen(rg));
    auto rayDir = dg::normalize(dg::Vector3<TypeParam>(gen(rg), gen(rg), gen(rg)));
    auto ray = math::RayT(rayStart, rayDir);

    auto result = dg::Vector3<TypeParam>(0, 0, 0);
    if (math::Intersection(ray, plane, result) != 0) {
        auto dir = dg::normalize(result - rayStart);
        ASSERT_VECTOR3_EQ(dir, rayDir);
        ASSERT_POINT_ON_PLANE(plane, result);
    }
}

}
