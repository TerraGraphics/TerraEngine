#pragma once

#include "middleware/gscheme/graph/gs_types.h"


namespace gs {

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector<T> || IsGenerator2D<T> || IsGenerator3D<T>>>
constexpr uint8_t UniversalTypesGetId() {
    if constexpr (IsFloat<T>) {
        return 0;
    } else if constexpr (IsVector2f<T>) {
        return 1;
    } else if constexpr (IsVector3f<T>) {
        return 2;
    } else if constexpr (IsVector4f<T>) {
        return 3;
    } else if constexpr (IsGenerator2D<T>) {
        return 4;
    } else if constexpr (IsGenerator3D<T>) {
        return 5;
    }
}

}
