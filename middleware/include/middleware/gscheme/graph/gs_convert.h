#pragma once

#include <type_traits>

#include "rttr/rttr.h"
#include "eigen/core.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_universal_type.h"


namespace gs {

template <typename T> inline constexpr bool IsFloat = std::is_same_v<T, float>;
template <typename T> inline constexpr bool IsVector2f = std::is_same_v<T, Eigen::Vector2f>;
template <typename T> inline constexpr bool IsVector3f = std::is_same_v<T, Eigen::Vector3f>;
template <typename T> inline constexpr bool IsVector4f = std::is_same_v<T, Eigen::Vector4f>;
template <typename T> inline constexpr bool IsVector = IsVector2f<T> || IsVector3f<T> || IsVector4f<T>;
template <typename T> inline constexpr bool IsGenerator2D = std::is_same_v<T, math::Generator2D>;
template <typename T> inline constexpr bool IsGenerator3D = std::is_same_v<T, math::Generator3D>;
template <typename T> inline constexpr bool IsUniversalType = std::is_same_v<T, UniversalType>;


template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector<T>>>
    float ToFloat(const T value) {
    if constexpr (IsFloat<T>) {
        return value;
    } else if constexpr (IsVector<T>) {
        return value[0];
    }
}

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector<T>>>
    Eigen::Vector2f ToVector2f(const T value) {
    if constexpr (IsFloat<T>) {
        return Eigen::Vector2f(value, value);
    } else if constexpr (IsVector2f<T>) {
        return value;
    } else if constexpr (IsVector3f<T> || IsVector4f<T>) {
        return value.head(2);
    }
}

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector3f<T> || IsVector4f<T>>>
    Eigen::Vector3f ToVector3f(const T value) {
    if constexpr (IsFloat<T>) {
        return Eigen::Vector3f(value, value, value);
    } else if constexpr (IsVector3f<T>) {
        return value;
    } else if constexpr (IsVector4f<T>) {
        return value.head(3);
    }
}

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector4f<T>>>
    Eigen::Vector4f ToVector4f(const T value) {
    if constexpr (IsFloat<T>) {
        return Eigen::Vector3f(value, value, value);
    } else if constexpr (IsVector4f<T>) {
        return value;
    }
}

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector<T> || IsGenerator2D<T>>>
    math::Generator2D ToGenerator2D(const T value) {
    if constexpr (IsGenerator2D<T>) {
        return value;
    } else if constexpr (IsFloat<T> || IsVector<T>) {
        return math::Generator2D(value);
    }
}

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector<T> || IsGenerator3D<T>>>
    math::Generator3D ToGenerator3D(const T value) {
    if constexpr (IsGenerator3D<T>) {
        return value;
    } else if constexpr (IsFloat<T> || IsVector<T>) {
        return math::Generator3D(value);
    }
}

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector<T> || IsGenerator2D<T> || IsGenerator3D<T> || IsUniversalType<T>>>
    UniversalType ToGenerator3D(const T value) {
    if constexpr (IsUniversalType<T>) {
        return value;
    } else if constexpr (IsFloat<T> || IsVector<T> || IsGenerator2D<T> || IsGenerator3D<T>) {
        return UniversalType(value);
    }
}

template <typename To, typename From, typename Enable = std::enable_if_t<IsFloat<To> || IsVector<To> || IsGenerator2D<To> || IsGenerator3D<To> || IsUniversalType<To>>>
    To ConvertTo(const From value) {
    if constexpr (IsUniversalType<From>) {
        return std::visit([](auto&& v) {
            using valueType = std::remove_cvref_t<decltype(v)>;
            return ConvertTo<valueType, To>(v);
        }, value);
    } else if constexpr (IsFloat<To>) {
        return ToFloat(value);
    } else if constexpr (IsVector2f<To>) {
        return ToVector2f(value);
    } else if constexpr (IsVector3f<To>) {
        return ToVector3f(value);
    } else if constexpr (IsVector4f<To>) {
        return ToVector4f(value);
    } else if constexpr (IsGenerator2D<To>) {
        return ToGenerator2D(value);
    } else if constexpr (IsGenerator3D<To>) {
        return ToGenerator3D(value);
    } else if constexpr (IsUniversalType<To>) {
        return ToUniversalType(value);
    }
}

}
