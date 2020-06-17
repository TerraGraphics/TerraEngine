#include "middleware/gscheme/graph/gs_types.h"

#include <utility>
#include <typeindex>
#include <algorithm>
#include <unordered_map>

#include "core/common/hash.h"


namespace gs {

TypeId MaxTypeId(TypeId a, TypeId b) {
    constexpr const uint8_t mask = (static_cast<uint8_t>(TypeId::UniversalType) - uint8_t(1));
    uint8_t aVal = static_cast<uint8_t>(a) & mask;
    uint8_t bVal = static_cast<uint8_t>(b) & mask;

    return static_cast<TypeId>((aVal > bVal) ? aVal : bVal);
}

TypeId GetTypeId(const std::type_info& typeInfo) {
    static const std::unordered_map<std::type_index, TypeId> typeIndexToId = {
        { std::type_index(typeid(int8_t)), TypeId::Int8 },
        { std::type_index(typeid(uint8_t)), TypeId::Uint8 },
        { std::type_index(typeid(int16_t)), TypeId::Int16 },
        { std::type_index(typeid(uint16_t)), TypeId::Uint16 },
        { std::type_index(typeid(int32_t)), TypeId::Int32 },
        { std::type_index(typeid(uint32_t)), TypeId::Uint32 },
        { std::type_index(typeid(int64_t)), TypeId::Int64 },
        { std::type_index(typeid(uint64_t)), TypeId::Uint64 },
        { std::type_index(typeid(float)), TypeId::Float },
        { std::type_index(typeid(double)), TypeId::Double },
        { std::type_index(typeid(std::string)), TypeId::String },
        { std::type_index(typeid(Eigen::Vector2f)), TypeId::Vector2f },
        { std::type_index(typeid(Eigen::Vector3f)), TypeId::Vector3f },
        { std::type_index(typeid(Eigen::Vector4f)), TypeId::Vector4f },
        { std::type_index(typeid(math::Generator2D)), TypeId::Generator2D },
        { std::type_index(typeid(math::Generator3D)), TypeId::Generator3D },
        { std::type_index(typeid(UniversalType)), TypeId::UniversalType },
    };

    if (const auto it = typeIndexToId.find(std::type_index(typeInfo)); it != typeIndexToId.cend()) {
        return it->second;
    }

    return TypeId::Unknown;
}

bool IsValidEmbeddedPinType(const std::type_info& typeInfo) {
    auto typeId = GetTypeId(typeInfo);

    return (
        (typeId == TypeId::Int8) ||
        (typeId == TypeId::Uint8) ||
        (typeId == TypeId::Int16) ||
        (typeId == TypeId::Uint16) ||
        (typeId == TypeId::Int32) ||
        (typeId == TypeId::Uint32) ||
        (typeId == TypeId::Int64) ||
        (typeId == TypeId::Uint64) ||
        (typeId == TypeId::Float) ||
        (typeId == TypeId::Double) ||
        (typeId == TypeId::Vector2f) ||
        (typeId == TypeId::Vector3f) ||
        (typeId == TypeId::Vector4f) ||
        (typeId == TypeId::String)
    );
}

bool IsValidInputPinType(const std::type_info& typeInfo) {
    auto typeId = GetTypeId(typeInfo);
    return (
        (typeId == TypeId::Float) ||
        (typeId == TypeId::Vector2f) ||
        (typeId == TypeId::Vector3f) ||
        (typeId == TypeId::Vector4f) ||
        (typeId == TypeId::Generator2D) ||
        (typeId == TypeId::Generator3D) ||
        (typeId == TypeId::UniversalType)
    );
}

bool IsValidOutputPinType(const std::type_info& typeInfo) {
    auto typeId = GetTypeId(typeInfo);

    return (
        (typeId == TypeId::Float) ||
        (typeId == TypeId::Vector2f) ||
        (typeId == TypeId::Vector3f) ||
        (typeId == TypeId::Vector4f) ||
        (typeId == TypeId::Generator2D) ||
        (typeId == TypeId::Generator3D) ||
        (typeId == TypeId::UniversalType)
    );
}

}
