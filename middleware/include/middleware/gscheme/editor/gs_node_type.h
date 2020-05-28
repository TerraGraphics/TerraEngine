#pragma once

#include <vector>
#include <string_view>

#include "core/common/ctor.h"
#include "core/common/pimpl.h"


namespace rttr {
    class type;
    class property;
}

class GSNodeType : Fixed {
public:
    GSNodeType() = delete;
    GSNodeType(const rttr::type& gsType);
    ~GSNodeType();

    std::string_view GetName() const noexcept;
    const rttr::type& GetType() const noexcept;

    const std::vector<rttr::property>& GetEmbeddedProps() const noexcept;
    const std::vector<rttr::property>& GetInputPinProps() const noexcept;
    const std::vector<rttr::property>& GetOutputPinProps() const noexcept;

private:
    struct Impl;
    Pimpl<Impl, 112, 8> impl;
};
