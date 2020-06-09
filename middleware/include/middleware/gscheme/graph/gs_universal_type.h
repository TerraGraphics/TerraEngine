#pragma once

#include <variant>
#include <type_traits>

#include "rttr/rttr.h"
#include "eigen/core.h"
#include "core/math/generator_type.h"


namespace gs {

using UniversalType = std::variant<float, Eigen::Vector2f, Eigen::Vector3f, Eigen::Vector4f, math::Generator2D, math::Generator3D>;

template <typename T> inline constexpr bool IsFloat = std::is_same_v<T, float>;
template <typename T> inline constexpr bool IsVector2f = std::is_same_v<T, Eigen::Vector2f>;
template <typename T> inline constexpr bool IsVector3f = std::is_same_v<T, Eigen::Vector3f>;
template <typename T> inline constexpr bool IsVector4f = std::is_same_v<T, Eigen::Vector4f>;
template <typename T> inline constexpr bool IsVector = IsVector2f<T> || IsVector3f<T> || IsVector4f<T>;
template <typename T> inline constexpr bool IsGenerator2D = std::is_same_v<T, math::Generator2D>;
template <typename T> inline constexpr bool IsGenerator3D = std::is_same_v<T, math::Generator3D>;
template <typename T> inline constexpr bool IsUniversalType = std::is_same_v<T, UniversalType>;

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
