#include "middleware/gscheme/graph/gs_class.h"

#include <new>
#include <cstddef>
#include <cstdlib>

#include "core/common/exception.h"
#include "middleware/gscheme/rttr/type.h"
#include "middleware/gscheme/rttr/variant.h"
#include "middleware/gscheme/graph/gs_limits.h"
#include "middleware/gscheme/reflection/gs_metadata.h"


namespace gs {

static_assert(sizeof(Class) == 48, "sizeof(Class) == 48 bytes");

Class::Class(const rttr::type& clsType) {
    if (!clsType.is_valid()) {
        throw EngineError("gs::Class::ctor: invalid clsType");
    }

    m_name = clsType.get_name().to_string();
    if (m_name.empty()) {
        throw EngineError("gs::Class::ctor: invalid clsType, type name is empty");
    }

    const auto props = clsType.get_properties();
    if (props.size() > static_cast<size_t>(MAX_PINS_COUNT)) {
        throw EngineError("gs::Class::ctor: invalid clsType (name = \"{}\"), max count properties = {}", m_name, MAX_PINS_COUNT);
    }

    for(auto prop: props) {
        if (!prop.is_valid()) {
            throw EngineError("gs::Class::ctor: invalid clsType (name = \"{}\"), has invalid property", m_name);
        }
        if (prop.get_name().empty()) {
            throw EngineError("gs::Class::ctor: invalid clsType (name = \"{}\"), has property with empty name", m_name);
        }

        if (prop.get_metadata(GSMetaTypes::GS_EMBEDDED_PROPERTY).is_valid()) {
            ++m_countEmbeddedPins;
        } else if (prop.get_metadata(GSMetaTypes::GS_INPUT_PIN).is_valid()) {
            ++m_countInputPins;
        } else if (prop.get_metadata(GSMetaTypes::GS_OUTPUT_PIN).is_valid()) {
            ++m_countOutputPins;
        } else {
            throw EngineError("gs::Class::ctor: invalid clsType (name = \"{}\"), property (name = \"{}\"), has no known metadata",
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

Class::~Class() {
    if (m_props != nullptr) {
        free(m_props);
    }
}

std::string_view Class::GetName() const {
    return m_name;
}

}
