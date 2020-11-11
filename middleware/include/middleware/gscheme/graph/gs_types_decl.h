#pragma once

#include <cstdint>

namespace gs {

enum class TypeId : uint8_t {
    Unknown = 0,
    Float = 1,
    Vector2f = 2,
    Vector3f = 3,
    Vector4f = 4,
    Generator2d = 5,
    Generator3d = 6,
    UniversalType = 128,
    UniversalFloat = UniversalType + Float,
    UniversalVector2f = UniversalType + Vector2f,
    UniversalVector3f = UniversalType + Vector3f,
    UniversalVector4f = UniversalType + Vector4f,
    UniversalGenerator2d = UniversalType + Generator2d,
    UniversalGenerator3d = UniversalType + Generator3d,
};

}
