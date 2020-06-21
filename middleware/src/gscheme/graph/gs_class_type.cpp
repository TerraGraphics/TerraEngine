#include "middleware/gscheme/graph/gs_class_type.h"

#include <string>
#include <cstdlib>
#include <typeinfo>
#include <typeindex>

#include "cpgf/metaclass.h"
#include "core/common/meta.h"
#include "core/common/exception.h"
#include "middleware/gscheme/graph/gs_limits.h"
#include "middleware/gscheme/graph/gs_metadata.h"


namespace gs {

static_assert(sizeof(ClassType) == 40, "sizeof(ClassType) == 40 bytes");


ClassType::~ClassType() {
    if (m_props != nullptr) {
        delete[] m_props;
    }
    if (m_defaults != nullptr) {
        delete[] m_defaults;
    }
}

void ClassType::Create(const cpgf::GMetaClass* metaClass, const ConvertStorage* convertStorage) {
    try {
        CheckMetaClass(metaClass);
    } catch(const std::exception& e) {
        throw EngineError("gs::ClassType::Create: {}", e.what());
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
    m_convertStorage = convertStorage;
    m_metaClass = metaClass;
    for(size_t i=0; i!=metaClass->getPropertyCount(); ++i) {
        const cpgf::GMetaProperty* prop = metaClass->getPropertyAt(i);
        auto pinType = prop->getAnnotation(gs::MetaNames::PIN)->getValue(gs::MetaNames::PIN_TYPE)->toObject<gs::PinTypes>();
        switch (pinType) {
        case gs::PinTypes::EMBEDDED:
            m_props[embeddedIndex++] = prop;
            break;
        case gs::PinTypes::INPUT:
            m_props[inputIndex++] = prop;
            break;
        case gs::PinTypes::OUTPUT:
            m_props[outputIndex++] = prop;
            break;
        }
    }
}

std::string_view ClassType::GetImplName() const {
    return m_metaClass->getAnnotation(gs::MetaNames::TYPE_CLASS)->getValue(gs::MetaNames::IMPL_CLASS)->toString();
}

std::string_view ClassType::GetPinName(uint8_t pinIndex) const {
    return m_props[pinIndex]->getName();
}

void* ClassType::NewInstance() {
    void* instance = m_metaClass->getConstructorAt(0)->invoke(m_convertStorage);
    if (m_defaults == nullptr) {
        m_defaults = new TypeId[m_countEmbeddedPins + m_countInputPins];
        for (uint8_t i=0; i!=(m_countEmbeddedPins + m_countInputPins); ++i) {
            m_defaults[i] = GetType(i, instance);
        }
    }

    return instance;
}

void ClassType::DeleteInstance(void* instance) {
    if (instance != nullptr) {
        m_metaClass->destroyInstance(instance);
    }
}

TypeId ClassType::GetType(uint8_t pinIndex, const void* instance) const {
    return cpgf::fromVariant<TypeId>(m_props[pinIndex]->get(instance));
}

void ClassType::SetType(uint8_t pinIndex, void* instance, TypeId value) const {
    m_props[pinIndex]->set(instance, value);
}

TypeId ClassType::GetDefaultType(uint8_t pinIndex) const {
    return m_defaults[pinIndex];
}

void ClassType::ResetToDefault(uint8_t pinIndex, void* instance) const {
    m_props[pinIndex]->set(instance, m_defaults[pinIndex]);
}

void ClassType::CheckMetaClass(const cpgf::GMetaClass* metaClass) const {
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

    const cpgf::GMetaAnnotation* clsAnnotation = metaClass->getAnnotation(gs::MetaNames::TYPE_CLASS);
    if (clsAnnotation == nullptr) {
        throw EngineError("invalid metaClass (name = '{}'), has invalid annotation TYPE_CLASS", clsName);
    }

    const cpgf::GAnnotationValue* clsImplNameValue = clsAnnotation->getValue(gs::MetaNames::IMPL_CLASS);
    if ((clsImplNameValue == nullptr) || (!clsImplNameValue->canToString())) {
        throw EngineError(
            "invalid metaClass (name = '{}'), does not have annotations or has invalid annotation type for IMPL_CLASS, need std::string",
            clsName);
    }

    if (metaClass->getPropertyCount() > static_cast<size_t>(MAX_PINS_COUNT)) {
        throw EngineError("invalid metaClass (name = '{}'), max count properties = {}, have = {}",
            clsName, MAX_PINS_COUNT, metaClass->getPropertyCount());
    }

    auto typeIdIndex = std::type_index(typeid(TypeId));
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
        if ((propPinType != gs::PinTypes::EMBEDDED) && (propPinType != gs::PinTypes::INPUT) && (propPinType != gs::PinTypes::OUTPUT)) {
            throw EngineError(
                "invalid metaClass (name = '{}'), has property (name = {}) with invalid value for annotation PIN_TYPE = {}",
                clsName, propName, static_cast<int>(propPinType));
        }

        if (std::type_index(prop->getItemType().getBaseType().getStdTypeInfo()) != typeIdIndex) {
            throw EngineError(
                "invalid metaClass (name = '{}'), has property (name = {}) with invalid type ({}), need gs::TypeId",
                clsName, propName, meta::DemangleTypeName(prop->getItemType().getBaseType().getStdTypeInfo().name()));
        }
    }

    const cpgf::GMetaMethod* mIsValid = metaClass->getMethod(MetaNames::METHOD_IS_VALID);
    if (mIsValid == nullptr) {
        throw EngineError("invalid metaClass (name = '{}'), does not have method {}", clsName, MetaNames::METHOD_IS_VALID);
    }
    if (mIsValid->getParamCount() != 0) {
        throw EngineError("invalid metaClass (name = '{}'), has method {} with invalid count params = {}, need 0",
            clsName, MetaNames::METHOD_IS_VALID, mIsValid->getParamCount());
    }
    if (!mIsValid->hasResult()) {
        throw EngineError("invalid metaClass (name = '{}'), has method {} without result",
            clsName, MetaNames::METHOD_IS_VALID);
    }
    if (std::type_index(mIsValid->getResultType().getBaseType().getStdTypeInfo()) != std::type_index(typeid(bool))) {
        throw EngineError("invalid metaClass (name = '{}'), has method {} with invalid result type ({}), need bool",
            clsName, MetaNames::METHOD_IS_VALID, meta::DemangleTypeName(mIsValid->getResultType().getBaseType().getStdTypeInfo().name()));
    }

    const cpgf::GMetaMethod* mApply = metaClass->getMethod(MetaNames::METHOD_APPLY);
    if (mApply == nullptr) {
        throw EngineError("invalid metaClass (name = '{}'), does not have method {}", clsName, MetaNames::METHOD_APPLY);
    }
    if (mApply->getParamCount() != 0) {
        throw EngineError("invalid metaClass (name = '{}'), has method {} with invalid count params = {}, need 0",
            clsName, MetaNames::METHOD_APPLY, mApply->getParamCount());
    }
    if (mApply->hasResult()) {
        throw EngineError("invalid metaClass (name = '{}'), has method {} with result", clsName, MetaNames::METHOD_APPLY);
    }

    const cpgf::GMetaMethod* mReset = metaClass->getMethod(MetaNames::METHOD_RESET);
    if (mReset == nullptr) {
        throw EngineError("invalid metaClass (name = '{}'), does not have method {}", clsName, MetaNames::METHOD_RESET);
    }
    if (mReset->getParamCount() != 0) {
        throw EngineError("invalid metaClass (name = '{}'), has method {} with invalid count params = {}, need 0",
            clsName, MetaNames::METHOD_RESET, mReset->getParamCount());
    }
    if (mReset->hasResult()) {
        throw EngineError("invalid metaClass (name = '{}'), has method {} with result", clsName, MetaNames::METHOD_RESET);
    }

    if (metaClass->getConstructorCount() != 1) {
        throw EngineError("invalid metaClass (name = '{}'), has {} constructors, 1 is required", clsName, metaClass->getConstructorCount());
    }
}

}
