#include "middleware/gscheme/editor/gs_node_type.h"

#include <string>

#include "middleware/gscheme/rttr/type.h"
#include "middleware/gscheme/editor/gs_node.h"


struct GSNodeType::Impl {
    Impl(const rttr::type& gsType);

    std::string m_name;
    rttr::type m_gsType;
};

GSNodeType::Impl::Impl(const rttr::type& gsType)
    : m_name(gsType.get_name())
    , m_gsType(gsType) {

}

GSNodeType::GSNodeType(const rttr::type& gsType)
    : impl(gsType) {

}

GSNodeType::~GSNodeType() {

}

std::shared_ptr<GSNode> GSNodeType::NewInstance() {
    return std::make_shared<GSNode>(impl->m_name, impl->m_gsType.create());
}
