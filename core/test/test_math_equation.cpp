#include <memory>
#include <random>
#include <chrono>
#include <gtest/gtest.h>

#include "core/math/equation.h"

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

#define EXPECT_QUAD(a, b, result) \
    EXPECT_NEAR(Quad(static_cast<decltype(a)>(1.), a, b, result), 0, QuadAbsError<decltype(a)>());

#define EXPECT_FULL_QUAD(a, b, c, result) \
    EXPECT_NEAR(Quad(a, b, c, result), 0, QuadAbsError<decltype(a)>());

#define EXPECT_FULL_CUBIC(a, b, c, d, result) \
    EXPECT_NEAR(Cubic(a, b, c, d, result), 0, CubicAbsError<decltype(a)>());
namespace {

TEST(MathEquation, SolveLinearDouble) {
    double a, b;
    double result[1];

    a = 1.;
    b = 2.;
    ASSERT_EQ(math::SolveLinear(a, b, result), 1);
    ASSERT_DOUBLE_EQ(a * result[0] + b, 0);

    a = 10.;
    b = 1.;
    ASSERT_EQ(math::SolveLinear(a, b, result), 1);
    ASSERT_DOUBLE_EQ(a * result[0] + b, 0);
}

TEST(MathEquation, SolveLinearDoubleAIsZero) {
    double a, b;
    double result[1];

    a = 0;
    b = 2.;
    ASSERT_EQ(math::SolveLinear(a, b, result), 0);
}

TEST(MathEquation, SolveLinearFloat) {
    float a, b;
    float result[1];

    a = 1.f;
    b = 2.f;
    ASSERT_EQ(math::SolveLinear(a, b, result), 1);
    ASSERT_FLOAT_EQ(a * result[0] + b, 0);

    a = 10.f;
    b = 1.f;
    ASSERT_EQ(math::SolveLinear(a, b, result), 1);
    ASSERT_FLOAT_EQ(a * result[0] + b, 0);
}

TEST(MathEquation, SolveLinearFloatAIsZero) {
    float a, b;
    float result[1];

    a = 0;
    b = 2.;
    ASSERT_EQ(math::SolveLinear(a, b, result), 0);
}

TEST(MathEquation, SolveQuadDouble) {
    double a, b;
    double result[2];

    a = 2.;
    b = 3.;
    ASSERT_EQ(math::SolveQuad(a, b, result), 0);

    a = 6.;
    b = 9.;
    ASSERT_EQ(math::SolveQuad(a, b, result), 2); // -3, -3
    EXPECT_QUAD(a, b, result[0]);
    EXPECT_QUAD(a, b, result[1]);

    a = 2.;
    b = -3.;
    ASSERT_EQ(math::SolveQuad(a, b, result), 2); // 1, -3
    EXPECT_QUAD(a, b, result[0]);
    EXPECT_QUAD(a, b, result[1]);
}

TEST(MathEquation, SolveQuadDoubleAIsZero) {
    double a, b;
    double result[2];

    a = 0;
    b = 3.;
    ASSERT_EQ(math::SolveQuad(a, b, result), 0);

    a = 0;
    b = -4.;
    ASSERT_EQ(math::SolveQuad(a, b, result), 2);
    EXPECT_QUAD(a, b, result[0]);
    EXPECT_QUAD(a, b, result[1]);
}

TEST(MathEquation, SolveQuadDoubleBIsZero) {
    double a, b;
    double result[2];

    a = 1.;
    b = 0;
    ASSERT_EQ(math::SolveQuad(a, b, result), 2);
    EXPECT_QUAD(a, b, result[0]);
    EXPECT_QUAD(a, b, result[1]);
}

TEST(MathEquation, SolveQuadDoubleAIsZeroBIsZero) {
    double a, b;
    double result[2];

    a = 0;
    b = 0;
    ASSERT_EQ(math::SolveQuad(a, b, result), 2);
    EXPECT_QUAD(a, b, result[0]);
    EXPECT_QUAD(a, b, result[1]);
}

TEST(MathEquation, SolveQuadFloat) {
    float a, b;
    float result[2];

    a = 2.f;
    b = 3.f;
    ASSERT_EQ(math::SolveQuad(a, b, result), 0);

    a = 6.f;
    b = 9.f;
    ASSERT_EQ(math::SolveQuad(a, b, result), 2); // -3, -3
    EXPECT_QUAD(a, b, result[0]);
    EXPECT_QUAD(a, b, result[1]);

    a = 2.f;
    b = -3.f;
    ASSERT_EQ(math::SolveQuad(a, b, result), 2); // 1, -3
    EXPECT_QUAD(a, b, result[0]);
    EXPECT_QUAD(a, b, result[1]);
}

TEST(MathEquation, SolveQuadFloatAIsZero) {
    float a, b;
    float result[2];

    a = 0;
    b = 3.f;
    ASSERT_EQ(math::SolveQuad(a, b, result), 0);

    a = 0;
    b = -4.f;
    ASSERT_EQ(math::SolveQuad(a, b, result), 2);
    EXPECT_QUAD(a, b, result[0]);
    EXPECT_QUAD(a, b, result[1]);
}

TEST(MathEquation, SolveQuadFloatBIsZero) {
    float a, b;
    float result[2];

    a = 1.f;
    b = 0;
    ASSERT_EQ(math::SolveQuad(a, b, result), 2);
    EXPECT_QUAD(a, b, result[0]);
    EXPECT_QUAD(a, b, result[1]);
}

TEST(MathEquation, SolveQuadFloatAIsZeroBIsZero) {
    float a, b;
    float result[2];

    a = 0;
    b = 0;
    ASSERT_EQ(math::SolveQuad(a, b, result), 2);
    EXPECT_QUAD(a, b, result[0]);
    EXPECT_QUAD(a, b, result[1]);
}

TEST(MathEquation, SolveFullQuadDouble) {
    double a, b, c;
    double result[2];

    a = 2.;
    b = 3.;
    c = 4.;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 0);

