#include "test/test.h"
#include "middleware/gscheme/graph/gs_convert_storage.h"


#define ASSERT_CONVERT_TRUE(From, To) ASSERT_TRUE(storage.CanConvert(gs::TypeId::From, gs::TypeId::To))
#define ASSERT_CONVERT_FALSE(From, To) ASSERT_FALSE(storage.CanConvert(gs::TypeId::From, gs::TypeId::To))

namespace {

TEST(GSConvertStorage, ToUint64) {
    gs::ConvertStorage storage;

    ASSERT_CONVERT_FALSE(Uint8, Uint64);
    ASSERT_CONVERT_FALSE(Int16, Uint64);
    ASSERT_CONVERT_FALSE(Uint32, Uint64);
    ASSERT_CONVERT_FALSE(Int64, Uint64);
    ASSERT_CONVERT_FALSE(Uint64, Uint64);
    ASSERT_CONVERT_FALSE(Float, Uint64);
    ASSERT_CONVERT_FALSE(Vector2f, Uint64);
    ASSERT_CONVERT_FALSE(Generator2D, Uint64);

    ASSERT_CONVERT_FALSE(UniversalFloat, Uint64);
    ASSERT_CONVERT_FALSE(UniversalVector2f, Uint64);
    ASSERT_CONVERT_FALSE(UniversalGenerator2D, Uint64);

    ASSERT_CONVERT_FALSE(UniversalType, Uint64);
}

TEST(GSConvertStorage, ToFloat) {
    gs::ConvertStorage storage;

    ASSERT_CONVERT_FALSE(Uint8, Float);
    ASSERT_CONVERT_TRUE(Float, Float);
    ASSERT_CONVERT_TRUE(Vector2f, Float);
    ASSERT_CONVERT_FALSE(Generator2D, Float);

    ASSERT_CONVERT_TRUE(UniversalFloat, Float);
    ASSERT_CONVERT_TRUE(UniversalVector2f, Float);
    ASSERT_CONVERT_FALSE(UniversalGenerator2D, Float);

    ASSERT_CONVERT_FALSE(UniversalType, Float);
}

TEST(GSConvertStorage, ToVector3f) {
    gs::ConvertStorage storage;

    ASSERT_CONVERT_FALSE(Uint16, Vector3f);
    ASSERT_CONVERT_TRUE(Float, Vector3f);
    ASSERT_CONVERT_FALSE(Vector2f, Vector3f);
    ASSERT_CONVERT_TRUE(Vector3f, Vector3f);
    ASSERT_CONVERT_FALSE(Generator2D, Vector3f);

    ASSERT_CONVERT_TRUE(UniversalFloat, Vector3f);
    ASSERT_CONVERT_FALSE(UniversalVector2f, Vector3f);
    ASSERT_CONVERT_TRUE(UniversalVector3f, Vector3f);
    ASSERT_CONVERT_FALSE(UniversalGenerator2D, Vector3f);

    ASSERT_CONVERT_FALSE(UniversalType, Vector3f);
}

TEST(GSConvertStorage, ToGenerator3D) {
    gs::ConvertStorage storage;

    ASSERT_CONVERT_FALSE(Int32, Generator3D);
    ASSERT_CONVERT_TRUE(Float, Generator3D);
    ASSERT_CONVERT_TRUE(Vector4f, Generator3D);
    ASSERT_CONVERT_FALSE(Generator2D, Generator3D);
    ASSERT_CONVERT_TRUE(Generator3D, Generator3D);

    ASSERT_CONVERT_TRUE(UniversalFloat, Generator3D);
    ASSERT_CONVERT_TRUE(UniversalVector4f, Generator3D);
    ASSERT_CONVERT_FALSE(UniversalGenerator2D, Generator3D);
    ASSERT_CONVERT_TRUE(UniversalGenerator3D, Generator3D);

    ASSERT_CONVERT_FALSE(UniversalType, Generator3D);
}

TEST(GSConvertStorage, ToUniversalType) {
    gs::ConvertStorage storage;

    ASSERT_CONVERT_FALSE(Int64, UniversalType);
    ASSERT_CONVERT_TRUE(Float, UniversalType);
    ASSERT_CONVERT_TRUE(Vector4f, UniversalType);
    ASSERT_CONVERT_TRUE(Generator2D, UniversalType);

    ASSERT_CONVERT_FALSE(UniversalFloat, UniversalType);
    ASSERT_CONVERT_FALSE(UniversalVector4f, UniversalType);
    ASSERT_CONVERT_FALSE(UniversalGenerator2D, UniversalType);

    ASSERT_CONVERT_TRUE(UniversalType, UniversalType);
}

}
