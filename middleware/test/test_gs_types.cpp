#include <string>
#include <cstdint>

#include "test/test.h"
#include "eigen/core.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_types.h"


namespace {

TEST(GSTypes, MaxTypeId) {
    ASSERT_EQ(gs::TypeId::Float, gs::MaxTypeId(gs::TypeId::Float, gs::TypeId::Float));
    ASSERT_EQ(gs::TypeId::Vector2f, gs::MaxTypeId(gs::TypeId::Float, gs::TypeId::Vector2f));
    ASSERT_EQ(gs::TypeId::Generator3d, gs::MaxTypeId(gs::TypeId::Vector4f, gs::TypeId::Generator3d));

    ASSERT_EQ(gs::TypeId::Float, gs::MaxTypeId(gs::TypeId::Float, gs::TypeId::UniversalFloat));
    ASSERT_EQ(gs::TypeId::Vector2f, gs::MaxTypeId(gs::TypeId::Vector2f, gs::TypeId::UniversalFloat));
    ASSERT_EQ(gs::TypeId::Generator3d, gs::MaxTypeId(gs::TypeId::UniversalVector4f, gs::TypeId::UniversalGenerator3d));
}

TEST(GSTypes, ToUniversalTypeId) {
    ASSERT_EQ(gs::TypeId::UniversalFloat, gs::ToUniversalTypeId(gs::TypeId::Float));
    ASSERT_EQ(gs::TypeId::UniversalVector2f, gs::ToUniversalTypeId(gs::TypeId::Vector2f));
    ASSERT_EQ(gs::TypeId::UniversalVector3f, gs::ToUniversalTypeId(gs::TypeId::Vector3f));
    ASSERT_EQ(gs::TypeId::UniversalVector4f, gs::ToUniversalTypeId(gs::TypeId::Vector4f));
    ASSERT_EQ(gs::TypeId::UniversalGenerator2d, gs::ToUniversalTypeId(gs::TypeId::Generator2d));
    ASSERT_EQ(gs::TypeId::UniversalGenerator3d, gs::ToUniversalTypeId(gs::TypeId::Generator3d));

    ASSERT_EQ(gs::TypeId::UniversalFloat, gs::ToUniversalTypeId(gs::TypeId::UniversalFloat));
    ASSERT_EQ(gs::TypeId::UniversalVector2f, gs::ToUniversalTypeId(gs::TypeId::UniversalVector2f));
    ASSERT_EQ(gs::TypeId::UniversalVector3f, gs::ToUniversalTypeId(gs::TypeId::UniversalVector3f));
    ASSERT_EQ(gs::TypeId::UniversalVector4f, gs::ToUniversalTypeId(gs::TypeId::UniversalVector4f));
    ASSERT_EQ(gs::TypeId::UniversalGenerator2d, gs::ToUniversalTypeId(gs::TypeId::UniversalGenerator2d));
    ASSERT_EQ(gs::TypeId::UniversalGenerator3d, gs::ToUniversalTypeId(gs::TypeId::UniversalGenerator3d));
}

TEST(GSTypes, ToBaseTypeId) {
    ASSERT_EQ(gs::TypeId::Float, gs::ToBaseTypeId(gs::TypeId::UniversalFloat));
    ASSERT_EQ(gs::TypeId::Vector2f, gs::ToBaseTypeId(gs::TypeId::UniversalVector2f));
    ASSERT_EQ(gs::TypeId::Vector3f, gs::ToBaseTypeId(gs::TypeId::UniversalVector3f));
    ASSERT_EQ(gs::TypeId::Vector4f, gs::ToBaseTypeId(gs::TypeId::UniversalVector4f));
    ASSERT_EQ(gs::TypeId::Generator2d, gs::ToBaseTypeId(gs::TypeId::UniversalGenerator2d));
    ASSERT_EQ(gs::TypeId::Generator3d, gs::ToBaseTypeId(gs::TypeId::UniversalGenerator3d));

    ASSERT_EQ(gs::TypeId::Float, gs::ToBaseTypeId(gs::TypeId::Float));
    ASSERT_EQ(gs::TypeId::Vector2f, gs::ToBaseTypeId(gs::TypeId::Vector2f));
    ASSERT_EQ(gs::TypeId::Vector3f, gs::ToBaseTypeId(gs::TypeId::Vector3f));
    ASSERT_EQ(gs::TypeId::Vector4f, gs::ToBaseTypeId(gs::TypeId::Vector4f));
    ASSERT_EQ(gs::TypeId::Generator2d, gs::ToBaseTypeId(gs::TypeId::Generator2d));
    ASSERT_EQ(gs::TypeId::Generator3d, gs::ToBaseTypeId(gs::TypeId::Generator3d));
}

TEST(GSTypes, IsConcreteUniversalType) {
    ASSERT_TRUE(gs::IsConcreteUniversalType(gs::TypeId::UniversalFloat));
    ASSERT_TRUE(gs::IsConcreteUniversalType(gs::TypeId::UniversalVector2f));
    ASSERT_TRUE(gs::IsConcreteUniversalType(gs::TypeId::UniversalVector3f));
    ASSERT_TRUE(gs::IsConcreteUniversalType(gs::TypeId::UniversalVector4f));
    ASSERT_TRUE(gs::IsConcreteUniversalType(gs::TypeId::UniversalGenerator2d));
    ASSERT_TRUE(gs::IsConcreteUniversalType(gs::TypeId::UniversalGenerator3d));

    ASSERT_FALSE(gs::IsConcreteUniversalType(gs::TypeId::Float));
    ASSERT_FALSE(gs::IsConcreteUniversalType(gs::TypeId::Vector2f));
    ASSERT_FALSE(gs::IsConcreteUniversalType(gs::TypeId::Vector3f));
    ASSERT_FALSE(gs::IsConcreteUniversalType(gs::TypeId::Vector4f));
    ASSERT_FALSE(gs::IsConcreteUniversalType(gs::TypeId::Generator2d));
    ASSERT_FALSE(gs::IsConcreteUniversalType(gs::TypeId::Generator3d));

    ASSERT_FALSE(gs::IsConcreteUniversalType(gs::TypeId::UniversalType));
}

TEST(GSTypes, GetTypeIdByT) {
    ASSERT_EQ(gs::TypeId::Float, gs::GetTypeId<float>());
    ASSERT_EQ(gs::TypeId::Vector3f, gs::GetTypeId<Eigen::Vector3f>());
    ASSERT_EQ(gs::TypeId::Generator2d, gs::GetTypeId<math::Generator2D>());
    ASSERT_EQ(gs::TypeId::UniversalType, gs::GetTypeId<gs::UniversalType>());
}

TEST(GSTypes, GetTypeIdByValue) {
    ASSERT_EQ(gs::TypeId::Float, gs::GetTypeId(typeid(float)));
    ASSERT_EQ(gs::TypeId::Vector3f, gs::GetTypeId(typeid(Eigen::Vector3f)));
    ASSERT_EQ(gs::TypeId::Generator2d, gs::GetTypeId(typeid(math::Generator2D)));
    ASSERT_EQ(gs::TypeId::UniversalType, gs::GetTypeId(typeid(gs::UniversalType)));
}

}
