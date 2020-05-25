#include "middleware/gscheme/editor/gs_node_type.h"

#include <string>

#include "middleware/gscheme/rttr/type.h"
#include "middleware/gscheme/editor/gs_node.h"


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

std::shared_ptr<GSNode> GSNodeType::NewInstance(uintptr_t id) {
    return std::make_shared<GSNode>(id, impl->m_name, impl->m_gsType.create());
}
