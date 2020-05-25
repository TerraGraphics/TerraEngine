#pragma once

#include <memory>
#include <cstdint>
#include <string_view>

#include "core/common/ctor.h"
#include "core/common/pimpl.h"


namespace rttr {
    class type;
}

class GSNode;
class GSNodeType : Fixed {
public:
    GSNodeType() = delete;
    GSNodeType(std::string_view name, const rttr::type& gsType);
    ~GSNodeType();

    std::shared_ptr<GSNode> NewInstance(uintptr_t id);

private:
    struct Impl;
    Pimpl<Impl, 40, 8> impl;
};
