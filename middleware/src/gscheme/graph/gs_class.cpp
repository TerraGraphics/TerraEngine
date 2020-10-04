#include "middleware/gscheme/graph/gs_class.h"

#include <cstdlib>
#include <typeinfo>
#include <typeindex>

#include "cpgf/metaclass.h"
#include "core/common/meta.h"
#include "core/common/exception.h"
#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/graph/gs_limits.h"
#include "middleware/gscheme/graph/gs_metadata.h"
#include "middleware/gscheme/graph/gs_class_type.h"


namespace gs {

static_assert(sizeof(Class) == 48, "sizeof(Class) == 48 bytes");

Class::~Class() {
    if (m_typeIds != nullptr) {
        delete[] m_typeIds;
    }
    if (m_props != nullptr) {
        delete[] m_props;
    }
    if (m_defaults != nullptr) {
        delete[] m_defaults;
    }
    m_metaClass = nullptr;
    m_classType = nullptr;
}

void Class::Create(const cpgf::GMetaClass* metaClass, ClassType* classType) {
    try {
        CheckMetaClass(metaClass, classType);
    } catch(const std::exception& e) {
        throw EngineError("gs::Class::Create: {}", e.what());
    }

    m_props = new const cpgf::GMetaProperty*[metaClass->getPropertyCount()];
    for(size_t i=0; i!=metaClass->getPropertyCount(); ++i) {
        const cpgf::GMetaProperty* prop = metaClass->getPropertyAt(i);
        auto pinType = prop->getAnnotation(gs::MetaNames::PIN)->getValue(gs::MetaNames::PIN_TYPE)->toObject<gs::PinTypes>();
        switch (pinType) {
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
    m_classType = classType;
    m_typeIds = new TypeId[metaClass->getPropertyCount()];
    for(size_t i=0; i!=metaClass->getPropertyCount(); ++i) {
        const cpgf::GMetaProperty* prop = metaClass->getPropertyAt(i);
        TypeId typeId = GetTypeId(prop->getItemType().getBaseType().getStdTypeInfo());
        auto pinType = prop->getAnnotation(gs::MetaNames::PIN)->getValue(gs::MetaNames::PIN_TYPE)->toObject<gs::PinTypes>();
        switch (pinType) {
        case gs::PinTypes::EMBEDDED:
            m_typeIds[embeddedIndex] = typeId;
            m_props[embeddedIndex++] = prop;
            break;
        case gs::PinTypes::INPUT:
            m_typeIds[inputIndex] = typeId;
            m_props[inputIndex++] = prop;
            break;
        case gs::PinTypes::OUTPUT:
            m_typeIds[outputIndex] = typeId;
            m_props[outputIndex++] = prop;
            break;
        }
    }
}

std::string_view Class::GetName() const {
    return m_metaClass->getName();
}

std::string Class::GetPrettyName() const {
    const cpgf::GMetaAnnotation* clsAnnotation = m_metaClass->getAnnotation(gs::MetaNames::CLASS);
    const cpgf::GAnnotationValue* clsPrettyNameValue = clsAnnotation->getValue(gs::MetaNames::PRETTY_NAME);
    if (clsPrettyNameValue != nullptr) {
        return clsPrettyNameValue->toString();
    } else {
        return m_metaClass->getName();
    }
}

std::string_view Class::GetPinName(uint8_t pinIndex) const {
    return m_props[pinIndex]->getName();
}

std::string Class::GetPinPrettyName(uint8_t pinIndex) const {
    const cpgf::GMetaAnnotation* pinAnnotation = m_props[pinIndex]->getAnnotation(gs::MetaNames::PIN);
    const cpgf::GAnnotationValue* propPrettyNameValue = pinAnnotation->getValue(gs::MetaNames::PRETTY_NAME);
    if (propPrettyNameValue != nullptr) {
        return propPrettyNameValue->toString();
    } else {
        return m_props[pinIndex]->getName();
    }
}

TypeId Class::GetPinTypeId(uint8_t pinIndex) const noexcept {
    return m_typeIds[pinIndex];
}

void Class::NewInstance(void*& instance, void*& instanceType) {
    instance = m_metaClass->createInstance();
    if (m_defaults == nullptr) {
        m_defaults = new cpgf::GVariant[m_countEmbeddedPins + m_countInputPins];
        for (uint8_t i=0; i!=(m_countEmbeddedPins + m_countInputPins); ++i) {
            m_defaults[i] = m_props[i]->get(instance);
        }
    }

    if (m_classType != nullptr) {
        instanceType = m_classType->NewInstance();
    } else {
        instanceType = nullptr;
    }
}

void Class::DeleteInstance(void* instance, void* instanceType) {
    if (instance != nullptr) {
        m_metaClass->destroyInstance(instance);
    }
    if (m_classType != nullptr) {
        m_classType->DeleteInstance(instanceType);
    }
}

cpgf::GVariant Class::GetValue(uint8_t pinIndex, const void* instance) const {
    return m_props[pinIndex]->get(instance);
}

void Class::SetValue(uint8_t pinIndex, void* instance, const cpgf::GVariant& value) const {
    m_props[pinIndex]->set(instance, value);
}

const cpgf::GVariant& Class::GetDefaultValue(uint8_t pinIndex) const {
    return m_defaults[pinIndex];
}

void Class::ResetToDefault(uint8_t pinIndex, void* instance) const {
    m_props[pinIndex]->set(instance, m_defaults[pinIndex]);
}

void Class::CheckMetaClass(const cpgf::GMetaClass* metaClass, ClassType* classType) const {
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

    bool universalTypeExists = false;
    auto universalIndex = std::type_index(typeid(UniversalType));
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

        universalTypeExists |= (std::type_index(typeInfo) == universalIndex);
    }

    if (universalTypeExists && (classType == nullptr)) {
        throw EngineError("invalid metaClass (name = '{}'), no class type found for it", clsName);
    }

    if ((!universalTypeExists) && (classType != nullptr)) {
        throw EngineError("invalid metaClass (name = '{}'), class types is found, if there are no inversion types.", clsName);
    }
}

}
