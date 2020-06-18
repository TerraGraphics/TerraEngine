#include "middleware/gscheme/graph/gs_type_class.h"

#include <cstdlib>
#include <typeinfo>

#include "cpgf/metaclass.h"
#include "core/common/meta.h"
#include "core/common/exception.h"
#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/graph/gs_limits.h"
#include "middleware/gscheme/graph/gs_metadata.h"


namespace gs {

static_assert(sizeof(TypeClass) == 40, "sizeof(TypeClass) == 40 bytes");

TypeClass::~TypeClass() {
    if (m_typeIds != nullptr) {
        delete[] m_typeIds;
    }
    if (m_props != nullptr) {
        delete[] m_props;
    }
    if (m_defaults != nullptr) {
        delete[] m_defaults;
    }
}

void TypeClass::Create(const cpgf::GMetaClass* metaClass) {
    try {
        CheckMetaClass(metaClass);
    } catch(const std::exception& e) {
        throw EngineError("gs::TypeClass::Create: {}", e.what());
    }

    m_props = new const cpgf::GMetaProperty*[metaClass->getPropertyCount()];
    for(size_t i=0; i!=metaClass->getPropertyCount(); ++i) {
        const cpgf::GMetaProperty* prop = metaClass->getPropertyAt(i);
        const cpgf::GMetaAnnotation* pinAnnotation = prop->getAnnotation(gs::MetaNames::PIN);
        const cpgf::GAnnotationValue* propPinTypeValue = pinAnnotation->getValue(gs::MetaNames::PIN_TYPE);
        gs::PinTypes propPinType = propPinTypeValue->toObject<gs::PinTypes>();
        switch (propPinType) {
        case gs::PinTypes::EMBEDDED:
            ++m_countEmbeddedPins;
            break;
        case gs::PinTypes::INPUT:
            ++m_countInputPins;
            break;
        case gs::PinTypes::OUTPUT:
            ++m_countOutputPins;
            break;
        }
    }

    uint8_t embeddedIndex = 0;
    uint8_t inputIndex = embeddedIndex + m_countEmbeddedPins;
    uint8_t outputIndex = inputIndex + m_countInputPins;
    m_metaClass = metaClass;
    m_typeIds = new TypeId[metaClass->getPropertyCount()];
    for(size_t i=0; i!=metaClass->getPropertyCount(); ++i) {
        const cpgf::GMetaProperty* prop = metaClass->getPropertyAt(i);
        const cpgf::GMetaAnnotation* pinAnnotation = prop->getAnnotation(gs::MetaNames::PIN);
        const cpgf::GAnnotationValue* propPinTypeValue = pinAnnotation->getValue(gs::MetaNames::PIN_TYPE);
        gs::PinTypes propPinType = propPinTypeValue->toObject<gs::PinTypes>();
        const auto& typeInfo = prop->getItemType().getBaseType().getStdTypeInfo();
        switch (propPinType) {
        case gs::PinTypes::EMBEDDED:
            m_typeIds[embeddedIndex] = GetTypeId(typeInfo);
            m_props[embeddedIndex++] = prop;
            break;
        case gs::PinTypes::INPUT:
            m_typeIds[inputIndex] = GetTypeId(typeInfo);
            m_props[inputIndex++] = prop;
            break;
        case gs::PinTypes::OUTPUT:
            m_typeIds[outputIndex] = GetTypeId(typeInfo);
            m_props[outputIndex++] = prop;
            break;
        }
    }
}

std::string_view TypeClass::GetName() const {
    return m_metaClass->getName();
}

std::string TypeClass::GetPrettyName() const {
    const cpgf::GMetaAnnotation* clsAnnotation = m_metaClass->getAnnotation(gs::MetaNames::CLASS);
    const cpgf::GAnnotationValue* clsPrettyNameValue = clsAnnotation->getValue(gs::MetaNames::PRETTY_NAME);
    if (clsPrettyNameValue != nullptr) {
        return clsPrettyNameValue->toString();
    } else {
        return m_metaClass->getName();
    }
}

std::string_view TypeClass::GetPinName(uint8_t pinIndex) const {
    return m_props[pinIndex]->getName();
}

std::string TypeClass::GetPinPrettyName(uint8_t pinIndex) const {
    const cpgf::GMetaAnnotation* pinAnnotation = m_props[pinIndex]->getAnnotation(gs::MetaNames::PIN);
    const cpgf::GAnnotationValue* propPrettyNameValue = pinAnnotation->getValue(gs::MetaNames::PRETTY_NAME);
    if (propPrettyNameValue != nullptr) {
        return propPrettyNameValue->toString();
    } else {
        return m_props[pinIndex]->getName();
    }
}

TypeId TypeClass::GetPinTypeId(uint8_t pinIndex) const noexcept {
    return m_typeIds[pinIndex];
}

void* TypeClass::NewInstance() {
    void* instance = m_metaClass->createInstance();
    if (m_defaults == nullptr) {
        m_defaults = new cpgf::GVariant[m_countEmbeddedPins + m_countInputPins];
        for (uint8_t i=0; i!=(m_countEmbeddedPins + m_countInputPins); ++i) {
            m_defaults[i] = m_props[i]->get(instance);
        }
    }

    return instance;
}

void TypeClass::DeleteInstance(void* instance) {
    if (instance != nullptr) {
        m_metaClass->destroyInstance(instance);
    }
}

cpgf::GVariant TypeClass::GetValue(uint8_t pinIndex, const void* instance) const {
    return m_props[pinIndex]->get(instance);
}

void TypeClass::SetValue(uint8_t pinIndex, void* instance, const cpgf::GVariant& value) const {
    m_props[pinIndex]->set(instance, value);
}

const cpgf::GVariant& TypeClass::GetDefaultValue(uint8_t pinIndex) const {
    return m_defaults[pinIndex];
}

void TypeClass::ResetToDefault(uint8_t pinIndex, void* instance) const {
    m_props[pinIndex]->set(instance, m_defaults[pinIndex]);
}

void TypeClass::CheckMetaClass(const cpgf::GMetaClass* metaClass) const {
    if ((m_countEmbeddedPins != 0) || (m_countInputPins != 0) || (m_countOutputPins != 0)) {
        throw EngineError("double create");
    }

    if (metaClass == nullptr) {
        throw EngineError("invalid metaClass");
    }

    const std::string& clsName = metaClass->getName();
    if (clsName.empty()) {
        throw EngineError("invalid metaClass, name is empty");
    }

    const cpgf::GMetaAnnotation* clsAnnotation = metaClass->getAnnotation(gs::MetaNames::CLASS);
    if (clsAnnotation == nullptr) {
        throw EngineError("invalid metaClass (name = '{}'), has invalid annotation CLASS", clsName);
    }

    const cpgf::GAnnotationValue* clsPrettyNameValue = clsAnnotation->getValue(gs::MetaNames::PRETTY_NAME);
    if ((clsPrettyNameValue != nullptr) && (!clsPrettyNameValue->canToString())) {
        throw EngineError("invalid metaClass (name = '{}'), has invalid annotation type for PRETTY_NAME, need std::string", clsName);
    }

    if (metaClass->getPropertyCount() > static_cast<size_t>(MAX_PINS_COUNT)) {
        throw EngineError("invalid metaClass (name = '{}'), max count properties = {}, have = {}",
            clsName, MAX_PINS_COUNT, metaClass->getPropertyCount());
    }

    for(size_t i=0; i!=metaClass->getPropertyCount(); ++i) {
        const cpgf::GMetaProperty* prop = metaClass->getPropertyAt(i);
        if (prop == nullptr) {
            throw EngineError("invalid metaClass (name = '{}'), has invalid property", clsName);
        }

        const std::string& propName = prop->getName();
        if (propName.empty()) {
            throw EngineError("invalid metaClass (name = '{}'), has property with empty name", clsName);
        }

        const cpgf::GMetaAnnotation* pinAnnotation = prop->getAnnotation(gs::MetaNames::PIN);
        if (pinAnnotation == nullptr) {
            throw EngineError("invalid metaClass (name = '{}'), has property (name = {}) with invalid annotation PIN", clsName, propName);
        }

        const cpgf::GAnnotationValue* propPrettyNameValue = pinAnnotation->getValue(gs::MetaNames::PRETTY_NAME);
        if ((propPrettyNameValue != nullptr) && (!propPrettyNameValue->canToString())) {
            throw EngineError(
                "invalid metaClass (name = '{}'), has property (name = {}) with invalid annotation type for PRETTY_NAME, need std::string",
                clsName, propName);
        }

        const cpgf::GAnnotationValue* propPinTypeValue = pinAnnotation->getValue(gs::MetaNames::PIN_TYPE);
        if (propPinTypeValue == nullptr) {
            throw EngineError(
                "invalid metaClass (name = '{}'), has property (name = {}) without annotation PIN_TYPE",
                clsName, propName);
        }

        const cpgf::GVariant* propPinTypeVariant = propPinTypeValue->getVariant();
        if (propPinTypeVariant == nullptr) {
            throw EngineError(
                "invalid metaClass (name = '{}'), has property (name = {}) with invalid annotation PIN_TYPE",
                clsName, propName);
        }
        if (!cpgf::canFromVariant<gs::PinTypes>(*propPinTypeVariant)) {
            throw EngineError(
                "invalid metaClass (name = '{}'), has property (name = {}) with invalid type for annotation PIN_TYPE, need gs::PinTypes",
                clsName, propName);
        }
        gs::PinTypes propPinType = cpgf::fromVariant<gs::PinTypes>(*propPinTypeVariant);
        const auto& typeInfo = prop->getItemType().getBaseType().getStdTypeInfo();

        bool isValidType = true;
        if (propPinType == gs::PinTypes::EMBEDDED) {
            isValidType = IsValidEmbeddedPinType(typeInfo);
        } else if (propPinType == gs::PinTypes::INPUT) {
            isValidType = IsValidInputPinType(typeInfo);
        } else if (propPinType == gs::PinTypes::OUTPUT) {
            isValidType = IsValidOutputPinType(typeInfo);
        } else {
            throw EngineError(
                "invalid metaClass (name = '{}'), has property (name = {}) with invalid value for annotation PIN_TYPE = {}",
                clsName, propName, static_cast<int>(propPinType));
        }

        if (!isValidType) {
            throw EngineError(
                "invalid metaClass (name = '{}'), has property (name = {}) with unsupported type = '{}' for this pin type",
                clsName, propName, meta::DemangleTypeName(typeInfo.name()));
        }
    }
}

}
