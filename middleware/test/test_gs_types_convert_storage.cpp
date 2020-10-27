#include <memory>
#include <variant>

#include "test/test.h"
#include "eigen/core.h"
#include "cpgf/variant.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/graph/gs_types_convert_storage.h"


#define ASSERT_CONVERT_TRUE(From, To) ASSERT_TRUE(storage.CanConvert(gs::TypeId::From, gs::TypeId::To))
#define ASSERT_CONVERT_FALSE(From, To) ASSERT_FALSE(storage.CanConvert(gs::TypeId::From, gs::TypeId::To))

namespace {

TEST(GSTypesConvertStorage, ToUint64) {
    gs::TypesConvertStorage storage;

    ASSERT_CONVERT_FALSE(Uint8, Uint64);
    ASSERT_CONVERT_FALSE(Int16, Uint64);
    ASSERT_CONVERT_FALSE(Uint32, Uint64);
    ASSERT_CONVERT_FALSE(Int64, Uint64);
    ASSERT_CONVERT_FALSE(Uint64, Uint64);
    ASSERT_CONVERT_FALSE(Float, Uint64);
    ASSERT_CONVERT_FALSE(Vector2f, Uint64);
    ASSERT_CONVERT_FALSE(Generator2d, Uint64);

    ASSERT_CONVERT_FALSE(UniversalFloat, Uint64);
    ASSERT_CONVERT_FALSE(UniversalVector2f, Uint64);
    ASSERT_CONVERT_FALSE(UniversalGenerator2d, Uint64);

    ASSERT_CONVERT_FALSE(UniversalType, Uint64);

    ASSERT_ANY_THROW(storage.GetConvertFunc(gs::TypeId::Uint8, gs::TypeId::Uint64));
}

TEST(GSTypesConvertStorage, ToFloat) {
    gs::TypesConvertStorage storage;

    ASSERT_CONVERT_FALSE(Uint8, Float);
    ASSERT_CONVERT_TRUE(Float, Float);
    ASSERT_CONVERT_TRUE(Vector2f, Float);
    ASSERT_CONVERT_FALSE(Generator2d, Float);

    ASSERT_CONVERT_TRUE(UniversalFloat, Float);
    ASSERT_CONVERT_TRUE(UniversalVector2f, Float);
    ASSERT_CONVERT_FALSE(UniversalGenerator2d, Float);

    ASSERT_CONVERT_FALSE(UniversalType, Float);

    ASSERT_ANY_THROW(storage.GetConvertFunc(gs::TypeId::Uint8, gs::TypeId::Float));

    auto floatToFloat = storage.GetConvertFunc(gs::TypeId::Float, gs::TypeId::Float);
    ASSERT_FLOAT_EQ(1.f, cpgf::fromVariant<float>(floatToFloat(1.f)));

    auto vector2fValue = Eigen::Vector2f(2.f, 3.f);
    auto vector2fToFloat = storage.GetConvertFunc(gs::TypeId::Vector2f, gs::TypeId::Float);
    ASSERT_FLOAT_EQ(2.f, cpgf::fromVariant<float>(vector2fToFloat(vector2fValue)));

    auto universalFloatToFloat = storage.GetConvertFunc(gs::TypeId::UniversalFloat, gs::TypeId::Float);
    ASSERT_FLOAT_EQ(1.f, cpgf::fromVariant<float>(universalFloatToFloat(gs::UniversalType(1.f))));

    auto universalVector2fValue = gs::UniversalType(Eigen::Vector2f(2.f, 3.f));
    auto universalVector2fToFloat = storage.GetConvertFunc(gs::TypeId::UniversalVector2f, gs::TypeId::Float);
    ASSERT_FLOAT_EQ(2.f, cpgf::fromVariant<float>(universalVector2fToFloat(universalVector2fValue)));
}

TEST(GSTypesConvertStorage, ToVector3f) {
    gs::TypesConvertStorage storage;

    ASSERT_CONVERT_FALSE(Uint16, Vector3f);
    ASSERT_CONVERT_TRUE(Float, Vector3f);
    ASSERT_CONVERT_FALSE(Vector2f, Vector3f);
    ASSERT_CONVERT_TRUE(Vector3f, Vector3f);
    ASSERT_CONVERT_FALSE(Generator2d, Vector3f);

    ASSERT_CONVERT_TRUE(UniversalFloat, Vector3f);
    ASSERT_CONVERT_FALSE(UniversalVector2f, Vector3f);
    ASSERT_CONVERT_TRUE(UniversalVector3f, Vector3f);
    ASSERT_CONVERT_FALSE(UniversalGenerator2d, Vector3f);

    ASSERT_CONVERT_FALSE(UniversalType, Vector3f);

    ASSERT_ANY_THROW(storage.GetConvertFunc(gs::TypeId::Uint16, gs::TypeId::Vector3f));

    auto floatToVector3f = storage.GetConvertFunc(gs::TypeId::Float, gs::TypeId::Vector3f);
    ASSERT_EQ(Eigen::Vector3f(1.f, 1.f, 1.f), cpgf::fromVariant<Eigen::Vector3f>(floatToVector3f(1.f)));

    auto vector3fToVector3f = storage.GetConvertFunc(gs::TypeId::Vector3f, gs::TypeId::Vector3f);
    ASSERT_EQ(Eigen::Vector3f(1.f, 2.f, 3.f), cpgf::fromVariant<Eigen::Vector3f>(vector3fToVector3f(Eigen::Vector3f(1.f, 2.f, 3.f))));

    auto universalFloatValue = gs::UniversalType(2.f);
    auto universalFloatToVector3f = storage.GetConvertFunc(gs::TypeId::UniversalFloat, gs::TypeId::Vector3f);
    ASSERT_EQ(Eigen::Vector3f(2.f, 2.f, 2.f), cpgf::fromVariant<Eigen::Vector3f>(universalFloatToVector3f(universalFloatValue)));

    auto universalVector3fValue = gs::UniversalType(Eigen::Vector3f(2.f, 3.f, 4.f));
    auto universalVector3fToVector3f = storage.GetConvertFunc(gs::TypeId::UniversalVector3f, gs::TypeId::Vector3f);
    ASSERT_EQ(Eigen::Vector3f(2.f, 3.f, 4.f), cpgf::fromVariant<Eigen::Vector3f>(universalVector3fToVector3f(universalVector3fValue)));
}

TEST(GSTypesConvertStorage, ToGenerator3d) {
    gs::TypesConvertStorage storage;

    ASSERT_CONVERT_FALSE(Int32, Generator3d);
    ASSERT_CONVERT_TRUE(Float, Generator3d);
    ASSERT_CONVERT_TRUE(Vector4f, Generator3d);
    ASSERT_CONVERT_FALSE(Generator2d, Generator3d);
    ASSERT_CONVERT_TRUE(Generator3d, Generator3d);

    ASSERT_CONVERT_TRUE(UniversalFloat, Generator3d);
    ASSERT_CONVERT_TRUE(UniversalVector4f, Generator3d);
    ASSERT_CONVERT_FALSE(UniversalGenerator2d, Generator3d);
    ASSERT_CONVERT_TRUE(UniversalGenerator3d, Generator3d);

    ASSERT_CONVERT_FALSE(UniversalType, Generator3d);

    ASSERT_ANY_THROW(storage.GetConvertFunc(gs::TypeId::Int32, gs::TypeId::Generator3d));

    auto floatToGenerator3d = storage.GetConvertFunc(gs::TypeId::Float, gs::TypeId::Generator3d);
    ASSERT_DOUBLE_EQ(1., cpgf::fromVariant<math::Generator3D>(floatToGenerator3d(1.f))(1.2, 2.3, 3.4));

    auto vector4fToGenerator3d = storage.GetConvertFunc(gs::TypeId::Vector4f, gs::TypeId::Generator3d);
    ASSERT_DOUBLE_EQ(2., cpgf::fromVariant<math::Generator3D>(vector4fToGenerator3d(Eigen::Vector4f(2.f, 3.f, 4.f, 5.f)))(1.2, 2.3, 3.4));

    auto generatorValue = math::Generator3D(3.f);
    auto generator3dToGenerator3d = storage.GetConvertFunc(gs::TypeId::Generator3d, gs::TypeId::Generator3d);
    ASSERT_DOUBLE_EQ(3., cpgf::fromVariant<math::Generator3D>(generator3dToGenerator3d(generatorValue))(1.2, 2.3, 3.4));

    auto universalFloatValue = gs::UniversalType(4.f);
    auto universalFloatToGenerator3d = storage.GetConvertFunc(gs::TypeId::UniversalFloat, gs::TypeId::Generator3d);
    ASSERT_DOUBLE_EQ(4., cpgf::fromVariant<math::Generator3D>(universalFloatToGenerator3d(universalFloatValue))(1.2, 2.3, 3.4));

    auto universalVector4fValue = gs::UniversalType(Eigen::Vector4f(5.f, 6.f, 7.f, 8.f));
    auto universalVector4fToGenerator3d = storage.GetConvertFunc(gs::TypeId::UniversalVector4f, gs::TypeId::Generator3d);
    ASSERT_DOUBLE_EQ(5., cpgf::fromVariant<math::Generator3D>(universalVector4fToGenerator3d(universalVector4fValue))(1.2, 2.3, 3.4));

    auto universalGeneratorValue = gs::UniversalType(math::Generator3D(6.f));
    auto universalGenerator3dToGenerator3d = storage.GetConvertFunc(gs::TypeId::UniversalGenerator3d, gs::TypeId::Generator3d);
    ASSERT_DOUBLE_EQ(6., cpgf::fromVariant<math::Generator3D>(universalGenerator3dToGenerator3d(universalGeneratorValue))(1.2, 2.3, 3.4));
}

TEST(GSTypesConvertStorage, ToUniversalType) {
    gs::TypesConvertStorage storage;

    ASSERT_CONVERT_FALSE(Int64, UniversalType);
    ASSERT_CONVERT_TRUE(Float, UniversalType);
    ASSERT_CONVERT_TRUE(Vector4f, UniversalType);
    ASSERT_CONVERT_TRUE(Generator2d, UniversalType);

    ASSERT_CONVERT_TRUE(UniversalFloat, UniversalType);
    ASSERT_CONVERT_TRUE(UniversalVector4f, UniversalType);
    ASSERT_CONVERT_TRUE(UniversalGenerator2d, UniversalType);

    ASSERT_CONVERT_FALSE(UniversalType, UniversalType);

    ASSERT_ANY_THROW(storage.GetConvertFunc(gs::TypeId::Int64, gs::TypeId::UniversalType));

    auto floatToUniversalType = storage.GetConvertFunc(gs::TypeId::Float, gs::TypeId::UniversalType);
    ASSERT_FLOAT_EQ(1.f,
        std::get<float>(cpgf::fromVariant<gs::UniversalType>(floatToUniversalType(1.f))));

    auto vector4fToUniversalType = storage.GetConvertFunc(gs::TypeId::Vector4f, gs::TypeId::UniversalType);
    ASSERT_EQ(Eigen::Vector4f(1.f, 2.f, 3.f, 4.f),
        std::get<Eigen::Vector4f>(cpgf::fromVariant<gs::UniversalType>(vector4fToUniversalType(Eigen::Vector4f(1.f, 2.f, 3.f, 4.f)))));

    auto generatorValue = math::Generator2D(2.f);
    auto generator2dToUniversalType = storage.GetConvertFunc(gs::TypeId::Generator2d, gs::TypeId::UniversalType);
    ASSERT_DOUBLE_EQ(2.,
        std::get<math::Generator2D>(cpgf::fromVariant<gs::UniversalType>(generator2dToUniversalType(generatorValue)))(1.1, 2.2));

    auto universalFloatToUniversalType = storage.GetConvertFunc(gs::TypeId::UniversalFloat, gs::TypeId::UniversalType);
    ASSERT_FLOAT_EQ(3.f,
        std::get<float>(cpgf::fromVariant<gs::UniversalType>(universalFloatToUniversalType(gs::UniversalType(3.f)))));

    auto universalVector4fValue = gs::UniversalType(Eigen::Vector4f(1.f, 2.f, 3.f, 4.f));
    auto universalVector4fToUniversalType = storage.GetConvertFunc(gs::TypeId::UniversalVector4f, gs::TypeId::UniversalType);
    ASSERT_EQ(Eigen::Vector4f(1.f, 2.f, 3.f, 4.f),
        std::get<Eigen::Vector4f>(cpgf::fromVariant<gs::UniversalType>(universalVector4fToUniversalType(universalVector4fValue))));

    auto universalGeneratorValue = gs::UniversalType(math::Generator2D(2.f));
    auto universalGenerator2dToUniversalType = storage.GetConvertFunc(gs::TypeId::UniversalGenerator2d, gs::TypeId::UniversalType);
    ASSERT_DOUBLE_EQ(2.,
        std::get<math::Generator2D>(cpgf::fromVariant<gs::UniversalType>(universalGenerator2dToUniversalType(universalGeneratorValue)))(1.1, 2.2));
}

}
