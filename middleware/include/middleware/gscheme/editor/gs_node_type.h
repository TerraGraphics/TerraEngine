#pragma once

#include <string_view>

#include "core/common/ctor.h"
#include "core/common/pimpl.h"


namespace rttr {
    class type;
}

class GSNodeType : Fixed {
public:
    GSNodeType() = delete;
    GSNodeType(std::string_view name, const rttr::type& gsType);
    ~GSNodeType();

    std::string_view GetName() const noexcept;
    const rttr::type& GetType() const noexcept;

private:
    struct Impl;
    Pimpl<Impl, 40, 8> impl;
};
