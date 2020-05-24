#include "middleware/gscheme/editor/gs_node_type.h"

#include "middleware/gscheme/rttr/type.h"


GSNodeType::GSNodeType(const rttr::type& gsType)
    : m_name(gsType.get_name()) {

}

GSNodeType::~GSNodeType() {

}
