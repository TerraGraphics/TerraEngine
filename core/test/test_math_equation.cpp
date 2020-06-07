#include <memory>

#include "test/test.h"
#include "core/math/equation.h"


template<typename T> static T Linear(T a, T b, T result) {
    return a * result + b;
}

#define EXPECT_LINEAR(a, b, result) \
    EXPECT_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<decltype(a)>, Linear(a, b, result), 0)

template<typename T> static T QuadAbsError() {
    if constexpr (std::is_same_v<T, double>) {
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
    if constexpr (std::is_same_v<T, double>) {
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

template<typename T> static T QuarticAbsError() {
    if constexpr (std::is_same_v<T, double>) {
        return 1.e-12;
    }

    return 1.e-5f;
}

template<typename T> static T Quartic(T a, T b, T c, T d, T e, T result) {
    double r = static_cast<double>(result);
    return static_cast<T>((((static_cast<double>(a) * r + static_cast<double>(b)) * r + static_cast<double>(c)) * r + static_cast<double>(d)) * r + static_cast<double>(e));
}

#define EXPECT_QUARTIC(a, b, c, d, result) \
    EXPECT_NEAR(Quartic(static_cast<decltype(a)>(1.), a, b, c, d, result), 0, QuarticAbsError<decltype(a)>());

#define EXPECT_FULL_QUARTIC(a, b, c, d, e, result) \
    EXPECT_NEAR(Quartic(a, b, c, d, e, result), 0, QuarticAbsError<decltype(a)>());

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

TYPED_TEST(MathEquation, SolveQuartic) {
    TypeParam a, b, c, d;
    TypeParam result[4];

    a = 1;
    b = 2;
    c = 3;
    d = 4;
    ASSERT_EQ(math::SolveQuartic(a, b, c, d, result), 0);

    a = 3;
    b = 3;
    c = -1;
    d = -6;
    ASSERT_EQ(math::SolveQuartic(a, b, c, d, result), 2);
    EXPECT_QUARTIC(a, b, c, d, result[0]);
    EXPECT_QUARTIC(a, b, c, d, result[1]);

    a = -3;
    b = static_cast<TypeParam>(-4. / 9.);
    c = 3;
    d = 1;
    ASSERT_EQ(math::SolveQuartic(a, b, c, d, result), 4);
    EXPECT_QUARTIC(a, b, c, d, result[0]);
    EXPECT_QUARTIC(a, b, c, d, result[1]);
    EXPECT_QUARTIC(a, b, c, d, result[2]);
    EXPECT_QUARTIC(a, b, c, d, result[3]);
}

TYPED_TEST(MathEquation, SolveQuarticAIsZero) {
    TypeParam a, b, c, d;
    TypeParam result[4];

    a = 0;
    b = -1;
    c = 2;
    d = -1;
    ASSERT_EQ(math::SolveQuartic(a, b, c, d, result), 2);
    EXPECT_QUARTIC(a, b, c, d, result[0]);
    EXPECT_QUARTIC(a, b, c, d, result[1]);
}

TYPED_TEST(MathEquation, SolveFullQuartic) {
    TypeParam a, b, c, d, e;
    TypeParam result[4];

    a = 2;
    b = 2;
    c = 3;
    d = 6;
    e = 8;
    ASSERT_EQ(math::SolveQuartic(a, b, c, d, e, result), 0);

    a = 2;
    b = 6;
    c = 6;
    d = -2;
    e = -12;
    ASSERT_EQ(math::SolveQuartic(a, b, c, d, e, result), 2);
    EXPECT_FULL_QUARTIC(a, b, c, d, e, result[0]);
    EXPECT_FULL_QUARTIC(a, b, c, d, e, result[1]);

    a = 6;
    b = -35;
    c = 62;
    d = -35;
    e = 6;
    ASSERT_EQ(math::SolveQuartic(a, b, c, d, e, result), 4);
    EXPECT_FULL_QUARTIC(a, b, c, d, e, result[0]);
    EXPECT_FULL_QUARTIC(a, b, c, d, e, result[1]);
    EXPECT_FULL_QUARTIC(a, b, c, d, e, result[2]);
    EXPECT_FULL_QUARTIC(a, b, c, d, e, result[3]);
}

TYPED_TEST(MathEquation, SolveFullQuarticAIsZero) {
    TypeParam a, b, c, d, e;
    TypeParam result[4];

    a = 0;
    b = 2;
    c = -11;
    d = 12;
    e = 9;
    ASSERT_EQ(math::SolveQuartic(a, b, c, d, e, result), 3);
    EXPECT_FULL_QUARTIC(a, b, c, d, e, result[0]);
    EXPECT_FULL_QUARTIC(a, b, c, d, e, result[1]);
    EXPECT_FULL_QUARTIC(a, b, c, d, e, result[2]);
}

}
