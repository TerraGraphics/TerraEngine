#pragma once

#include <cstdint>
#include <string_view>

#include "core/common/ctor.h"
#include "middleware/gscheme/graph/gs_types_decl.h"


namespace cpgf {
    class GMetaClass;
    class GMetaProperty;
}

namespace gs {

class ConvertStorage;
class ClassType : Fixed {
public:
    ClassType() = default;
    ~ClassType();

    void Create(const cpgf::GMetaClass* metaClass, const ConvertStorage* convertStorage);

    std::string_view GetName() const;
    std::string_view GetImplName() const;
    std::string_view GetPinName(uint8_t pinIndex) const;

    uint8_t EmbeddedPinsCount() const noexcept { return m_countEmbeddedPins; }
    uint8_t InputPinsCount() const noexcept { return m_countInputPins; }
    uint8_t OutputPinsCount() const noexcept { return m_countOutputPins; }

    void* NewInstance();
    void DeleteInstance(void* instance);

    TypeId GetType(uint8_t pinIndex, const void* instance) const;
    void SetType(uint8_t pinIndex, void* instance, TypeId value) const;

    TypeId GetDefaultType(uint8_t pinIndex) const;
    void ResetToDefault(uint8_t pinIndex, void* instance) const;

private:
    void CheckMetaClass(const cpgf::GMetaClass* metaClass) const;

private:
    uint8_t m_countEmbeddedPins = 0;
    uint8_t m_countInputPins = 0;
    uint8_t m_countOutputPins = 0;
    const ConvertStorage* m_convertStorage;
    const cpgf::GMetaProperty** m_props = nullptr;
    TypeId* m_defaults = nullptr;
    const cpgf::GMetaClass* m_metaClass = nullptr;
};

}