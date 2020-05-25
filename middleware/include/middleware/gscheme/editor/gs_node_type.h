#pragma once

#include <memory>

#include "core/common/ctor.h"
#include "core/common/pimpl.h"


namespace rttr {
    class type;
}

class GSNode;
class GSNodeType : Fixed {
public:
    GSNodeType() = delete;
    GSNodeType(const rttr::type& gsType);
    ~GSNodeType();

    std::shared_ptr<GSNode> NewInstance();

private:
    struct Impl;
    Pimpl<Impl, 40, 8> impl;
};
