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

}
