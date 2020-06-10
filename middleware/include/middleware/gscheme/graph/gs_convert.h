#pragma once

#include <type_traits>

#include "eigen/core.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_universal_type.h"


namespace gs {

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector<T>>>
    float ToFloat(const T value) {
    if constexpr (IsFloat<T>) {
        return value;
    } else if constexpr (IsVector<T>) {
        return value[0];
    }
}

template <typename T>
    using ToFloatT = decltype(ToFloat<T>);

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector<T>>>
    Eigen::Vector2f ToVector2f(const T value) {
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
    Eigen::Vector3f ToVector3f(const T value) {
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
    Eigen::Vector4f ToVector4f(const T value) {
    if constexpr (IsFloat<T>) {
        return Eigen::Vector4f(value, value, value, value);
    } else if constexpr (IsVector4f<T>) {
        return value;
    }
}

template <typename T>
    using ToVector4fT = decltype(ToVector4f<T>);

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector<T> || IsGenerator2D<T>>>
    math::Generator2D ToGenerator2D(const T value) {
    if constexpr (IsGenerator2D<T>) {
        return value;
    } else if constexpr (IsFloat<T> || IsVector<T>) {
        return math::Generator2D(value);
    }
}

template <typename T>
    using ToGenerator2DT = decltype(ToGenerator2D<T>);

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector<T> || IsGenerator3D<T>>>
    math::Generator3D ToGenerator3D(const T value) {
    if constexpr (IsGenerator3D<T>) {
        return value;
    } else if constexpr (IsFloat<T> || IsVector<T>) {
        return math::Generator3D(value);
    }
}

template <typename T>
    using ToGenerator3DT = decltype(ToGenerator3D<T>);

template<typename T, typename Enable = std::enable_if_t<IsFloat<T> || IsVector<T> || IsGenerator2D<T> || IsGenerator3D<T> || IsUniversalType<T>>>
    UniversalType ToUniversalType(const T value) {
    if constexpr (IsUniversalType<T>) {
        return value;
    } else if constexpr (IsFloat<T> || IsVector<T> || IsGenerator2D<T> || IsGenerator3D<T>) {
        return UniversalType(value);
    }
}

template <typename T>
    using ToUniversalTypeT = decltype(ToUniversalType<T>);

template <typename To, typename From, typename Enable = std::enable_if_t<IsFloat<To> || IsVector<To> || IsGenerator2D<To> || IsGenerator3D<To> || IsUniversalType<To>>>
    To ConvertTo(const From value) {
    if constexpr (IsUniversalType<From>) {
        return std::visit([](auto&& v) {
            using valueType = std::remove_cvref_t<decltype(v)>;
            return ConvertTo<To, valueType>(v);
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

template <typename To, typename From>
    constexpr bool IsCanConvert() {
        if constexpr (IsUniversalType<From>) {
            // we can't known variant type
            return false;
        } else if constexpr (IsFloat<To>) {
            return meta::Detect<From, ToFloatT>::value;
        } else if constexpr (IsVector2f<To>) {
            return meta::Detect<From, ToVector2fT>::value;
        } else if constexpr (IsVector3f<To>) {
            return meta::Detect<From, ToVector3fT>::value;
        } else if constexpr (IsVector4f<To>) {
            return meta::Detect<From, ToVector4fT>::value;
        } else if constexpr (IsGenerator2D<To>) {
            return meta::Detect<From, ToGenerator2DT>::value;
        } else if constexpr (IsGenerator3D<To>) {
            return meta::Detect<From, ToGenerator3DT>::value;
        } else if constexpr (IsUniversalType<To>) {
            return meta::Detect<From, ToUniversalTypeT>::value;
        } else {
            return false;
        }
    }

template <typename To>
    constexpr bool IsCanConvertUniversalType(const gs::UniversalType& value) {
        return std::visit([](auto&& v) {
            using valueType = std::remove_cvref_t<decltype(v)>;
            return IsCanConvert<To, valueType>();
        }, value);
    }

template <typename To, typename From>
    inline constexpr bool CanConvert = IsCanConvert<To, From>();

}
