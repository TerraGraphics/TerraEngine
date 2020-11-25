#include <string>
#include <cstdint>

#include "test/test.h"
#include "eigen/core.h"
#include "core/math/generator_type.h"
#include "middleware/gschema/graph/gs_types.h"
#include "middleware/gschema/graph/gs_types_convert.h"


#define ASSERT_CONVERT_TRUE(From, To) ASSERT_TRUE((gs::CanConvert<To, From>))
#define ASSERT_CONVERT_FALSE(From, To) ASSERT_FALSE((gs::CanConvert<To, From>))

namespace {

TEST(GSConvert, ToUint64) {
    ASSERT_CONVERT_FALSE(uint8_t, uint64_t);
    ASSERT_CONVERT_FALSE(int16_t, uint64_t);
    ASSERT_CONVERT_FALSE(uint32_t, uint64_t);
    ASSERT_CONVERT_FALSE(int64_t, uint64_t);
    ASSERT_CONVERT_FALSE(uint64_t, uint64_t);
    ASSERT_CONVERT_FALSE(float, uint64_t);
    ASSERT_CONVERT_FALSE(Eigen::Vector2f, uint64_t);
    ASSERT_CONVERT_FALSE(math::Generator2D, uint64_t);
    ASSERT_CONVERT_FALSE(gs::UniversalType, uint64_t);
}

TEST(GSConvert, ToFloat) {
    ASSERT_CONVERT_FALSE(uint8_t, float);
    ASSERT_CONVERT_TRUE(float, float);
    ASSERT_CONVERT_TRUE(Eigen::Vector2f, float);
    ASSERT_CONVERT_FALSE(math::Generator2D, float);
    ASSERT_CONVERT_FALSE(gs::UniversalType, float);

    ASSERT_FLOAT_EQ(1.f, gs::ConvertTo<float>(1.f));
    ASSERT_FLOAT_EQ(2.f, gs::ConvertTo<float>(Eigen::Vector2f(2.f, 3.f)));
}

TEST(GSConvert, ToVector3f) {
    ASSERT_CONVERT_FALSE(uint16_t, Eigen::Vector3f);
    ASSERT_CONVERT_TRUE(float, Eigen::Vector3f);
    ASSERT_CONVERT_FALSE(Eigen::Vector2f, Eigen::Vector3f);
    ASSERT_CONVERT_TRUE(Eigen::Vector3f, Eigen::Vector3f);
    ASSERT_CONVERT_FALSE(math::Generator2D, Eigen::Vector3f);
    ASSERT_CONVERT_FALSE(gs::UniversalType, Eigen::Vector3f);

    ASSERT_EQ(Eigen::Vector3f(1.f, 1.f, 1.f), gs::ConvertTo<Eigen::Vector3f>(1.f));
    ASSERT_EQ(Eigen::Vector3f(1.f, 2.f, 3.f), gs::ConvertTo<Eigen::Vector3f>(Eigen::Vector3f(1.f, 2.f, 3.f)));
}

TEST(GSConvert, ToGenerator3d) {
    ASSERT_CONVERT_FALSE(int32_t, math::Generator3D);
    ASSERT_CONVERT_TRUE(float, math::Generator3D);
    ASSERT_CONVERT_TRUE(Eigen::Vector4f, math::Generator3D);
    ASSERT_CONVERT_FALSE(math::Generator2D, math::Generator3D);
    ASSERT_CONVERT_TRUE(math::Generator3D, math::Generator3D);
    ASSERT_CONVERT_FALSE(gs::UniversalType, math::Generator3D);

    EXPECT_DOUBLE_EQ(1., gs::ConvertTo<math::Generator3D>(1.f)(1.1, 2.2, 3.3));
}

TEST(GSConvert, ToUniversalType) {
    ASSERT_CONVERT_FALSE(int64_t, gs::UniversalType);
    ASSERT_CONVERT_FALSE(float, gs::UniversalType);
    ASSERT_CONVERT_FALSE(Eigen::Vector4f, gs::UniversalType);
    ASSERT_CONVERT_FALSE(math::Generator2D, gs::UniversalType);
    ASSERT_CONVERT_FALSE(gs::UniversalType, gs::UniversalType);
}

}
