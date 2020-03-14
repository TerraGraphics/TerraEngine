#include <memory>
#include <random>
#include <chrono>

#include "gtest.h"
#include "core/math/equation.h"


template<typename T> static T Linear(T a, T b, T result) {
    return a * result + b;
}

#define EXPECT_LINEAR(a, b, result) \
    EXPECT_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<decltype(a)>, Linear(a, b, result), 0)

template<typename T> static T QuadAbsError() {
    if constexpr (std::is_same<T, double>::value) {
        return 1.e-15;
    }

    return 1.e-6f;
}

template<typename T> static T Quad(T a, T b, T c, T result) {
    double r = static_cast<double>(result);
    return static_cast<T>((static_cast<double>(a) * r + static_cast<double>(b)) * r + static_cast<double>(c));
}

#define EXPECT_QUAD(a, b, result) \
    EXPECT_NEAR(Quad(static_cast<decltype(a)>(1.), a, b, result), 0, QuadAbsError<decltype(a)>());

#define EXPECT_FULL_QUAD(a, b, c, result) \
    EXPECT_NEAR(Quad(a, b, c, result), 0, QuadAbsError<decltype(a)>());

template<typename T> static T CubicAbsError() {
    if constexpr (std::is_same<T, double>::value) {
        return 1.e-13;
    }

    return 1.e-5f;
}

template<typename T> static T Cubic(T a, T b, T c, T d, T result) {
    double r = static_cast<double>(result);
    return static_cast<T>(((static_cast<double>(a) * r + static_cast<double>(b)) * r + static_cast<double>(c)) * r + static_cast<double>(d));
}

#define EXPECT_CUBIC(a, b, c, result) \
    EXPECT_NEAR(Cubic(static_cast<decltype(a)>(1.), a, b, c, result), 0, CubicAbsError<decltype(a)>());

#define EXPECT_FULL_CUBIC(a, b, c, d, result) \
    EXPECT_NEAR(Cubic(a, b, c, d, result), 0, CubicAbsError<decltype(a)>());

}

#define EXPECT_CUBIC(a, b, c, result) \
    EXPECT_NEAR(Cubic(static_cast<decltype(a)>(1.), a, b, c, result), 0, CubicAbsError<decltype(a)>());

#define EXPECT_FULL_CUBIC(a, b, c, d, result) \
    EXPECT_NEAR(Cubic(a, b, c, d, result), 0, CubicAbsError<decltype(a)>());

TEST(MathEquation, SolveLinearDouble) {
    double a, b;
    double result[1];

namespace {

template <typename T> class MathEquation : public ::testing::Test {};
TYPED_TEST_SUITE(MathEquation, double, float);


TYPED_TEST(MathEquation, SolveLinear) {
    TypeParam a, b;
    TypeParam result[1];

    a = 1;
    b = 2;
    ASSERT_EQ(math::SolveLinear(a, b, result), 1);
    EXPECT_LINEAR(a, b, result[0]);

    a = 10;
    b = 1;
    ASSERT_EQ(math::SolveLinear(a, b, result), 1);
    EXPECT_LINEAR(a, b, result[0]);
}

TYPED_TEST(MathEquation, SolveLinearAIsZero) {
    TypeParam a, b;
    TypeParam result[1];

    a = 0;
    b = 2;
    ASSERT_EQ(math::SolveLinear(a, b, result), 0);
}

TYPED_TEST(MathEquation, SolveQuad) {
    TypeParam a, b;
    TypeParam result[2];

    a = 2;
    b = 3;
    ASSERT_EQ(math::SolveQuad(a, b, result), 0);

    a = 6;
    b = 9;
    ASSERT_EQ(math::SolveQuad(a, b, result), 2);
    EXPECT_QUAD(a, b, result[0]);
    EXPECT_QUAD(a, b, result[1]);

    a = 2;
    b = -3;
    ASSERT_EQ(math::SolveQuad(a, b, result), 2);
    EXPECT_QUAD(a, b, result[0]);
    EXPECT_QUAD(a, b, result[1]);
}

TYPED_TEST(MathEquation, SolveQuadAIsZero) {
    TypeParam a, b;
    TypeParam result[2];

    a = 0;
    b = 3;
    ASSERT_EQ(math::SolveQuad(a, b, result), 0);

    a = 0;
    b = -4;
    ASSERT_EQ(math::SolveQuad(a, b, result), 2);
    EXPECT_QUAD(a, b, result[0]);
    EXPECT_QUAD(a, b, result[1]);
}

TYPED_TEST(MathEquation, SolveQuadBIsZero) {
    TypeParam a, b;
    TypeParam result[2];

    a = 1;
    b = 0;
    ASSERT_EQ(math::SolveQuad(a, b, result), 2);
    EXPECT_QUAD(a, b, result[0]);
    EXPECT_QUAD(a, b, result[1]);
}

TYPED_TEST(MathEquation, SolveQuadAIsZeroBIsZero) {
    TypeParam a, b;
    TypeParam result[2];

    a = 0;
    b = 0;
    ASSERT_EQ(math::SolveQuad(a, b, result), 2);
    EXPECT_QUAD(a, b, result[0]);
    EXPECT_QUAD(a, b, result[1]);
}

TYPED_TEST(MathEquation, SolveFullQuad) {
    TypeParam a, b, c;
    TypeParam result[2];

    a = 2;
    b = 3;
    c = 4;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 0);

    a = 2;
    b = 12;
    c = 18;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2);
    EXPECT_FULL_QUAD(a, b, c, result[0]);
    EXPECT_FULL_QUAD(a, b, c, result[1]);

    a = 2;
    b = 4;
    c = -6;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2);
    EXPECT_FULL_QUAD(a, b, c, result[0]);
    EXPECT_FULL_QUAD(a, b, c, result[1]);
}

