#pragma once

#include <string>
#include <cstdint>
#include <string_view>

#include "core/common/ctor.h"
#include "middleware/gscheme/graph/gs_types_decl.h"
#include "middleware/gscheme/graph/gs_types_convert_func.h"


namespace cpgf {
    class GVariant;
    class GMetaClass;
    class GMetaProperty;
}

namespace gs {

class ClassType;
class TypesConvertStorage;
class Class : Fixed {
public:
    Class() = default;
    ~Class();

    void Create(const cpgf::GMetaClass* metaClass, ClassType* classType, const TypesConvertStorage* typesConvertStorage);

    // unique class name
    std::string_view GetName() const;
    // name for UI
    std::string GetPrettyName() const;

    // unique pin name
    std::string_view GetPinName(uint8_t pinIndex) const;
    // pin name for UI
    std::string GetPinPrettyName(uint8_t pinIndex) const;

    uint8_t EmbeddedPinsCount() const noexcept { return m_countEmbeddedPins; }
    uint8_t InputPinsCount() const noexcept { return m_countInputPins; }
    uint8_t OutputPinsCount() const noexcept { return m_countOutputPins; }

    TypeId GetDeclPinTypeId(uint8_t pinIndex) const noexcept;
    // will return false if convertation is not possible
    ConvertFunc GetFuncConvertToDeclType(uint8_t pinIndex, TypeId typeId) const;
    // valid only for input pins with decl type = UniversalType, typeId should bу concrete universal type
    void SetConcreteUniversalPinType(uint8_t pinIndex, void* instanceType, TypeId typeId);
    // valid only for input pins with decl type = UniversalType
    void ResetUniversalPinTypeToDefault(uint8_t pinIndex, void* instanceType);
    // valid only for output pins with decl type = UniversalType
    TypeId GetConcreteUniversalPinType(uint8_t pinIndex, void* instanceType);

    bool CheckIsClassTypeValid(void* instanceType) const;

    void NewInstance(void*& instance, void*& instanceType);
    void DeleteInstance(void* instance, void* instanceType);

    cpgf::GVariant GetValue(uint8_t pinIndex, const void* instance) const;
    void SetValue(uint8_t pinIndex, void* instance, const cpgf::GVariant& value) const;

    const cpgf::GVariant& GetDefaultValue(uint8_t pinIndex) const;
    void ResetToDefault(uint8_t pinIndex, void* instance) const;

private:
    void CheckIsValidUniversalPinIndex(uint8_t pinIndex, bool inputPinNeed);
    void CheckMetaClass(const cpgf::GMetaClass* metaClass, ClassType* classType) const;

private:
    uint8_t m_countEmbeddedPins = 0;
    uint8_t m_countInputPins = 0;
    uint8_t m_countOutputPins = 0;
    TypeId* m_declTypeIds = nullptr;
    const cpgf::GMetaProperty** m_props = nullptr;
    cpgf::GVariant* m_defaults = nullptr;
    const cpgf::GMetaClass* m_metaClass = nullptr;
    ClassType* m_classType = nullptr;
    const TypesConvertStorage* m_typesConvertStorage = nullptr;
};

}
