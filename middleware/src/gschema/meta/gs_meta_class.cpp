#include "middleware/gschema/meta/gs_meta_class.h"

#include "core/common/exception.h"


namespace gs {

MetaClass::MetaClass(TCtor& ctor, std::string_view name, std::string_view displayName, MetaClass* baseClass)
    : m_baseClass(baseClass)
    , m_ctor(ctor)
    , m_name(name.cbegin(), name.cend())
    , m_displayName(displayName.cbegin(), displayName.cend()) {

    if (m_name.empty()) {
        throw EngineError("gs::MetaClass::MetaClass: Attempting to create a metaclass with an empty name");
    }
    if (m_ctor == nullptr) {
        throw EngineError("gs::MetaClass::MetaClass: Attempting to create a metaclass (name = {}) with an empty constructor", name);
    }
    if (displayName.empty()) {
        m_displayName = m_name;
    }
}

void* MetaClass::Create() const {
    return m_ctor();
}

void MetaClass::AddProperty(const cpgf::GMetaProperty* property) {
    // TODO: check unique for property name
    m_properties.push_back(property);
}

std::vector<const cpgf::GMetaProperty*> MetaClass::GetProperties() const {
    if (m_baseClass == nullptr) {
        return m_properties;
    }

    auto result = m_baseClass->GetProperties();
    result.insert(result.end(), m_properties.begin(), m_properties.end());

    return result;
}

}
