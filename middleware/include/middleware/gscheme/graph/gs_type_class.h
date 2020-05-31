#pragma once

#include <string>
#include <cstdint>
#include <string_view>

#include "core/common/ctor.h"


namespace rttr {
    class type;
    class variant;
    class instance;
    class property;
}

namespace gs {

class TypeClass : Fixed {
public:
    TypeClass() = default;
    ~TypeClass();

    void Create(const rttr::type& clsType);

    std::string_view GetName() const;
    std::string_view GetPrettyName() const;

    rttr::variant GetValue(uint8_t pinIndex, rttr::instance& instance) const;
    void SetValue(uint8_t pinIndex, rttr::instance& instance, rttr::variant& value) const;

private:
    uint8_t m_countEmbeddedPins = 0;
    uint8_t m_countInputPins = 0;
    uint8_t m_countOutputPins = 0;
    rttr::property* m_props = nullptr;
    std::string m_name;
};

}
