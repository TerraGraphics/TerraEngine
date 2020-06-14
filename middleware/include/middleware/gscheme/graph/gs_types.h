#pragma once

#include <string>
#include <cstdint>
#include <variant>
#include <type_traits>

#include "eigen/core.h"
#include "core/math/generator_type.h"


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
    Generator2D = 14,
    Generator3D = 15,
    String = 16,
    UniversalType = 128,
    UniversalFloat = UniversalType + Float,
    UniversalVector2f = UniversalType + Vector2f,
    UniversalVector3f = UniversalType + Vector3f,
    UniversalVector4f = UniversalType + Vector4f,
    UniversalGenerator2D = UniversalType + Generator2D,
    UniversalGenerator3D = UniversalType + Generator3D,
};

TypeId MaxTypeId(TypeId a, TypeId b);

using UniversalType = std::variant<float, Eigen::Vector2f, Eigen::Vector3f, Eigen::Vector4f, math::Generator2D, math::Generator3D>;

#define UNIVERSAL_TYPES \
    (float, (Eigen::Vector2f, (Eigen::Vector3f, (Eigen::Vector4f, (math::Generator2D, (math::Generator3D, (gs::UniversalType, BOOST_PP_NIL)))))))

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
template <typename T> inline constexpr bool IsGenerator2D = std::is_same_v<T, math::Generator2D>;
template <typename T> inline constexpr bool IsGenerator3D = std::is_same_v<T, math::Generator3D>;
template <typename T> inline constexpr bool IsString = std::is_same_v<T, std::string>;
template <typename T> inline constexpr bool IsUniversalType = std::is_same_v<T, UniversalType>;

template <typename T> inline constexpr bool IsVector = IsVector2f<T> || IsVector3f<T> || IsVector4f<T>;
template <typename T> inline constexpr bool IsGenerator = IsGenerator2D<T> || IsGenerator3D<T>;
template <typename T> inline constexpr bool IsUniversalTypeCompatible = IsFloat<T> || IsVector<T> || IsGenerator<T> || IsUniversalType<T>;
template <typename T> inline constexpr bool IsGsType = IsInt8<T> || IsUint8<T> || IsInt16<T> || IsUint16<T> || IsInt32<T> || IsUint32<T> ||
    IsInt64<T> || IsUint64<T> || IsDouble<T> || IsString<T> || IsUniversalTypeCompatible<T>;


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
    } else if constexpr (IsGenerator2D<T>) {
        return TypeId::Generator2D;
    } else if constexpr (IsGenerator3D<T>) {
        return TypeId::Generator3D;
    } else if constexpr (IsUniversalType<T>) {
        return TypeId::UniversalType;
    }
}

uintptr_t RttrTypeIdInt8();
uintptr_t RttrTypeIdUint8();
uintptr_t RttrTypeIdInt16();
uintptr_t RttrTypeIdUint16();
uintptr_t RttrTypeIdInt32();
uintptr_t RttrTypeIdUint32();
uintptr_t RttrTypeIdInt64();
uintptr_t RttrTypeIdUint64();
uintptr_t RttrTypeIdFloat();
uintptr_t RttrTypeIdDouble();
uintptr_t RttrTypeIdVector2f();
uintptr_t RttrTypeIdVector3f();
uintptr_t RttrTypeIdVector4f();
uintptr_t RttrTypeIdGenerator2D();
uintptr_t RttrTypeIdGenerator3D();
uintptr_t RttrTypeIdString();
uintptr_t RttrTypeIdUniversalType();

bool IsValidEmbeddedPinRttrTypeId(uintptr_t typeId);
bool IsValidInputPinRttrTypeId(uintptr_t typeId);
bool IsValidOutputPinRttrTypeId(uintptr_t typeId);

}
