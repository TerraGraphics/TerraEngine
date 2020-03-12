#include <memory>
#include <random>
#include <chrono>
#include <gtest/gtest.h>

#include "core/math/equation.h"


#define EXPECT_QUAD(a, b, c, result) \
    ASSERT_DOUBLE_EQ(a * result * result + b * result + c, 0);

#define EXPECT_QUAD_FLOAT(a, b, c, result) \
    ASSERT_FLOAT_EQ(a * result * result + b * result + c, 0);

#define EXPECT_CUBIC(a, b, c, d, result) \
    EXPECT_NEAR(a * result * result * result + b * result * result + c * result + d, 0, 1.e-13);

#define EXPECT_CUBIC_FLOAT(a, b, c, d, result) \
    EXPECT_NEAR(a * result * result * result + b * result * result + c * result + d, 0, 1.e-5);

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
    double a, b, c;
    double result[2];

    a = 1.;
    b = 2.;
    c = 3.;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 0);

    a = 1.;
    b = 6.;
    c = 9.;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2); // -3, -3
    EXPECT_QUAD(a, b, c, result[0]);
    EXPECT_QUAD(a, b, c, result[1]);

    a = 1.;
    b = 2.;
    c = -3.;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2); // 1, -3
    EXPECT_QUAD(a, b, c, result[0]);
    EXPECT_QUAD(a, b, c, result[1]);
}

TEST(MathEquation, SolveQuadDoubleAIsZero) {
    double a, b, c;
    double result[2];

    a = 0.;
    b = 2.;
    c = 3.;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 1); // -2
    EXPECT_QUAD(a, b, c, result[0]);
}

TEST(MathEquation, SolveQuadDoubleAIsZeroBIsZero) {
    double a, b, c;
    double result[2];

    a = 0.;
    b = 0.;
    c = 3.;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 0);
}

TEST(MathEquation, SolveQuadFloat) {
    float a, b, c;
    float result[2];

    a = 1.f;
    b = 2.f;
    c = 3.f;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 0);

    a = 1.f;
    b = 6.f;
    c = 9.f;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2); // -3, -3
    EXPECT_QUAD_FLOAT(a, b, c, result[0]);
    EXPECT_QUAD_FLOAT(a, b, c, result[1]);

    a = 1.f;
    b = 2.f;
    c = -3.f;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2); // 1, -3
    EXPECT_QUAD_FLOAT(a, b, c, result[0]);
    EXPECT_QUAD_FLOAT(a, b, c, result[1]);
}

TEST(MathEquation, SolveQuadFloatAIsZero) {
    float a, b, c;
    float result[2];

    a = 0.f;
    b = 2.f;
    c = 3.f;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 1); // -2
    EXPECT_QUAD_FLOAT(a, b, c, result[0]);
}

TEST(MathEquation, SolveQuadFloatAIsZeroBIsZero) {
    float a, b, c;
    float result[2];

    a = 0.f;
    b = 0.f;
    c = 3.f;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 0);
}

TEST(MathEquation, SolveCubicDouble) {
    double a, b, c, d;
    double result[3];

    a = 1.;
    b = 2.;
    c = 3.;
    d = 4.;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 1); // - 1.6506
    EXPECT_CUBIC(a, b, c, d, result[0]);

    a = 1.;
    b = 2.;
    c = 3.;
    d = -1.;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 1); // - 0.27568
    EXPECT_CUBIC(a, b, c, d, result[0]);

    a = 5.;
    b = -8.;
    c = -8.;
    d = 5.;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 3);
    EXPECT_CUBIC(a, b, c, d, result[0]);
    EXPECT_CUBIC(a, b, c, d, result[1]);
    EXPECT_CUBIC(a, b, c, d, result[2]);
}

TEST(MathEquation, SolveCubicDoubleAIsZero) {
    double a, b, c, d;
    double result[3];

    a = 0.;
    b = 1.;
    c = 6.;
    d = 9.;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 2);
    EXPECT_CUBIC(a, b, c, d, result[0]);
    EXPECT_CUBIC(a, b, c, d, result[1]);
}

TEST(MathEquation, SolveCubicFloat) {
    float a, b, c, d;
    float result[3];

    a = 1.f;
    b = 2.f;
    c = 3.f;
    d = 4.f;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 1); // - 1.6506
    EXPECT_CUBIC_FLOAT(a, b, c, d, result[0]);

    a = 1.f;
    b = 2.f;
    c = 3.f;
    d = -1.f;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 1); // - 0.27568
    EXPECT_CUBIC_FLOAT(a, b, c, d, result[0]);

    a = 5.f;
    b = -8.f;
    c = -8.f;
    d = 5.f;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 3);
    EXPECT_CUBIC_FLOAT(a, b, c, d, result[0]);
    EXPECT_CUBIC_FLOAT(a, b, c, d, result[1]);
    EXPECT_CUBIC_FLOAT(a, b, c, d, result[2]);
}

TEST(MathEquation, SolveCubicFloatAIsZero) {
    float a, b, c, d;
    float result[3];

    a = 0.f;
    b = 1.f;
    c = 6.f;
    d = 9.f;
    ASSERT_EQ(math::SolveCubic(a, b, c, d, result), 2);
    EXPECT_CUBIC_FLOAT(a, b, c, d, result[0]);
    EXPECT_CUBIC_FLOAT(a, b, c, d, result[1]);
}

}
