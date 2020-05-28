#include "middleware/gscheme/editor/gs_node_type.h"

#include <string>

#include "core/common/exception.h"
#include "middleware/gscheme/rttr/type.h"
#include "middleware/gscheme/reflection/gs_metadata.h"


struct GSNodeType::Impl {
    Impl(const rttr::type& gsType);

    std::string m_name;
    const rttr::type m_gsType;
    std::vector<rttr::property> m_embeddedProps;
    std::vector<rttr::property> m_inputPinProps;
    std::vector<rttr::property> m_outputPinProps;
};

GSNodeType::Impl::Impl(const rttr::type& gsType)
    : m_gsType(gsType) {

    if (!gsType.is_valid()) {
        throw EngineError("GSNodeType: invalid type \"{}\"", m_name);
    }

    m_name = gsType.get_name().to_string();

    for(auto prop: gsType.get_properties()) {
        if (!prop.is_valid()) {
            throw EngineError("GSNodeType: invalid type \"{}\" - has invalid property", m_name);
        }
        if (prop.get_metadata(GSMetaTypes::GS_EMBEDDED_PROPERTY).is_valid()) {
            m_embeddedProps.push_back(prop);
        } else if (prop.get_metadata(GSMetaTypes::GS_INPUT_PIN).is_valid()) {
            m_inputPinProps.push_back(prop);
        } else if (prop.get_metadata(GSMetaTypes::GS_OUTPUT_PIN).is_valid()) {
            m_outputPinProps.push_back(prop);
        } else {
            throw EngineError("GSNodeType: invalid type \"{}\", property \"{}\" - has unknown metadata type",
                m_name, prop.get_name().to_string());
        }
    }
}

GSNodeType::GSNodeType(const rttr::type& gsType)
    : impl(gsType) {

}

GSNodeType::~GSNodeType() {

}

std::string_view GSNodeType::GetName() const noexcept {
    return impl->m_name;
}

const rttr::type& GSNodeType::GetType() const noexcept {
    return impl->m_gsType;
}

const std::vector<rttr::property>& GSNodeType::GetEmbeddedProps() const noexcept {
    return impl->m_embeddedProps;
}

const std::vector<rttr::property>& GSNodeType::GetInputPinProps() const noexcept {
    return impl->m_inputPinProps;
}

const std::vector<rttr::property>& GSNodeType::GetOutputPinProps() const noexcept {
    return impl->m_outputPinProps;
}
