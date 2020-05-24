#pragma once

#include <string>

#include "core/common/ctor.h"


namespace rttr {
    class type;
}

class GSNodeType : Fixed {
public:
    GSNodeType() = delete;
    GSNodeType(const rttr::type& gsType);
    ~GSNodeType();

private:
    std::string m_name;
};
