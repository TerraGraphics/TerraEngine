#include <string>

#include "test/test.h"
#include "eigen/core.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_convert.h"
#include "middleware/gscheme/graph/gs_universal_type.h"


#define ASSERT_CONVERT_TRUE(From, To) ASSERT_TRUE((gs::CanConvert<To, From>));
#define ASSERT_CONVERT_FALSE(From, To) ASSERT_FALSE((gs::CanConvert<To, From>));

namespace {

TEST(gsConvert, ToFloat) {
    ASSERT_CONVERT_TRUE(float, float);
    ASSERT_CONVERT_TRUE(Eigen::Vector2f, float);
    ASSERT_CONVERT_FALSE(math::Generator2D, float);
    ASSERT_CONVERT_FALSE(gs::UniversalType, float);
}

TEST(gsConvert, ToVector3f) {
    ASSERT_CONVERT_TRUE(float, Eigen::Vector3f);
    ASSERT_CONVERT_FALSE(Eigen::Vector2f, Eigen::Vector3f);
    ASSERT_CONVERT_TRUE(Eigen::Vector3f, Eigen::Vector3f);
    ASSERT_CONVERT_FALSE(math::Generator2D, Eigen::Vector3f);
    ASSERT_CONVERT_FALSE(gs::UniversalType, Eigen::Vector3f);
}

TEST(gsConvert, ToGenerator3D) {
    ASSERT_CONVERT_TRUE(float, math::Generator3D);
    ASSERT_CONVERT_TRUE(Eigen::Vector4f, math::Generator3D);
    ASSERT_CONVERT_FALSE(math::Generator2D, math::Generator3D);
    ASSERT_CONVERT_TRUE(math::Generator3D, math::Generator3D);
    ASSERT_CONVERT_FALSE(gs::UniversalType, math::Generator3D);
}

TEST(gsConvert, ToUniversalType) {
    ASSERT_CONVERT_TRUE(float, gs::UniversalType);
    ASSERT_CONVERT_TRUE(Eigen::Vector4f, gs::UniversalType);
    ASSERT_CONVERT_TRUE(math::Generator2D, gs::UniversalType);
    ASSERT_CONVERT_FALSE(gs::UniversalType, gs::UniversalType);
}

TEST(gsConvert, FromUniversalType) {
    gs::UniversalType value;

    value = float(1.f);
    ASSERT_TRUE(gs::IsCanConvertUniversalType<float>(value));
    ASSERT_TRUE(gs::IsCanConvertUniversalType<gs::UniversalType>(value));

    value = Eigen::Vector2f(1.f, 2.f);
    ASSERT_TRUE(gs::IsCanConvertUniversalType<float>(value));
    ASSERT_TRUE(gs::IsCanConvertUniversalType<Eigen::Vector2f>(value));
    ASSERT_FALSE(gs::IsCanConvertUniversalType<Eigen::Vector4f>(value));
    ASSERT_TRUE(gs::IsCanConvertUniversalType<gs::UniversalType>(value));
}

}
