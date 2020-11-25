#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <typeindex>
#include <string_view>
#include <unordered_map>

#include "core/common/ctor.h"
#include "middleware/gschema/graph/gs_types_decl.h"
#include "middleware/gschema/graph/gs_types_convert_func.h"


namespace cpgf {
    class GVariant;
    class GMetaClass;
    class GMetaProperty;
}

namespace gs {

class TypeInstanceEdit;
class TypesConvertStorage;
class Class : Fixed {
public:
    Class() = default;
    ~Class();

    void Create(const cpgf::GMetaClass* metaClass,
        const TypesConvertStorage* typesConvertStorage, const std::unordered_map<TypeId, TypeInstanceEdit*>* freeTypeInstances);

    // unique class name
    std::string_view GetName() const;
    // name for UI
    std::string GetDisplayName() const;

    // unique pin name
    std::string_view GetPinName(uint8_t pinIndex) const;
    // pin name for UI
    std::string GetPinDisplayName(uint8_t pinIndex) const;

    uint8_t EmbeddedPinsCount() const noexcept { return m_countEmbeddedPins; }
    uint8_t InputPinsCount() const noexcept { return m_countInputPins; }
    uint8_t OutputPinsCount() const noexcept { return m_countOutputPins; }

    // valid only input pins, for output pins returns decl type, for embedded pins returns Unknown
    TypeId GetDefaultPinTypeId(uint8_t pinIndex) const noexcept;
    // valid for input and output pins, for embedded pins returns Unknown
    TypeId GetDeclPinTypeId(uint8_t pinIndex) const noexcept;
    // valid for input and output pins
    bool CanConvertToDefaultType(uint8_t pinIndex, TypeId typeId) const;
    // valid for input and output pins
    // will return nullptr if convertation is not possible
    ConvertFunc GetFuncConvertToDefaultType(uint8_t pinIndex, TypeId typeId) const;

    void* NewInstance();
    void DeleteInstance(void* instance);

    cpgf::GVariant GetValue(uint8_t pinIndex, const void* instance) const;
    void SetValue(uint8_t pinIndex, void* instance, const cpgf::GVariant& value) const;

    const cpgf::GVariant& GetDefaultValue(uint8_t pinIndex) const;
    void ResetToDefault(uint8_t pinIndex, void* instance) const;

    std::type_index GetTypeIndexForEmbedded(uint8_t pinIndex) const;

    TypeInstanceEdit* GetFreeTypeInstance(TypeId typeId) const;
    TypeInstanceEdit* GetTypeInstanceForEmbedded(uint8_t pinIndex) const;

private:
    void CheckMetaClass(const cpgf::GMetaClass* metaClass, const std::vector<const cpgf::GMetaProperty*>& props) const;

private:
    uint8_t m_countEmbeddedPins = 0;
    uint8_t m_countInputPins = 0;
    uint8_t m_countOutputPins = 0;
    TypeId* m_defaultTypeIds = nullptr;
    TypeInstanceEdit** m_embeddedTypeInstances = nullptr;
    const std::unordered_map<TypeId, TypeInstanceEdit*>* m_freeTypeInstances;
    const cpgf::GMetaProperty** m_props = nullptr;
    cpgf::GVariant* m_defaults = nullptr;
    const cpgf::GMetaClass* m_metaClass = nullptr;
    const TypesConvertStorage* m_typesConvertStorage = nullptr;
};

}
