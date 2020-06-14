#include "middleware/gscheme/graph/gs_type_class.h"

#include <new>
#include <cstdlib>

#include "core/common/exception.h"
#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/graph/gs_limits.h"
#include "middleware/gscheme/reflection/gs_metadata.h"


namespace gs {

static_assert(sizeof(TypeClass) == 32, "sizeof(TypeClass) == 32 bytes");

TypeClass::~TypeClass() {
    if (m_props != nullptr) {
        free(m_props);
    }
    if (m_defaults != nullptr) {
        delete[] m_defaults;
    }
}

void TypeClass::Create(const rttr::type& clsType) {
    try {
        CheckClassType(clsType);
    } catch(const std::exception& e) {
        throw EngineError("gs::TypeClass::Create: {}", e.what());
    }

    const auto props = clsType.get_properties();
    m_props = reinterpret_cast<rttr::property*>(malloc(props.size() * sizeof(rttr::property)));
    if (m_props == nullptr) {
        throw std::bad_alloc();
    }

    for(auto prop: props) {
        if (prop.get_metadata(MetaTypes::EMBEDDED_PROPERTY).is_valid()) {
            ++m_countEmbeddedPins;
        } else if (prop.get_metadata(MetaTypes::INPUT_PIN).is_valid()) {
            ++m_countInputPins;
        } else {
            ++m_countOutputPins;
        }
    }

    uint8_t embeddedIndex = 0;
    uint8_t inputIndex = embeddedIndex + m_countEmbeddedPins;
    uint8_t outputIndex = inputIndex + m_countInputPins;
    m_clsType = clsType;
    for(auto prop: props) {
        if (prop.get_metadata(MetaTypes::EMBEDDED_PROPERTY).is_valid()) {
            m_props[embeddedIndex++] = prop;
        } else if (prop.get_metadata(MetaTypes::INPUT_PIN).is_valid()) {
            m_props[inputIndex++] = prop;
        } else {
            m_props[outputIndex++] = prop;
        }
    }
}

std::string_view TypeClass::GetName() const {
    auto name = m_clsType.get_name();
    return std::string_view(name.cbegin(), name.cend());
}

std::string TypeClass::GetPrettyName() const {
    return m_clsType.get_metadata(MetaTypes::CLASS).get_value<std::string>();
}

std::string_view TypeClass::GetPinName(uint8_t pinIndex) const {
    auto name = m_props[pinIndex].get_name();
    return std::string_view(name.cbegin(), name.cend());
}

std::string TypeClass::GetPinPrettyName(uint8_t pinIndex) const {
    MetaTypes metaName = MetaTypes::OUTPUT_PIN;
    if (pinIndex < m_countEmbeddedPins) {
        metaName = MetaTypes::EMBEDDED_PROPERTY;
    } else if (pinIndex < (m_countEmbeddedPins + m_countInputPins)) {
        metaName = MetaTypes::INPUT_PIN;
    }

    return m_props[pinIndex].get_metadata(metaName).get_value<std::string>();
}

rttr::variant TypeClass::NewInstance() {
    if (m_defaults != nullptr) {
        return m_clsType.create();
    } else {
        auto instance = m_clsType.create();
        m_defaults = new rttr::variant[m_countEmbeddedPins + m_countInputPins];
        for (uint8_t i=0; i!=(m_countEmbeddedPins + m_countInputPins); ++i) {
            m_defaults[i] = m_props[i].get_value(instance);
        }

        return instance;
    }
}

rttr::variant TypeClass::GetValue(uint8_t pinIndex, rttr::variant& instance) const {
    return m_props[pinIndex].get_value(instance);
}

void TypeClass::SetValue(uint8_t pinIndex, rttr::variant& instance, const rttr::variant& value) const {
    m_props[pinIndex].set_value(instance, value);
}

const rttr::variant& TypeClass::GetDefaultValue(uint8_t pinIndex) const {
    return m_defaults[pinIndex];
}

void TypeClass::ResetToDefault(uint8_t pinIndex, rttr::variant& instance) const {
    m_props[pinIndex].set_value(instance, m_defaults[pinIndex]);
}

void TypeClass::CheckClassType(const rttr::type& clsType) const {
    if ((m_countEmbeddedPins != 0) || (m_countInputPins != 0) || (m_countOutputPins != 0)) {
        throw EngineError("double create");
    }

    if (!clsType.is_valid()) {
        throw EngineError("invalid clsType");
    }

    std::string clsName = clsType.get_name().to_string();
    if (clsName.empty()) {
        throw EngineError("invalid clsType, type name is empty");
    }

    auto clsMeta = clsType.get_metadata(MetaTypes::CLASS);
    if (!clsMeta.is_valid()) {
        throw EngineError("invalid clsType (name = '{}'), has invalid metadata CLASS", clsName);
    }
    if (clsMeta.get_type().get_id() != RttrTypeIdString()) {
        throw EngineError("invalid clsType (name = '{}'), has invalid value type = '{}' for metadata CLASS, need std::string",
            clsName, clsMeta.get_type().get_name().to_string());
    }

    const auto props = clsType.get_properties();
    if (props.size() > static_cast<size_t>(MAX_PINS_COUNT)) {
        throw EngineError("invalid clsType (name = '{}'), max count properties = {}", clsName, MAX_PINS_COUNT);
    }

    for(auto prop: props) {
        if (!prop.is_valid()) {
            throw EngineError("invalid clsType (name = '{}'), has invalid property", clsName);
        }

        auto propName = prop.get_name().to_string();
        if (propName.empty()) {
            throw EngineError("invalid clsType (name = '{}'), has property with empty name", clsName);
        }

        bool isValidType = false;
        auto propTypeId = prop.get_type().get_id();
        auto propMeta = prop.get_metadata(MetaTypes::EMBEDDED_PROPERTY);
        if (propMeta.is_valid()) {
            isValidType = IsValidEmbeddedPinRttrTypeId(propTypeId);
        } else  {
            propMeta = prop.get_metadata(MetaTypes::INPUT_PIN);
            if (propMeta.is_valid()) {
                isValidType = IsValidInputPinRttrTypeId(propTypeId);
            } else {
                propMeta = prop.get_metadata(MetaTypes::OUTPUT_PIN);
                if (propMeta.is_valid()) {
                    isValidType = IsValidOutputPinRttrTypeId(propTypeId);
                } else {
                    throw EngineError("invalid clsType (name = '{}'), has property with name = '{}' and without metadata",
                        clsName, propName);
                }
            }
        }

        if (!isValidType) {
            throw EngineError(
                "invalid clsType (name = '{}'), has property with name = '{}' and unknown type = '{}'",
                clsName, propName, prop.get_type().get_name().to_string());
        }

        if (propMeta.get_type().get_id() != RttrTypeIdString()) {
            throw EngineError(
                "invalid clsType (name = '{}'), has property with name = '{}' and invalid metadata value type = '{}', need std::string",
                clsName, propName, propMeta.get_type().get_name().to_string());
        }
    }
}

}
