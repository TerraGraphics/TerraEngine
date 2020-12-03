#include "middleware/gschema/meta/gs_meta_class.h"

#include "core/common/exception.h"
#include "middleware/gschema/meta/gs_meta_property.h"


namespace gs {

MetaClass::MetaClass(TCtor ctor, TDtor dtor, std::string_view name, std::string_view displayName, MetaClass* baseClass)
    : m_baseClass(baseClass)
    , m_ctor(ctor)
    , m_dtor(dtor)
    , m_name(name.cbegin(), name.cend())
    , m_displayName(displayName.cbegin(), displayName.cend()) {

    if (m_name.empty()) {
        throw EngineError("gs::MetaClass::MetaClass: Attempting to create a metaclass with an empty name");
    }
    if (m_ctor == nullptr) {
        throw EngineError("gs::MetaClass::MetaClass: Attempting to create a metaclass (name = {}) with an empty constructor", name);
    }
    if (m_dtor == nullptr) {
        throw EngineError("gs::MetaClass::MetaClass: Attempting to create a metaclass (name = {}) with an empty destructor", name);
    }
    if (displayName.empty()) {
        m_displayName = m_name;
    }
}

MetaClass::~MetaClass() {
    for (const auto* prop: m_properties) {
        delete prop;
    }
}

void* MetaClass::CreateInstance() const {
    return m_ctor();
}

void MetaClass::DestroyInstance(void* instance) const {
    if (instance != nullptr) {
        m_dtor(instance);
    }
}

void MetaClass::AddProperty(cpgf::GMetaProperty* property, std::string_view name, std::string_view displayName, PinTypes pinType, TypeInstanceEdit* typeInstance) {
    for (const auto* prop: m_properties) {
        if (prop->GetName() == name) {
            throw EngineError(
                "gs::MetaClass::AddProperty: metaproperty with name = {} already exists by name for metaclass = {}", name, m_name);
        }
    }
    m_properties.push_back(new MetaProperty(property, name, displayName, pinType, typeInstance));
}

std::vector<const MetaProperty*> MetaClass::GetProperties() const {
    if (m_baseClass == nullptr) {
        return m_properties;
    }

    auto result = m_baseClass->GetProperties();
    result.insert(result.end(), m_properties.begin(), m_properties.end());

    return result;
}

}