    a = 2.;
    b = 12.;
    c = 18.;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2);
    EXPECT_FULL_QUAD(a, b, c, result[0]);
    EXPECT_FULL_QUAD(a, b, c, result[1]);

    a = 2.;
    b = 4.;
    c = -6.;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2);
    EXPECT_FULL_QUAD(a, b, c, result[0]);
    EXPECT_FULL_QUAD(a, b, c, result[1]);
}

TEST(MathEquation, SolveFullQuadDoubleAIsZero) {
    double a, b, c;
    double result[2];

    a = 0.;
    b = 2.;
    c = 3.;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 1); // -2
    EXPECT_FULL_QUAD(a, b, c, result[0]);
}

TEST(MathEquation, SolveFullQuadDoubleBIsZero) {
    double a, b, c;
    double result[2];

    a = 2.;
    b = 0;
    c = 3.;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 0);

    a = 2.;
    b = 0;
    c = -4.;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2);
    EXPECT_FULL_QUAD(a, b, c, result[0]);
    EXPECT_FULL_QUAD(a, b, c, result[1]);
}

TEST(MathEquation, SolveFullQuadDoubleCIsZero) {
    double a, b, c;
    double result[2];

    a = 2.;
    b = 4.;
    c = 0;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2);
    EXPECT_FULL_QUAD(a, b, c, result[0]);
    EXPECT_FULL_QUAD(a, b, c, result[1]);
}

TEST(MathEquation, SolveFullQuadDoubleAIsZeroBIsZero) {
    double a, b, c;
    double result[2];

    a = 0.;
    b = 0.;
    c = 3.;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 0);
}

TEST(MathEquation, SolveFullQuadDoubleBIsZeroCIsZero) {
    double a, b, c;
    double result[2];

    a = 2.;
    b = 0.;
    c = 0.;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2);
    EXPECT_FULL_QUAD(a, b, c, result[0]);
    EXPECT_FULL_QUAD(a, b, c, result[1]);
}

TEST(MathEquation, SolveFullQuadFloat) {
    float a, b, c;
    float result[2];

    a = 2.f;
    b = 3.f;
    c = 4.f;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 0);

    a = 2.f;
    b = 12.f;
    c = 18.f;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2);
    EXPECT_FULL_QUAD(a, b, c, result[0]);
    EXPECT_FULL_QUAD(a, b, c, result[1]);

    a = 2.f;
    b = 4.f;
    c = -6.f;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2);
    EXPECT_FULL_QUAD(a, b, c, result[0]);
    EXPECT_FULL_QUAD(a, b, c, result[1]);
}

TEST(MathEquation, SolveFullQuadFloatAIsZero) {
    float a, b, c;
    float result[2];

    a = 0.f;
    b = 2.f;
    c = 3.f;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 1); // -2
    EXPECT_FULL_QUAD(a, b, c, result[0]);
}

TEST(MathEquation, SolveFullQuadFloatBIsZero) {
    float a, b, c;
    float result[2];

    a = 2.f;
    b = 0;
    c = 3.f;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 0);

    a = 2.f;
    b = 0;
    c = -4.f;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2);
    EXPECT_FULL_QUAD(a, b, c, result[0]);
    EXPECT_FULL_QUAD(a, b, c, result[1]);
}

TEST(MathEquation, SolveFullQuadFloatCIsZero) {
    float a, b, c;
    float result[2];

    a = 2.f;
    b = 4.f;
    c = 0;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2);
    EXPECT_FULL_QUAD(a, b, c, result[0]);
    EXPECT_FULL_QUAD(a, b, c, result[1]);
}

TEST(MathEquation, SolveFullQuadFloatAIsZeroBIsZero) {
    float a, b, c;
    float result[2];

    a = 0;
    b = 0;
    c = 3.f;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 0);
}

TEST(MathEquation, SolveFullQuadFloatBIsZeroCIsZero) {
    float a, b, c;
    float result[2];

    a = 2.f;
    b = 0;
    c = 0;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2);
    EXPECT_FULL_QUAD(a, b, c, result[0]);
    EXPECT_FULL_QUAD(a, b, c, result[1]);
}

TEST(MathEquation, SolveCubicDouble) {
    double a, b, c, d;
    double result[3];

    a = 1.;
    b = 2.;
    c = 3.;
    d = 4.;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 1); // - 1.6506
    EXPECT_FULL_CUBIC(a, b, c, d, result[0]);

    a = 1.;
    b = 2.;
    c = 3.;
    d = -1.;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 1); // - 0.27568
    EXPECT_FULL_CUBIC(a, b, c, d, result[0]);

    a = 5.;
    b = -8.;
    c = -8.;
    d = 5.;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 3);
    EXPECT_FULL_CUBIC(a, b, c, d, result[0]);
    EXPECT_FULL_CUBIC(a, b, c, d, result[1]);
    EXPECT_FULL_CUBIC(a, b, c, d, result[2]);
}

TEST(MathEquation, SolveCubicDoubleAIsZero) {
    double a, b, c, d;
    double result[3];

    a = 0.;
    b = 1.;
    c = 6.;
    d = 9.;
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
