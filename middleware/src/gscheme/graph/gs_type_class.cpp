#include "middleware/gscheme/graph/gs_type_class.h"

#include <new>
#include <cstdlib>

#include "core/common/exception.h"
#include "middleware/gscheme/rttr/type.h"
#include "middleware/gscheme/rttr/variant.h"
#include "middleware/gscheme/graph/gs_limits.h"
#include "middleware/gscheme/reflection/gs_metadata.h"


namespace gs {

static_assert(sizeof(TypeClass) == 48, "sizeof(TypeClass) == 48 bytes");

TypeClass::~TypeClass() {
    if (m_props != nullptr) {
        free(m_props);
    }
}

void TypeClass::Create(const rttr::type& clsType) {
    if (!m_name.empty()) {
        throw EngineError("gs::TypeClass::Create: double create");
    }

    if (!clsType.is_valid()) {
        throw EngineError("gs::TypeClass::Create: invalid clsType");
    }

    m_name = clsType.get_name().to_string();
    if (m_name.empty()) {
        throw EngineError("gs::TypeClass::Create: invalid clsType, type name is empty");
    }

    auto clsMeta = clsType.get_metadata(GSMetaTypes::GS_CLASS);
    if (!clsMeta.is_valid()) {
        throw EngineError("gs::TypeClass::Create: invalid clsType (name = \"{}\"), has invalid metadata GS_CLASS", m_name);
    }

    const auto props = clsType.get_properties();
    if (props.size() > static_cast<size_t>(MAX_PINS_COUNT)) {
        throw EngineError("gs::TypeClass::Create: invalid clsType (name = \"{}\"), max count properties = {}", m_name, MAX_PINS_COUNT);
    }

    for(auto prop: props) {
        if (!prop.is_valid()) {
            throw EngineError("gs::TypeClass::Create: invalid clsType (name = \"{}\"), has invalid property", m_name);
        }
        if (prop.get_name().empty()) {
            throw EngineError("gs::TypeClass::Create: invalid clsType (name = \"{}\"), has property with empty name", m_name);
        }

        if (prop.get_metadata(GSMetaTypes::GS_EMBEDDED_PROPERTY).is_valid()) {
            ++m_countEmbeddedPins;
        } else if (prop.get_metadata(GSMetaTypes::GS_INPUT_PIN).is_valid()) {
            ++m_countInputPins;
        } else if (prop.get_metadata(GSMetaTypes::GS_OUTPUT_PIN).is_valid()) {
            ++m_countOutputPins;
        } else {
            throw EngineError("gs::TypeClass::Create: invalid clsType (name = \"{}\"), property (name = \"{}\"), has no known metadata",
                m_name, prop.get_name().to_string());
        }
    }

    uint8_t embeddedIndex = 0;
    uint8_t inputIndex = embeddedIndex + m_countEmbeddedPins;
    uint8_t outputIndex = inputIndex + m_countInputPins;

    m_props = reinterpret_cast<rttr::property*>(malloc(props.size() * sizeof(rttr::property)));
    if (m_props == nullptr) {
        throw std::bad_alloc();
    }
    for(auto prop: props) {
        if (prop.get_metadata(GSMetaTypes::GS_EMBEDDED_PROPERTY).is_valid()) {
            m_props[embeddedIndex++] = prop;
        } else if (prop.get_metadata(GSMetaTypes::GS_INPUT_PIN).is_valid()) {
            m_props[inputIndex++] = prop;
        } else {
            m_props[outputIndex++] = prop;
        }
    }
}

std::string_view TypeClass::GetName() const {
    return m_name;
}

std::string_view TypeClass::GetPrettyName() const {
    return m_name;
}

rttr::variant TypeClass::GetValue(uint8_t pinIndex, rttr::instance& instance) const {
    return m_props[pinIndex].get_value(instance);
}

void TypeClass::SetValue(uint8_t pinIndex, rttr::instance& instance, rttr::variant& value) const {
    m_props[pinIndex].set_value(instance, value);
}

}
