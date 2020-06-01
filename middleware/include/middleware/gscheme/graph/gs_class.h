#pragma once

#include <string>
#include <cstdint>
#include <string_view>

#include "core/common/ctor.h"


namespace rttr {
    class type;
    class property;
}

namespace gs {

class Class : Fixed {
public:
    Class() = delete;
    explicit Class(const rttr::type& clsType);
    ~Class();

    std::string_view GetName() const;

private:
    uint8_t m_countEmbeddedPins = 0;
    uint8_t m_countInputPins = 0;
    uint8_t m_countOutputPins = 0;
    rttr::property* m_props = nullptr;
    std::string m_name;
};

}
