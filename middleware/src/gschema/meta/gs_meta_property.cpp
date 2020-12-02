#include "middleware/gschema/meta/gs_meta_property.h"

#include "cpgf/metaproperty.h"
#include "core/common/exception.h"
#include "middleware/gschema/meta/gs_type_instance.h"


namespace gs {

MetaProperty::MetaProperty(cpgf::GMetaProperty* property, std::string_view name, std::string_view displayName, PinTypes pinType, TypeInstanceEdit* typeInstance)
    : m_property(property)
    , m_typeInstance(typeInstance)
    , m_name(name.cbegin(), name.cend())
    , m_displayName(displayName.cbegin(), displayName.cend())
    , m_pinType(pinType) {

    if (m_name.empty()) {
        throw EngineError("gs::MetaProperty::MetaProperty: Attempting to create a metaproperty with an empty name");
    }
    switch (m_pinType) {
    case PinTypes::EMBEDDED:
        if (m_typeInstance == nullptr) {
            throw EngineError(
                "gs::MetaProperty::MetaProperty: Attempting to create a metaproperty (name = {}) for embedded pin with an empty TypeInstance",
                m_name);
        }
        if (m_typeInstance->GetTypeIndex() != std::type_index(m_property->getItemType().getBaseType().getStdTypeInfo())) {
            throw EngineError(
                "gs::MetaProperty::MetaProperty: Attempting to create a metaproperty (name = {}) for embedded pin with wrong type_index",
                m_name);
        }
        break;
    case PinTypes::INPUT:
    case PinTypes::OUTPUT:
        if (m_typeInstance != nullptr) {
            throw EngineError(
                "gs::MetaProperty::MetaProperty: Attempting to create a metaproperty (name = {}) for input or output pin with a TypeInstance",
                m_name);
        }
        break;
    default:
        throw EngineError(
                "gs::MetaProperty::MetaProperty: Attempting to create a metaproperty (name = {}) with unknown PinType = {}",
                m_name, static_cast<uint8_t>(pinType));
        break;
    }

    if (displayName.empty()) {
        m_displayName = m_name;
    }
}

MetaProperty::~MetaProperty() {
    if (m_property != nullptr) {
        delete m_property;
    }
    if (m_typeInstance != nullptr) {
        delete m_typeInstance;
    }
}

const std::type_info& MetaProperty::GetTypeInfo() const {
    return m_property->getItemType().getBaseType().getStdTypeInfo();
}

cpgf::GVariant MetaProperty::Get(const void* instance) const {
    return m_property->get(instance);
}

void MetaProperty::Set(void* instance, const cpgf::GVariant& value) const {
    return m_property->set(instance, value);
}

}
