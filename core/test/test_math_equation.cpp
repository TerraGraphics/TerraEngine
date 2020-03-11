#include <memory>
#include <random>
#include <chrono>
#include <gtest/gtest.h>

#include "core/math/equation.h"


#define ASSERT_FLOAT3_EQ(actual, expected) \
    ASSERT_FLOAT_EQ(actual.x, expected.x); \
    ASSERT_FLOAT_EQ(actual.y, expected.y); \
    ASSERT_FLOAT_EQ(actual.z, expected.z);

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
    ASSERT_DOUBLE_EQ(a * result[0] * result[0] + b * result[0] + c, 0);
    ASSERT_DOUBLE_EQ(a * result[1] * result[1] + b * result[1] + c, 0);

    a = 1.;
    b = 2.;
    c = -3.;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2); // 1, -3
    ASSERT_DOUBLE_EQ(a * result[0] * result[0] + b * result[0] + c, 0);
    ASSERT_DOUBLE_EQ(a * result[1] * result[1] + b * result[1] + c, 0);
}

TEST(MathEquation, SolveQuadDoubleAIsZero) {
    double a, b, c;
    double result[2];

    a = 0.;
    b = 2.;
    c = 3.;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 1); // -2
    ASSERT_DOUBLE_EQ(a * result[0] * result[0] + b * result[0] + c, 0);
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
    ASSERT_FLOAT_EQ(a * result[0] * result[0] + b * result[0] + c, 0);
    ASSERT_FLOAT_EQ(a * result[1] * result[1] + b * result[1] + c, 0);

    a = 1.f;
    b = 2.f;
    c = -3.f;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 2); // 1, -3
    ASSERT_FLOAT_EQ(a * result[0] * result[0] + b * result[0] + c, 0);
    ASSERT_FLOAT_EQ(a * result[1] * result[1] + b * result[1] + c, 0);
}

TEST(MathEquation, SolveQuadFloatAIsZero) {
    float a, b, c;
    float result[2];

    a = 0.f;
    b = 2.f;
    c = 3.f;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 1); // -2
    ASSERT_FLOAT_EQ(a * result[0] * result[0] + b * result[0] + c, 0);
}

TEST(MathEquation, SolveQuadFloatAIsZeroBIsZero) {
    float a, b, c;
    float result[2];

    a = 0.f;
    b = 0.f;
    c = 3.f;
    ASSERT_EQ(math::SolveQuad(a, b, c, result), 0);
}

}
