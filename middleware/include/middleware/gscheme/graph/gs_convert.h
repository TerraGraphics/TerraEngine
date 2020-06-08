#pragma once

#include <type_traits>

#include "rttr/rttr.h"
#include "eigen/core.h"


namespace gs {

template <typename T> inline constexpr bool IsFloat = std::is_same_v<T, float>;
template <typename T> inline constexpr bool IsVector2f = std::is_same_v<T, Eigen::Vector2f>;
template <typename T> inline constexpr bool IsVector3f = std::is_same_v<T, Eigen::Vector3f>;
template <typename T> inline constexpr bool IsVector4f = std::is_same_v<T, Eigen::Vector4f>;

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector2f<T> || IsVector3f<T> || IsVector4f<T>>>
    float ToFloat(const T value) {
    if constexpr (IsFloat<T>) {
        return value;
    } else if constexpr (IsVector2f<T> || IsVector3f<T> || IsVector4f<T>) {
        return value[0];
    }
}

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector2f<T> || IsVector3f<T> || IsVector4f<T>>>
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

template <typename To, typename From> To ConvertTo(const From& value) {
    if constexpr (IsFloat<To>) {
        return ToFloat(value);
    } else if constexpr (IsVector2f<To>) {
        return ToVector2f(value);
    } else if constexpr (IsVector3f<To>) {
        return ToVector3f(value);
    } else if constexpr (IsVector4f<To>) {
        return ToVector4f(value);
    }
}

}
