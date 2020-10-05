#pragma once

#include <cstdint>

namespace gs {

enum class TypeId : uint8_t {
    Unknown = 0,
    Int8 = 1,
    Uint8 = 2,
    Int16 = 3,
    Uint16 = 4,
    Int32 = 5,
    Uint32 = 6,
    Int64 = 7,
    Uint64 = 8,
    Float = 9,
    Double = 10,
    Vector2f = 11,
    Vector3f = 12,
    Vector4f = 13,
    Generator2d = 14,
    Generator3d = 15,
    String = 16,
    UniversalType = 128,
    UniversalFloat = UniversalType + Float,
    UniversalVector2f = UniversalType + Vector2f,
    UniversalVector3f = UniversalType + Vector3f,
    UniversalVector4f = UniversalType + Vector4f,
    UniversalGenerator2d = UniversalType + Generator2d,
    UniversalGenerator3d = UniversalType + Generator3d,
};

}
