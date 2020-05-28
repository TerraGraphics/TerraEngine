#include "middleware/gscheme/editor/gs_node_type.h"

#include <string>

#include "middleware/gscheme/rttr/type.h"


struct GSNodeType::Impl {
    Impl(std::string_view name, const rttr::type& gsType);

    std::string m_name;
    rttr::type m_gsType;
};

GSNodeType::Impl::Impl(std::string_view name, const rttr::type& gsType)
    : m_name(name)
    , m_gsType(gsType) {

}

GSNodeType::GSNodeType(std::string_view name, const rttr::type& gsType)
    : impl(name, gsType) {

}

GSNodeType::~GSNodeType() {

}

std::string_view GSNodeType::GetName() const noexcept {
    return impl->m_name;
}

const rttr::type& GSNodeType::GetType() const noexcept {
    return impl->m_gsType;
}