TYPED_TEST(MathEquation, SolveFullQuadAIsZero) {
    TypeParam a, b, c;
    TypeParam result[2];

    a = 0;
    b = 2;
    c = 3;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 1);
    EXPECT_FULL_QUAD(a, b, c, result[0]);
}

TYPED_TEST(MathEquation, SolveFullQuadBIsZero) {
    TypeParam a, b, c;
    TypeParam result[2];

    a = 2;
    b = 0;
    c = 3;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 0);

    a = 2;
    b = 0;
    c = -4;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2);
    EXPECT_FULL_QUAD(a, b, c, result[0]);
    EXPECT_FULL_QUAD(a, b, c, result[1]);
}

TYPED_TEST(MathEquation, SolveFullQuadCIsZero) {
    TypeParam a, b, c;
    TypeParam result[2];

    a = 2;
    b = 4;
    c = 0;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2);
    EXPECT_FULL_QUAD(a, b, c, result[0]);
    EXPECT_FULL_QUAD(a, b, c, result[1]);
}

TYPED_TEST(MathEquation, SolveFullQuadAIsZeroBIsZero) {
    TypeParam a, b, c;
    TypeParam result[2];

    a = 0;
    b = 0;
    c = 3;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 0);
}

TYPED_TEST(MathEquation, SolveFullQuadBIsZeroCIsZero) {
    TypeParam a, b, c;
    TypeParam result[2];

    a = 2;
    b = 0;
    c = 0;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2);
    EXPECT_FULL_QUAD(a, b, c, result[0]);
    EXPECT_FULL_QUAD(a, b, c, result[1]);
}

TYPED_TEST(MathEquation, SolveCubic) {
    TypeParam a, b, c;
    TypeParam result[3];

    a = 2;
    b = 3;
    c = 4;
    ASSERT_EQ(math::SolveCubic(a, b, c, result), 1);
    EXPECT_CUBIC(a, b, c, result[0]);

    a = 2;
    b = 3;
    c = -1;
    ASSERT_EQ(math::SolveCubic(a, b, c, result), 1);
    EXPECT_CUBIC(a, b, c, result[0]);

    a = TypeParam(-5.5);
    b = 6;
    c = TypeParam(4.5);
    ASSERT_EQ(math::SolveCubic(a, b, c, result), 3);
    EXPECT_CUBIC(a, b, c, result[0]);
    EXPECT_CUBIC(a, b, c, result[1]);
    EXPECT_CUBIC(a, b, c, result[2]);
}

TYPED_TEST(MathEquation, SolveCubicAIsZero) {
    TypeParam a, b, c;
    TypeParam result[3];

    a = 0;
    b = 1;
    c = 2;
    ASSERT_EQ(math::SolveCubic(a, b, c, result), 1);
    EXPECT_CUBIC(a, b, c, result[0]);
}

TYPED_TEST(MathEquation, SolveCubicBIsZero) {
    TypeParam a, b, c;
    TypeParam result[3];

    a = 1;
    b = 0;
    c = 2;
    ASSERT_EQ(math::SolveCubic(a, b, c, result), 1);
    EXPECT_CUBIC(a, b, c, result[0]);
}

