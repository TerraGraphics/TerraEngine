#include "middleware/gschema/graph/gs_types.h"

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
        { std::type_index(typeid(float)), TypeId::Float },
        { std::type_index(typeid(Eigen::Vector2f)), TypeId::Vector2f },
        { std::type_index(typeid(Eigen::Vector3f)), TypeId::Vector3f },
        { std::type_index(typeid(Eigen::Vector4f)), TypeId::Vector4f },
        { std::type_index(typeid(math::Generator2D)), TypeId::Generator2d },
        { std::type_index(typeid(math::Generator3D)), TypeId::Generator3d },
        { std::type_index(typeid(UniversalType)), TypeId::UniversalType },
    };

    if (const auto it = typeIndexToId.find(std::type_index(typeInfo)); it != typeIndexToId.cend()) {
        return it->second;
    }

    return TypeId::Unknown;
}

TypeId GetUniversalTypeId(const UniversalType& v) {
    return std::visit([](auto&& value) -> TypeId {
        using T = std::remove_cvref_t<decltype(value)>;
        return ToUniversalTypeId(GetTypeId<T>());
    }, v);
}

TypeId GetBeginBaseType() {
    return TypeId::Float;
}

TypeId GetEndBaseType() {
    return TypeId::Unknown;
}

TypeId GetNextBaseType(TypeId typeId) {
    if ((typeId == TypeId::Unknown) || (typeId >= TypeId::Generator3d)) {
        return TypeId::Unknown;
    }

    return static_cast<TypeId>(static_cast<uint8_t>(typeId) + 1);
}

bool IsValidPinType(const std::type_info& typeInfo) {
    auto typeId = GetTypeId(typeInfo);
    return (
        (typeId == TypeId::Float) ||
        (typeId == TypeId::Vector2f) ||
        (typeId == TypeId::Vector3f) ||
        (typeId == TypeId::Vector4f) ||
        (typeId == TypeId::Generator2d) ||
        (typeId == TypeId::Generator3d) ||
        (typeId == TypeId::UniversalType)
    );
}

bool IsEnableUI(TypeId typeId) {
    return (
        (typeId == TypeId::Float) ||
        (typeId == TypeId::Vector2f) ||
        (typeId == TypeId::Vector3f) ||
        (typeId == TypeId::Vector4f)
    );
}

}
