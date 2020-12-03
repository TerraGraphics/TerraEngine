#include "middleware/gschema/graph/gs_class.h"

#include <vector>
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
#include "middleware/gschema/meta/gs_meta_property.h"
#include "middleware/gschema/graph/gs_types_convert_storage.h"


namespace gs {

static_assert(sizeof(Class) == 56, "sizeof(Class) == 56 bytes");

Class::~Class() {
    if (m_defaultTypeIds != nullptr) {
        delete[] m_defaultTypeIds;
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

        if ((m_countEmbeddedPins != 0) || (m_countInputPins != 0) || (m_countOutputPins != 0)) {
            throw EngineError("double create");
        }

        size_t propsCount = metaClass->GetProperties().size();
        if (propsCount > static_cast<size_t>(MAX_PINS_COUNT)) {
            throw EngineError("invalid metaClass (name = '{}'), max count properties = {}, have = {}",
                metaClass->GetName(), MAX_PINS_COUNT, propsCount);
        }
    } catch(const std::exception& e) {
        throw EngineError("gs::Class::Create: {}", e.what());
    }

    const std::vector<const MetaProperty*>& props = metaClass->GetProperties();
    m_props = new const MetaProperty*[props.size()];
    for(const MetaProperty* prop: props) {
        switch (prop->GetPinType()) {
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
    for(const MetaProperty* prop: props) {
        TypeId typeId = GetTypeId(prop->GetTypeIndex());
        switch (prop->GetPinType()) {
        case PinTypes::EMBEDDED:
            m_defaultTypeIds[embeddedIndex] = TypeId::Unknown;
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
    return m_props[pinIndex]->GetName();
}

std::string_view Class::GetPinDisplayName(uint8_t pinIndex) const {
    return m_props[pinIndex]->GetDisplayName();
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
            m_defaults[i] = m_props[i]->Get(instance);
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
    return m_props[pinIndex]->Get(instance);
}

void Class::SetValue(uint8_t pinIndex, void* instance, const cpgf::GVariant& value) const {
    m_props[pinIndex]->Set(instance, value);
}

const cpgf::GVariant& Class::GetDefaultValue(uint8_t pinIndex) const {
    return m_defaults[pinIndex];
}

void Class::ResetToDefault(uint8_t pinIndex, void* instance) const {
    m_props[pinIndex]->Set(instance, m_defaults[pinIndex]);
}

std::type_index Class::GetTypeIndex(uint8_t pinIndex) const {
    return m_props[pinIndex]->GetTypeIndex();
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

    return m_props[pinIndex]->GetTypeInstance();
}

}
