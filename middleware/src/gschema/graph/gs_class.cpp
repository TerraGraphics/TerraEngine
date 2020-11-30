#include "middleware/gschema/graph/gs_class.h"

#include <cstdlib>
#include <utility>
#include <typeinfo>

#include "cpgf/variant.h"
#include "core/common/meta.h"
#include "cpgf/metaproperty.h"
#include "cpgf/metaannotation.h"
#include "core/common/exception.h"
#include "middleware/gschema/graph/gs_types.h"
#include "middleware/gschema/graph/gs_limits.h"
#include "middleware/gschema/meta/gs_meta_class.h"
#include "middleware/gschema/graph/gs_types_fmt.h" // IWYU pragma: keep
#include "middleware/gschema/meta/gs_meta_consts.h"
#include "middleware/gschema/meta/gs_type_instance.h"
#include "middleware/gschema/graph/gs_types_convert_storage.h"


namespace gs {

static_assert(sizeof(Class) == 64, "sizeof(Class) == 64 bytes");

Class::~Class() {
    if (m_defaultTypeIds != nullptr) {
        delete[] m_defaultTypeIds;
    }
    if (m_embeddedTypeInstances != nullptr) {
        for (int i=0; i!=m_countEmbeddedPins; ++i) {
            delete m_embeddedTypeInstances[i];
        }
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

void Class::Create(const MetaClass* metaClass,
    const TypesConvertStorage* typesConvertStorage, const std::unordered_map<TypeId, TypeInstanceEdit*>* freeTypeInstances) {

    m_freeTypeInstances = freeTypeInstances;

    try {
        if (metaClass == nullptr) {
            throw EngineError("invalid metaClass");
        }

        CheckMetaClass(metaClass);
    } catch(const std::exception& e) {
        throw EngineError("gs::Class::Create: {}", e.what());
    }

    const std::vector<const cpgf::GMetaProperty*>& props = metaClass->GetProperties();
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
            m_defaultTypeIds[embeddedIndex] = TypeId::Unknown;
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
    return m_metaClass->GetName();
}

std::string_view Class::GetDisplayName() const {
    return m_metaClass->GetDisplayName();
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
    if (pinIndex < m_countEmbeddedPins) {
        throw EngineError("gs::Class::CanConvertToDefaultType: invalid pinIndex = {}, need not embedded index", pinIndex);
    }
    return m_typesConvertStorage->CanConvert(typeId, GetDeclPinTypeId(pinIndex));
}

ConvertFunc Class::GetFuncConvertToDefaultType(uint8_t pinIndex, TypeId typeId) const {
    if (pinIndex < m_countEmbeddedPins) {
        throw EngineError("gs::Class::CanConvertToDefaultType: invalid pinIndex = {}, need not embedded index", pinIndex);
    }
    if (m_typesConvertStorage->CanConvert(typeId, GetDeclPinTypeId(pinIndex))) {
        return m_typesConvertStorage->GetConvertFunc(typeId, GetDeclPinTypeId(pinIndex));
    }

    return nullptr;
}

void* Class::CreateInstance() {
    void* instance = m_metaClass->CreateInstance();

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

void Class::DestroyInstance(void* instance) {
    m_metaClass->DestroyInstance(instance);
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

TypeInstanceEdit* Class::GetFreeTypeInstance(TypeId typeId) const {
    if (auto it = m_freeTypeInstances->find(typeId); it != m_freeTypeInstances->cend()) {
        return it->second;
    }

    throw EngineError("gs::Class::GetFreeTypeInstance: invalid typeId = {}, need typeId enabled for UI", typeId);
}

TypeInstanceEdit* Class::GetTypeInstanceForEmbedded(uint8_t pinIndex) const {
    if (pinIndex >= m_countEmbeddedPins) {
        throw EngineError("gs::Class::GetTypeInstanceForEmbedded: invalid pinIndex = {}, need embedded index", pinIndex);
    }

    return m_embeddedTypeInstances[pinIndex];
}

void Class::CheckMetaClass(const MetaClass* metaClass) const {
    if ((m_countEmbeddedPins != 0) || (m_countInputPins != 0) || (m_countOutputPins != 0)) {
        throw EngineError("double create");
    }

    std::string_view clsName = metaClass->GetName();
    const std::vector<const cpgf::GMetaProperty*> props = metaClass->GetProperties();
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
            isValidType = true;
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