TYPED_TEST(MathEquation, SolveCubicCIsZero) {
    TypeParam a, b, c;
    TypeParam result[3];

    a = static_cast<TypeParam>(4. / 3.);
    b = static_cast<TypeParam>(2. / 3.);
    c = 0;
    ASSERT_EQ(math::SolveCubic(a, b, c, result), 1);
    EXPECT_CUBIC(a, b, c, result[0]);
}

TYPED_TEST(MathEquation, SolveCubicAIsZeroBIsZero) {
    TypeParam a, b, c;
    TypeParam result[3];

    a = 0;
    b = 0;
    c = 1;
    ASSERT_EQ(math::SolveCubic(a, b, c, result), 1);
    EXPECT_CUBIC(a, b, c, result[0]);
}

TYPED_TEST(MathEquation, SolveCubicAIsZeroCIsZero) {
    TypeParam a, b, c;
    TypeParam result[3];

    a = 0;
    b = 1;
    c = 0;
    ASSERT_EQ(math::SolveCubic(a, b, c, result), 1);
    EXPECT_CUBIC(a, b, c, result[0]);
}

TYPED_TEST(MathEquation, SolveCubicBIsZeroCIsZero) {
    TypeParam a, b, c;
    TypeParam result[3];

    a = 1;
    b = 0;
    c = 0;
    ASSERT_EQ(math::SolveCubic(a, b, c, result), 1);
    EXPECT_CUBIC(a, b, c, result[0]);
}

TYPED_TEST(MathEquation, SolveCubicAIsZeroBIsZeroCIsZero) {
    TypeParam a, b, c;
    TypeParam result[3];

    a = 0;
    b = 0;
    c = 0;
    ASSERT_EQ(math::SolveCubic(a, b, c, result), 1);
    EXPECT_CUBIC(a, b, c, result[0]);
}

TYPED_TEST(MathEquation, SolveFullCubic) {
    TypeParam a, b, c, d;
    TypeParam result[3];

    a = 1;
    b = 2;
    c = 3;
    d = 4;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 1);
    EXPECT_FULL_CUBIC(a, b, c, d, result[0]);

    a = 1;
    b = 2;
    c = 3;
    d = -1;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 1);
    EXPECT_FULL_CUBIC(a, b, c, d, result[0]);

    a = 2;
    b = -11;
    c = 12;
    d = 9;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 3);
    EXPECT_FULL_CUBIC(a, b, c, d, result[0]);
    EXPECT_FULL_CUBIC(a, b, c, d, result[1]);
    EXPECT_FULL_CUBIC(a, b, c, d, result[2]);

    a = 5;
    b = -8;
    c = -8;
    d = 5;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 3);
    EXPECT_FULL_CUBIC(a, b, c, d, result[0]);
    EXPECT_FULL_CUBIC(a, b, c, d, result[1]);
    EXPECT_FULL_CUBIC(a, b, c, d, result[2]);
}

TYPED_TEST(MathEquation, SolveFullCubicAIsZero) {
    TypeParam a, b, c, d;
    TypeParam result[3];

    a = 0;
    b = 1;
    c = 6;
    d = 9;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 2);
    EXPECT_FULL_CUBIC(a, b, c, d, result[0]);
    EXPECT_FULL_CUBIC(a, b, c, d, result[1]);
}

TEST(MathEquation, SolveCubicFloat) {
    float a, b, c, d;
    float result[3];

    a = 1.f;
    b = 2.f;
    c = 3.f;
    d = 4.f;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 1); // - 1.6506
    EXPECT_FULL_CUBIC(a, b, c, d, result[0]);

    a = 1.f;
    b = 2.f;
    c = 3.f;
    d = -1.f;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 1); // - 0.27568
    EXPECT_FULL_CUBIC(a, b, c, d, result[0]);

    a = 5.f;
    b = -8.f;
    c = -8.f;
    d = 5.f;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 3);
    EXPECT_FULL_CUBIC(a, b, c, d, result[0]);
    EXPECT_FULL_CUBIC(a, b, c, d, result[1]);
    EXPECT_FULL_CUBIC(a, b, c, d, result[2]);
}

TEST(MathEquation, SolveCubicFloatAIsZero) {
    float a, b, c, d;
    float result[3];

    a = 0.f;
    b = 1.f;
    c = 6.f;
    d = 9.f;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 2);
    EXPECT_FULL_CUBIC(a, b, c, d, result[0]);
    EXPECT_FULL_CUBIC(a, b, c, d, result[1]);
}

}

}
