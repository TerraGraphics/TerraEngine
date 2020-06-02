#include "middleware/gscheme/graph/gs_type_class.h"

#include <new>
#include <string>
#include <cstdlib>

#include "core/common/exception.h"
#include "middleware/gscheme/graph/gs_limits.h"
#include "middleware/gscheme/reflection/gs_metadata.h"


namespace gs {

static_assert(sizeof(TypeClass) == 24, "sizeof(TypeClass) == 24 bytes");

TypeClass::~TypeClass() {
    if (m_props != nullptr) {
        free(m_props);
    }
}

void TypeClass::Create(const rttr::type& clsType) {
    if ((m_countEmbeddedPins != 0) || (m_countInputPins != 0) || (m_countOutputPins != 0)) {
        throw EngineError("gs::TypeClass::Create: double create");
    }

    if (!clsType.is_valid()) {
        throw EngineError("gs::TypeClass::Create: invalid clsType");
    }

    std::string clsName = clsType.get_name().to_string();
    if (clsName.empty()) {
        throw EngineError("gs::TypeClass::Create: invalid clsType, type name is empty");
    }

    auto clsMeta = clsType.get_metadata(GSMetaTypes::GS_CLASS);
    if (!clsMeta.is_valid()) {
        throw EngineError("gs::TypeClass::Create: invalid clsType (name = \"{}\"), has invalid metadata GS_CLASS", clsName);
    }

    const auto props = clsType.get_properties();
    if (props.size() > static_cast<size_t>(MAX_PINS_COUNT)) {
        throw EngineError("gs::TypeClass::Create: invalid clsType (name = \"{}\"), max count properties = {}", clsName, MAX_PINS_COUNT);
    }

    uint8_t countEmbeddedPins = 0;
    uint8_t countInputPins = 0;
    uint8_t countOutputPins = 0;
    for(auto prop: props) {
        if (!prop.is_valid()) {
            throw EngineError("gs::TypeClass::Create: invalid clsType (name = \"{}\"), has invalid property", clsName);
        }
        if (prop.get_name().empty()) {
            throw EngineError("gs::TypeClass::Create: invalid clsType (name = \"{}\"), has property with empty name", clsName);
        }

        if (prop.get_metadata(GSMetaTypes::GS_EMBEDDED_PROPERTY).is_valid()) {
            ++countEmbeddedPins;
        } else if (prop.get_metadata(GSMetaTypes::GS_INPUT_PIN).is_valid()) {
            ++countInputPins;
        } else if (prop.get_metadata(GSMetaTypes::GS_OUTPUT_PIN).is_valid()) {
            ++countOutputPins;
        } else {
            throw EngineError("gs::TypeClass::Create: invalid clsType (name = \"{}\"), property (name = \"{}\"), has no known metadata",
                clsName, prop.get_name().to_string());
        }
    }

    uint8_t embeddedIndex = 0;
    uint8_t inputIndex = embeddedIndex + countEmbeddedPins;
    uint8_t outputIndex = inputIndex + countInputPins;

    m_props = reinterpret_cast<rttr::property*>(malloc(props.size() * sizeof(rttr::property)));
    if (m_props == nullptr) {
        throw std::bad_alloc();
    }

    // filling fields after all checks
    m_countEmbeddedPins = countEmbeddedPins;
    m_countInputPins = countInputPins;
    m_countOutputPins = countOutputPins;
    m_clsType = clsType;
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
    auto name = m_clsType.get_name();
    return std::string_view(name.cbegin(), name.cend());
}

std::string_view TypeClass::GetPrettyName() const {
    // TODO: show real pretty name
    return GetName();
}

rttr::variant TypeClass::NewInstance() const {
    return m_clsType.create();
}

rttr::variant TypeClass::GetValue(uint8_t pinIndex, rttr::variant& instance) const {
    return m_props[pinIndex].get_value(instance);
}

void TypeClass::SetValue(uint8_t pinIndex, rttr::variant& instance, rttr::variant& value) const {
    m_props[pinIndex].set_value(instance, value);
}

}
