#include <memory>
#include <random>

#include "test/test.h"
#include "test_helpers.h"
#include "core/math/types.h"


template<typename T> static T PlaneAbsError() {
    if constexpr (std::is_same_v<T, double>) {
        return 1.e-13;
    }

    return 1.e-4f;
}

#define EXPECT_PLANE(actual, expected) \
    EXPECT_NEAR(1, expected.a, PlaneAbsError<decltype(actual.a)>()); \
    EXPECT_NEAR(actual.b / actual.a, expected.b, PlaneAbsError<decltype(actual.a)>()); \
    EXPECT_NEAR(actual.c / actual.a, expected.c, PlaneAbsError<decltype(actual.a)>()); \
    EXPECT_NEAR(actual.d / actual.a, expected.d, PlaneAbsError<decltype(actual.a)>());

namespace {

template <typename T> class MathTypes : public ::testing::Test {
public:
    void SetUp() override {
        m_generator = GetRandomGenerator();
    }

    math::PlaneT<T> GetRandomPlane() {
        return ::GetRandomPlane<T>(m_generator);
    }

    dg::Vector3<T> GetRandomPointInPlane(math::PlaneT<T> plane) {
        auto gen = std::uniform_real_distribution<T>(0.01f, 1.f);
        T x = gen(m_generator);
        T y = gen(m_generator);
        return dg::Vector3<T>(x, y, - (plane.a * x + plane.b * y + plane.d) / plane.c);
    }

    std::mt19937_64 m_generator;
};

TYPED_TEST_SUITE(MathTypes, double, float);


TYPED_TEST(MathTypes, PlaneByPointAndNormal) {
    auto planeExpected = this->GetRandomPlane();
    auto point0 = this->GetRandomPointInPlane(planeExpected);
    auto normal = static_cast<dg::Vector3<TypeParam>>(planeExpected);

    auto planeActual = math::PlaneT<TypeParam>(point0, normal);

    EXPECT_PLANE(planeActual, planeExpected);
}

TYPED_TEST(MathTypes, PlaneByPointAndTwoVec) {
    auto planeExpected = this->GetRandomPlane();
    auto point0 = this->GetRandomPointInPlane(planeExpected);
    auto point1 = this->GetRandomPointInPlane(planeExpected);
    auto point2 = this->GetRandomPointInPlane(planeExpected);

    auto planeActual = math::PlaneT<TypeParam>(point0, point1 - point0, point2 - point0);

    EXPECT_PLANE(planeActual, planeExpected);
}

}
