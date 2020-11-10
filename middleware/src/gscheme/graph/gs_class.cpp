#include "middleware/gscheme/graph/gs_class.h"

#include <cstdlib>
#include <typeinfo>

#include "cpgf/metaclass.h"
#include "core/common/meta.h"
#include "core/common/exception.h"
#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/graph/gs_limits.h"
#include "middleware/gscheme/graph/gs_types_fmt.h" // IWYU pragma: keep
#include "middleware/gscheme/meta/gs_meta_consts.h"
#include "middleware/gscheme/meta/gs_type_instance.h"
#include "middleware/gscheme/graph/gs_types_convert_storage.h"


namespace gs {

static_assert(sizeof(Class) == 56, "sizeof(Class) == 56 bytes");

Class::~Class() {
    if (m_defaultTypeIds != nullptr) {
        delete[] m_defaultTypeIds;
    }
    if (m_embeddedTypeInstances != nullptr) {
        delete[] m_embeddedTypeInstances;
    }
    if (m_props != nullptr) {
        delete[] m_props;
    }
    if (m_defaults != nullptr) {
        delete[] m_defaults;
    }
    m_metaClass = nullptr;
    m_typesConvertStorage = nullptr;
}

void Class::Create(const cpgf::GMetaClass* metaClass, const TypesConvertStorage* typesConvertStorage) {
    std::vector<const cpgf::GMetaProperty*> props;

    try {
        if (metaClass == nullptr) {
            throw EngineError("invalid metaClass");
        }

        for(size_t i=0; i!=metaClass->getPropertyCount(); ++i) {
            props.push_back(metaClass->getPropertyAt(i));
        }

        for(size_t j=0; j!=metaClass->getBaseCount(); ++j) {
            auto* cls = metaClass->getBaseClass(j);
            for(size_t i=0; i!=cls->getPropertyCount(); ++i) {
                props.push_back(cls->getPropertyAt(i));
            }
        }

        CheckMetaClass(metaClass, props);
    } catch(const std::exception& e) {
        throw EngineError("gs::Class::Create: {}", e.what());
    }

    m_props = new const cpgf::GMetaProperty*[props.size()];
    for(const cpgf::GMetaProperty* prop: props) {
        auto pinType = prop->getAnnotation(MetaNames::PIN)->getValue(MetaNames::PIN_TYPE)->toObject<PinTypes>();
        switch (pinType) {
        case PinTypes::EMBEDDED:
            ++m_countEmbeddedPins;
            break;
        case PinTypes::INPUT:
            ++m_countInputPins;
            break;
        case PinTypes::OUTPUT:
            ++m_countOutputPins;
            break;
        }
    }

    uint8_t embeddedIndex = 0;
    uint8_t inputIndex = embeddedIndex + m_countEmbeddedPins;
    uint8_t outputIndex = inputIndex + m_countInputPins;
    m_typesConvertStorage = typesConvertStorage;
    m_metaClass = metaClass;
    m_defaultTypeIds = new TypeId[props.size()];
    m_embeddedTypeInstances = new TypeInstanceEdit*[m_countEmbeddedPins];
    for(const cpgf::GMetaProperty* prop: props) {
        TypeId typeId = GetTypeId(prop->getItemType().getBaseType().getStdTypeInfo());
        const cpgf::GMetaAnnotation* pinAnnotation = prop->getAnnotation(MetaNames::PIN);
        auto pinType = pinAnnotation->getValue(MetaNames::PIN_TYPE)->toObject<PinTypes>();
        switch (pinType) {
        case PinTypes::EMBEDDED:
            m_defaultTypeIds[embeddedIndex] = typeId;
            m_embeddedTypeInstances[embeddedIndex] = pinAnnotation->getValue(MetaNames::TYPE_INSTANCE)->toObject<TypeInstanceEdit*>();
            m_props[embeddedIndex++] = prop;
            break;
        case PinTypes::INPUT:
            m_defaultTypeIds[inputIndex] = typeId;
            m_props[inputIndex++] = prop;
            break;
        case PinTypes::OUTPUT:
            m_defaultTypeIds[outputIndex] = typeId;
            m_props[outputIndex++] = prop;
            break;
        }
    }
}

std::string_view Class::GetName() const {
    return m_metaClass->getName();
}

std::string Class::GetDisplayName() const {
    const cpgf::GMetaAnnotation* clsAnnotation = m_metaClass->getAnnotation(MetaNames::CLASS);
    const cpgf::GAnnotationValue* clsDisplayNameValue = clsAnnotation->getValue(MetaNames::DISPLAY_NAME);
    if (clsDisplayNameValue != nullptr) {
        return clsDisplayNameValue->toString();
    } else {
        return m_metaClass->getName();
    }
}

std::string_view Class::GetPinName(uint8_t pinIndex) const {
    return m_props[pinIndex]->getName();
}

std::string Class::GetPinDisplayName(uint8_t pinIndex) const {
    const cpgf::GMetaAnnotation* pinAnnotation = m_props[pinIndex]->getAnnotation(MetaNames::PIN);
    const cpgf::GAnnotationValue* propDisplayNameValue = pinAnnotation->getValue(MetaNames::DISPLAY_NAME);
    if (propDisplayNameValue != nullptr) {
        return propDisplayNameValue->toString();
    } else {
        return m_props[pinIndex]->getName();
    }
}

TypeId Class::GetDeclPinTypeId(uint8_t pinIndex) const noexcept {
    if (HasUniversalBit(m_defaultTypeIds[pinIndex])) {
        return TypeId::UniversalType;
    }

    return m_defaultTypeIds[pinIndex];
}

TypeId Class::GetDefaultPinTypeId(uint8_t pinIndex) const noexcept {
    return m_defaultTypeIds[pinIndex];
}

bool Class::CanConvertToDefaultType(uint8_t pinIndex, TypeId typeId) const {
    return m_typesConvertStorage->CanConvert(typeId, GetDeclPinTypeId(pinIndex));
}

ConvertFunc Class::GetFuncConvertToDefaultType(uint8_t pinIndex, TypeId typeId) const {
    if (m_typesConvertStorage->CanConvert(typeId, GetDeclPinTypeId(pinIndex))) {
        return m_typesConvertStorage->GetConvertFunc(typeId, GetDeclPinTypeId(pinIndex));
    }

    return nullptr;
}

void* Class::NewInstance() {
    void* instance = m_metaClass->createInstance();

    if (m_defaults == nullptr) {
        m_defaults = new cpgf::GVariant[m_countEmbeddedPins + m_countInputPins];
        for (uint8_t i=0; i!=(m_countEmbeddedPins + m_countInputPins); ++i) {
            // inside the value is completely copied
            m_defaults[i] = m_props[i]->get(instance);
            if (HasUniversalBit(m_defaultTypeIds[i])) {
                m_defaultTypeIds[i] = GetUniversalTypeId(cpgf::fromVariant<UniversalType>(m_defaults[i]));
            }
        }
    }

    return instance;
}

void Class::DeleteInstance(void* instance) {
    if (instance != nullptr) {
        m_metaClass->destroyInstance(instance);
    }
}

cpgf::GVariant Class::GetValue(uint8_t pinIndex, const void* instance) const {
    // inside the value is completely copied
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

std::type_index Class::GetTypeIndexForEmbedded(uint8_t pinIndex) const {
    if (pinIndex >= m_countEmbeddedPins) {
        throw EngineError("gs::Class::GetTypeIndexForEmbedded: invalid pinIndex = {}, need embedded index", pinIndex);
    }

    return m_embeddedTypeInstances[pinIndex]->GetTypeIndex();
}

TypeInstance* Class::GetTypeInstanceForEmbedded(uint8_t pinIndex, const void* instance) const {
    if (pinIndex >= m_countEmbeddedPins) {
        throw EngineError("gs::Class::GetTypeInstanceForEmbedded: invalid pinIndex = {}, need embedded index", pinIndex);
    }

    auto* typeInstance = m_embeddedTypeInstances[pinIndex];
    typeInstance->Init(GetValue(pinIndex, instance));
    return static_cast<TypeInstance*>(typeInstance);
}

bool Class::ApplyTypeInstanceForEmbedded(uint8_t pinIndex, void* instance) const {
    if (pinIndex >= m_countEmbeddedPins) {
        throw EngineError("gs::Class::ApplyTypeInstanceForEmbedded: invalid pinIndex = {}, need embedded index", pinIndex);
    }

    auto* typeInstance = m_embeddedTypeInstances[pinIndex];
    if (typeInstance->IsChanged()) {
        SetValue(pinIndex, instance, typeInstance->Result());
        return true;
    }

    return false;
}

void Class::CheckMetaClass(const cpgf::GMetaClass* metaClass, const std::vector<const cpgf::GMetaProperty*>& props) const {
    if ((m_countEmbeddedPins != 0) || (m_countInputPins != 0) || (m_countOutputPins != 0)) {
        throw EngineError("double create");
    }

    const std::string& clsName = metaClass->getName();
    if (clsName.empty()) {
        throw EngineError("invalid metaClass, name is empty");
    }

    const cpgf::GMetaAnnotation* clsAnnotation = metaClass->getAnnotation(MetaNames::CLASS);
    if (clsAnnotation == nullptr) {
        throw EngineError("invalid metaClass (name = '{}'), has invalid annotation CLASS", clsName);
    }

    const cpgf::GAnnotationValue* clsDisplayNameValue = clsAnnotation->getValue(MetaNames::DISPLAY_NAME);
    if ((clsDisplayNameValue != nullptr) && (!clsDisplayNameValue->canToString())) {
        throw EngineError("invalid metaClass (name = '{}'), has invalid annotation type for DISPLAY_NAME, need std::string", clsName);
    }

    if (props.size() > static_cast<size_t>(MAX_PINS_COUNT)) {
        throw EngineError("invalid metaClass (name = '{}'), max count properties = {}, have = {}",
            clsName, MAX_PINS_COUNT, props.size());
    }

    for(const cpgf::GMetaProperty* prop: props) {
        if (prop == nullptr) {
            throw EngineError("invalid metaClass (name = '{}'), has invalid property", clsName);
        }

        const std::string& propName = prop->getName();
        if (propName.empty()) {
            throw EngineError("invalid metaClass (name = '{}'), has property with empty name", clsName);
        }

        const cpgf::GMetaAnnotation* pinAnnotation = prop->getAnnotation(MetaNames::PIN);
        if (pinAnnotation == nullptr) {
            throw EngineError("invalid metaClass (name = '{}'), has property (name = {}) with invalid annotation PIN", clsName, propName);
        }

        const cpgf::GAnnotationValue* propDisplayNameValue = pinAnnotation->getValue(MetaNames::DISPLAY_NAME);
        if ((propDisplayNameValue != nullptr) && (!propDisplayNameValue->canToString())) {
            throw EngineError(
                "invalid metaClass (name = '{}'), has property (name = {}) with invalid annotation type for DISPLAY_NAME, need std::string",
                clsName, propName);
        }

        const cpgf::GAnnotationValue* propPinTypeValue = pinAnnotation->getValue(MetaNames::PIN_TYPE);
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
        if (!cpgf::canFromVariant<PinTypes>(*propPinTypeVariant)) {
            throw EngineError(
                "invalid metaClass (name = '{}'), has property (name = {}) with invalid type for annotation PIN_TYPE, need gs::PinTypes",
                clsName, propName);
        }
        PinTypes propPinType = cpgf::fromVariant<PinTypes>(*propPinTypeVariant);

        const cpgf::GAnnotationValue* propPinTypeInstance = pinAnnotation->getValue(MetaNames::TYPE_INSTANCE);
        if (propPinType == PinTypes::EMBEDDED) {
            if (propPinTypeInstance == nullptr) {
                throw EngineError(
                    "invalid metaClass (name = '{}'), has embedded property (name = {}) without annotation TYPE_INSTANCE",
                    clsName, propName);
            }
            const cpgf::GVariant* propPinTypeInstanceVariant = propPinTypeInstance->getVariant();
            if (propPinTypeInstanceVariant == nullptr) {
                throw EngineError(
                    "invalid metaClass (name = '{}'), has embedded property (name = {}) with invalid annotation TYPE_INSTANCE",
                    clsName, propName);
            }
            if (!cpgf::canFromVariant<TypeInstanceEdit*>(*propPinTypeInstanceVariant)) {
                throw EngineError(
                    "invalid metaClass (name = '{}'), has embedded property (name = {}) with invalid type for annotation TYPE_INSTANCE, need gs::TypeInstanceEdit*",
                    clsName, propName);
            }
            TypeInstanceEdit* typeInstance = cpgf::fromVariant<TypeInstanceEdit*>(*propPinTypeInstanceVariant);
            if (typeInstance == nullptr) {
                throw EngineError(
                    "invalid metaClass (name = '{}'), has embedded property (name = {}) with annotation TYPE_INSTANCE equals nullptr",
                    clsName, propName);
            }
        }

        const auto& typeInfo = prop->getItemType().getBaseType().getStdTypeInfo();
        bool isValidType = true;
        if (propPinType == PinTypes::EMBEDDED) {
            isValidType = IsValidEmbeddedPinType(typeInfo);
        } else if (propPinType == PinTypes::INPUT) {
            isValidType = IsValidInputPinType(typeInfo);
        } else if (propPinType == PinTypes::OUTPUT) {
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
