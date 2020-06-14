#include <string>

#include "test/test.h"
#include "eigen/core.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/graph/gs_convert.h"


#define ASSERT_CONVERT_TRUE(From, To) ASSERT_TRUE((gs::CanConvert<To, From>));
#define ASSERT_CONVERT_FALSE(From, To) ASSERT_FALSE((gs::CanConvert<To, From>));

namespace {

TEST(GSConvert, ToFloat) {
    ASSERT_CONVERT_TRUE(float, float);
    ASSERT_CONVERT_TRUE(Eigen::Vector2f, float);
    ASSERT_CONVERT_FALSE(math::Generator2D, float);
    ASSERT_CONVERT_FALSE(gs::UniversalType, float);
}

TEST(GSConvert, ToVector3f) {
    ASSERT_CONVERT_TRUE(float, Eigen::Vector3f);
    ASSERT_CONVERT_FALSE(Eigen::Vector2f, Eigen::Vector3f);
    ASSERT_CONVERT_TRUE(Eigen::Vector3f, Eigen::Vector3f);
    ASSERT_CONVERT_FALSE(math::Generator2D, Eigen::Vector3f);
    ASSERT_CONVERT_FALSE(gs::UniversalType, Eigen::Vector3f);
}

TEST(GSConvert, ToGenerator3D) {
    ASSERT_CONVERT_TRUE(float, math::Generator3D);
    ASSERT_CONVERT_TRUE(Eigen::Vector4f, math::Generator3D);
    ASSERT_CONVERT_FALSE(math::Generator2D, math::Generator3D);
    ASSERT_CONVERT_TRUE(math::Generator3D, math::Generator3D);
    ASSERT_CONVERT_FALSE(gs::UniversalType, math::Generator3D);
}

TEST(GSConvert, ToUniversalType) {
    ASSERT_CONVERT_FALSE(float, gs::UniversalType);
    ASSERT_CONVERT_FALSE(Eigen::Vector4f, gs::UniversalType);
    ASSERT_CONVERT_FALSE(math::Generator2D, gs::UniversalType);
    ASSERT_CONVERT_FALSE(gs::UniversalType, gs::UniversalType);
}

}
