#pragma once

#include <string>
#include <cstdint>
#include <variant>
#include <typeinfo>
#include <type_traits>

#include "eigen/core.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_types_decl.h"


#define UNIVERSAL_TYPES \
    (float, (Eigen::Vector2f, (Eigen::Vector3f, (Eigen::Vector4f, (math::Generator2d, (math::Generator3d, BOOST_PP_NIL))))))

namespace gs {

TypeId MaxTypeId(TypeId a, TypeId b);

constexpr inline TypeId ToUniversalTypeId(TypeId id) {
    return static_cast<TypeId>(static_cast<uint8_t>(id) | static_cast<uint8_t>(TypeId::UniversalType));
}
constexpr inline TypeId ToBaseTypeId(TypeId id) {
    return static_cast<TypeId>(static_cast<uint8_t>(id) & ~static_cast<uint8_t>(TypeId::UniversalType));
}
constexpr inline bool HasUniversalBit(TypeId id) {
    return ((static_cast<uint8_t>(id) & static_cast<uint8_t>(TypeId::UniversalType)) != 0);
}
constexpr inline bool IsConcreteUniversalType(TypeId id) {
    return (((static_cast<uint8_t>(id) & static_cast<uint8_t>(TypeId::UniversalType)) != 0) && (id != TypeId::UniversalType));
}

using UniversalType = std::variant<float, Eigen::Vector2f, Eigen::Vector3f, Eigen::Vector4f, math::Generator2d, math::Generator3d>;

template <typename T> inline constexpr bool IsInt8 = std::is_same_v<T, int8_t>;
template <typename T> inline constexpr bool IsUint8 = std::is_same_v<T, uint8_t>;
template <typename T> inline constexpr bool IsInt16 = std::is_same_v<T, int16_t>;
template <typename T> inline constexpr bool IsUint16 = std::is_same_v<T, uint16_t>;
template <typename T> inline constexpr bool IsInt32 = std::is_same_v<T, int32_t>;
template <typename T> inline constexpr bool IsUint32 = std::is_same_v<T, uint32_t>;
template <typename T> inline constexpr bool IsInt64 = std::is_same_v<T, int64_t>;
template <typename T> inline constexpr bool IsUint64 = std::is_same_v<T, uint64_t>;
template <typename T> inline constexpr bool IsFloat = std::is_same_v<T, float>;
template <typename T> inline constexpr bool IsDouble = std::is_same_v<T, double>;
template <typename T> inline constexpr bool IsVector2f = std::is_same_v<T, Eigen::Vector2f>;
template <typename T> inline constexpr bool IsVector3f = std::is_same_v<T, Eigen::Vector3f>;
template <typename T> inline constexpr bool IsVector4f = std::is_same_v<T, Eigen::Vector4f>;
template <typename T> inline constexpr bool IsGenerator2d = std::is_same_v<T, math::Generator2d>;
template <typename T> inline constexpr bool IsGenerator3d = std::is_same_v<T, math::Generator3d>;
template <typename T> inline constexpr bool IsString = std::is_same_v<T, std::string>;
template <typename T> inline constexpr bool IsUniversalType = std::is_same_v<T, UniversalType>;

template <typename T> inline constexpr bool IsStd = IsInt8<T> || IsUint8<T> || IsInt16<T> || IsUint16<T> ||
    IsInt32<T> || IsUint32<T> || IsInt64<T> || IsUint64<T> || IsFloat<T> || IsDouble<T> || IsString<T>;
template <typename T> inline constexpr bool IsVector = IsVector2f<T> || IsVector3f<T> || IsVector4f<T>;
template <typename T> inline constexpr bool IsGenerator = IsGenerator2d<T> || IsGenerator3d<T>;
template <typename T> inline constexpr bool IsGsType = IsStd<T> || IsVector<T> || IsGenerator<T> || IsUniversalType<T>;

template <typename T> inline constexpr bool IsEmbedded = IsStd<T> || IsVector<T>;
template <typename T> inline constexpr bool IsInput = IsFloat<T> || IsVector<T> || IsGenerator<T>;

template<typename T, typename Enable = std::enable_if_t<IsGsType<T>>>
    constexpr TypeId GetTypeId() {
    if constexpr (IsInt8<T>) {
        return TypeId::Int8;
    } else if constexpr (IsUint8<T>) {
        return TypeId::Uint8;
    } else if constexpr (IsInt16<T>) {
        return TypeId::Int16;
    } else if constexpr (IsUint16<T>) {
        return TypeId::Uint16;
    } else if constexpr (IsInt32<T>) {
        return TypeId::Int32;
    } else if constexpr (IsUint32<T>) {
        return TypeId::Uint32;
    } else if constexpr (IsInt64<T>) {
        return TypeId::Int64;
    } else if constexpr (IsUint64<T>) {
        return TypeId::Uint64;
    } else if constexpr (IsFloat<T>) {
        return TypeId::Float;
    } else if constexpr (IsDouble<T>) {
        return TypeId::Double;
    } else if constexpr (IsString<T>) {
        return TypeId::String;
    } else if constexpr (IsVector2f<T>) {
        return TypeId::Vector2f;
    } else if constexpr (IsVector3f<T>) {
        return TypeId::Vector3f;
    } else if constexpr (IsVector4f<T>) {
        return TypeId::Vector4f;
    } else if constexpr (IsGenerator2d<T>) {
        return TypeId::Generator2d;
    } else if constexpr (IsGenerator3d<T>) {
        return TypeId::Generator3d;
    } else if constexpr (IsUniversalType<T>) {
        return TypeId::UniversalType;
    }
}

TypeId GetTypeId(const std::type_info& typeInfo);

bool IsValidEmbeddedPinType(const std::type_info& typeInfo);
bool IsValidInputPinType(const std::type_info& typeInfo);
bool IsValidOutputPinType(const std::type_info& typeInfo);

}
