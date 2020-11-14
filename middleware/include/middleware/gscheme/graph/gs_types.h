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
    (float, (Eigen::Vector2f, (Eigen::Vector3f, (Eigen::Vector4f, (math::Generator2D, (math::Generator3D, BOOST_PP_NIL))))))

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

using UniversalType = std::variant<float, Eigen::Vector2f, Eigen::Vector3f, Eigen::Vector4f, math::Generator2D, math::Generator3D>;

template <typename T> inline constexpr bool IsFloat = std::is_same_v<T, float>;
template <typename T> inline constexpr bool IsVector2f = std::is_same_v<T, Eigen::Vector2f>;
template <typename T> inline constexpr bool IsVector3f = std::is_same_v<T, Eigen::Vector3f>;
template <typename T> inline constexpr bool IsVector4f = std::is_same_v<T, Eigen::Vector4f>;
template <typename T> inline constexpr bool IsGenerator2d = std::is_same_v<T, math::Generator2D>;
template <typename T> inline constexpr bool IsGenerator3d = std::is_same_v<T, math::Generator3D>;
template <typename T> inline constexpr bool IsUniversalType = std::is_same_v<T, UniversalType>;

template <typename T> inline constexpr bool IsVector = IsVector2f<T> || IsVector3f<T> || IsVector4f<T>;
template <typename T> inline constexpr bool IsGenerator = IsGenerator2d<T> || IsGenerator3d<T>;
template <typename T> inline constexpr bool IsInput = IsFloat<T> || IsVector<T> || IsGenerator<T>;
template <typename T> inline constexpr bool IsGsType = IsInput<T> || IsUniversalType<T>;

template<typename T, typename Enable = std::enable_if_t<IsGsType<T>>>
    constexpr TypeId GetTypeId() {
    if constexpr (IsFloat<T>) {
        return TypeId::Float;
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
TypeId GetUniversalTypeId(const UniversalType& v);

bool IsValidInputPinType(const std::type_info& typeInfo);
bool IsValidOutputPinType(const std::type_info& typeInfo);
bool IsEnableUI(TypeId typeId);

}
