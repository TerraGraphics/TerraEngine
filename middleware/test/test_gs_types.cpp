#include <string>

#include "test/test.h"
#include "middleware/gscheme/graph/gs_types.h"


namespace {

TEST(GSTypes, MaxTypeId) {
    ASSERT_EQ(gs::TypeId::Float, gs::MaxTypeId(gs::TypeId::Float, gs::TypeId::Float));
    ASSERT_EQ(gs::TypeId::Vector2f, gs::MaxTypeId(gs::TypeId::Float, gs::TypeId::Vector2f));
    ASSERT_EQ(gs::TypeId::Generator3D, gs::MaxTypeId(gs::TypeId::Vector4f, gs::TypeId::Generator3D));

    ASSERT_EQ(gs::TypeId::Float, gs::MaxTypeId(gs::TypeId::Float, gs::TypeId::UniversalFloat));
    ASSERT_EQ(gs::TypeId::Vector2f, gs::MaxTypeId(gs::TypeId::Vector2f, gs::TypeId::UniversalFloat));
    ASSERT_EQ(gs::TypeId::Generator3D, gs::MaxTypeId(gs::TypeId::UniversalVector4f, gs::TypeId::UniversalGenerator3D));
}

TEST(GSTypes, GetTypeIdByT) {
    ASSERT_EQ(gs::TypeId::Int8, gs::GetTypeId<int8_t>());
    ASSERT_EQ(gs::TypeId::Uint16, gs::GetTypeId<uint16_t>());
    ASSERT_EQ(gs::TypeId::Float, gs::GetTypeId<float>());
    ASSERT_EQ(gs::TypeId::String, gs::GetTypeId<std::string>());
    ASSERT_EQ(gs::TypeId::Vector3f, gs::GetTypeId<Eigen::Vector3f>());
    ASSERT_EQ(gs::TypeId::Generator2D, gs::GetTypeId<math::Generator2D>());
    ASSERT_EQ(gs::TypeId::UniversalType, gs::GetTypeId<gs::UniversalType>());
}

TEST(GSTypes, GetTypeIdByValue) {
    ASSERT_EQ(gs::TypeId::Int8, gs::GetTypeId(typeid(int8_t)));
    ASSERT_EQ(gs::TypeId::Uint16, gs::GetTypeId(typeid(uint16_t)));
    ASSERT_EQ(gs::TypeId::Float, gs::GetTypeId(typeid(float)));
    ASSERT_EQ(gs::TypeId::String, gs::GetTypeId(typeid(std::string)));
    ASSERT_EQ(gs::TypeId::Vector3f, gs::GetTypeId(typeid(Eigen::Vector3f)));
    ASSERT_EQ(gs::TypeId::Generator2D, gs::GetTypeId(typeid(math::Generator2D)));
    ASSERT_EQ(gs::TypeId::UniversalType, gs::GetTypeId(typeid(gs::UniversalType)));
}

}
