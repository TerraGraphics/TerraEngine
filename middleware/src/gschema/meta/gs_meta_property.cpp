#include "middleware/gschema/meta/gs_meta_property.h"

#include <cstdint>

#include "cpgf/variant.h"
#include "core/common/meta.h"
#include "core/common/exception.h"
#include "middleware/gschema/graph/gs_types.h"
#include "middleware/gschema/meta/gs_type_instance.h"
#include "middleware/gschema/meta/gs_meta_property_data.h"


namespace gs {

MetaProperty::MetaProperty(MetaPropertyDataBase* data, std::type_index typeIndex, std::string_view name, std::string_view displayName, PinTypes pinType, TypeInstanceEdit* typeInstance)
    : m_data(data)
    , m_typeInstance(typeInstance)
    , m_typeIndex(typeIndex)
    , m_name(name.cbegin(), name.cend())
    , m_displayName(displayName.cbegin(), displayName.cend())
    , m_pinType(pinType) {

    if (m_name.empty()) {
        throw EngineError("gs::MetaProperty::MetaProperty: Attempting to create a metaproperty with an empty name");
    }

    if (m_data == nullptr) {
        throw EngineError("gs::MetaProperty::MetaProperty: Attempting to create a metaproperty (name = {}) an empty property", m_name);
    }

    switch (m_pinType) {
    case PinTypes::EMBEDDED:
        if (m_typeInstance == nullptr) {
            throw EngineError(
                "gs::MetaProperty::MetaProperty: Attempting to create a metaproperty (name = {}) for embedded pin with an empty TypeInstance",
                m_name);
        }
        if (m_typeInstance->GetTypeIndex() != m_typeIndex) {
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
        if (!IsValidPinType(m_typeIndex)) {
            throw EngineError(
                "gs::MetaProperty::MetaProperty: Attempting to create a metaproperty (name = {}) with unsupported type = '{}' for this pin type",
                m_name, meta::DemangleTypeName(m_typeIndex.name()));
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
    if (m_data != nullptr) {
        m_data->Release();
    }
    if (m_typeInstance != nullptr) {
        delete m_typeInstance;
    }
}

cpgf::GVariant MetaProperty::Get(const void* instance) const {
	return m_data->Get(instance);
}

void MetaProperty::Set(void* instance, const cpgf::GVariant& value) const {
	m_data->Set(instance, value);
}

}
