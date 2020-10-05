#pragma once

#include <type_traits>

#include "eigen/core.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_types.h"


namespace gs {

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector<T>>>
    float ToFloat(const T& value) {
    if constexpr (IsFloat<T>) {
        return value;
    } else if constexpr (IsVector<T>) {
        return value[0];
    }
}

template <typename T>
    using ToFloatT = decltype(ToFloat<T>);

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector<T>>>
    Eigen::Vector2f ToVector2f(const T& value) {
    if constexpr (IsFloat<T>) {
        return Eigen::Vector2f(value, value);
    } else if constexpr (IsVector2f<T>) {
        return value;
    } else if constexpr (IsVector3f<T> || IsVector4f<T>) {
        return Eigen::Vector2f(value[0], value[1]);
    }
}

template <typename T>
    using ToVector2fT = decltype(ToVector2f<T>);

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector3f<T> || IsVector4f<T>>>
    Eigen::Vector3f ToVector3f(const T& value) {
    if constexpr (IsFloat<T>) {
        return Eigen::Vector3f(value, value, value);
    } else if constexpr (IsVector3f<T>) {
        return value;
    } else if constexpr (IsVector4f<T>) {
        return Eigen::Vector3f(value[0], value[1], value[2]);
    }
}

template <typename T>
    using ToVector3fT = decltype(ToVector3f<T>);

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector4f<T>>>
    Eigen::Vector4f ToVector4f(const T& value) {
    if constexpr (IsFloat<T>) {
        return Eigen::Vector4f(value, value, value, value);
    } else if constexpr (IsVector4f<T>) {
        return value;
    }
}

template <typename T>
    using ToVector4fT = decltype(ToVector4f<T>);

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector<T> || IsGenerator2d<T>>>
    math::Generator2d ToGenerator2d(const T& value) {
    if constexpr (IsGenerator2d<T>) {
        return value;
    } else if constexpr (IsFloat<T> || IsVector<T>) {
        return math::Generator2d(value);
    }
}

template <typename T>
    using ToGenerator2dT = decltype(ToGenerator2d<T>);

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector<T> || IsGenerator3d<T>>>
    math::Generator3d ToGenerator3d(const T& value) {
    if constexpr (IsGenerator3d<T>) {
        return value;
    } else if constexpr (IsFloat<T> || IsVector<T>) {
        return math::Generator3d(value);
    }
}

template <typename T>
    using ToGenerator3dT = decltype(ToGenerator3d<T>);

// Works only for basic types, not including UniversalType
template <typename To, typename From, typename Enable = std::enable_if_t<IsFloat<To> || IsVector<To> || IsGenerator2d<To> || IsGenerator3d<To>>>
    To ConvertTo(const From& value) {
    if constexpr (IsFloat<To>) {
        return ToFloat(value);
    } else if constexpr (IsVector2f<To>) {
        return ToVector2f(value);
    } else if constexpr (IsVector3f<To>) {
        return ToVector3f(value);
    } else if constexpr (IsVector4f<To>) {
        return ToVector4f(value);
    } else if constexpr (IsGenerator2d<To>) {
        return ToGenerator2d(value);
    } else if constexpr (IsGenerator3d<To>) {
        return ToGenerator3d(value);
    }
}

// Works only for basic types, not including UniversalType
template <typename To, typename From>
    constexpr bool IsCanConvert() {
        if constexpr (IsFloat<To>) {
            return meta::Detect<From, ToFloatT>::value;
        } else if constexpr (IsVector2f<To>) {
            return meta::Detect<From, ToVector2fT>::value;
        } else if constexpr (IsVector3f<To>) {
            return meta::Detect<From, ToVector3fT>::value;
        } else if constexpr (IsVector4f<To>) {
            return meta::Detect<From, ToVector4fT>::value;
        } else if constexpr (IsGenerator2d<To>) {
            return meta::Detect<From, ToGenerator2dT>::value;
        } else if constexpr (IsGenerator3d<To>) {
            return meta::Detect<From, ToGenerator3dT>::value;
        } else {
            return false;
        }
    }

// Works only for basic types, not including UniversalType
template <typename To, typename From>
    inline constexpr bool CanConvert = IsCanConvert<To, From>();

}
