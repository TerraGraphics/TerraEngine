#pragma once

#include <string>
#include <cstdint>
#include <string_view>

#include "core/common/ctor.h"
#include "middleware/gscheme/rttr/variant.h"


namespace gs {

class TypeClass : Fixed {
public:
    TypeClass() = default;
    ~TypeClass();

    void Create(const rttr::type& clsType);

    // unique class name
    std::string_view GetName() const;
    // name for UI
    std::string GetPrettyName() const;

    // unique pin name
    std::string_view GetPinName(uint8_t pinIndex) const;
    // pin name for UI
    std::string GetPrettyPinName(uint8_t pinIndex) const;

    uint8_t EmbeddedPinsCount() const noexcept { return m_countEmbeddedPins; }
    uint8_t InputPinsCount() const noexcept { return m_countInputPins; }
    uint8_t OutputPinsCount() const noexcept { return m_countOutputPins; }

    rttr::variant NewInstance() const;

    rttr::variant GetValue(uint8_t pinIndex, rttr::variant& instance) const;
    void SetValue(uint8_t pinIndex, rttr::variant& instance, const rttr::variant& value) const;

private:
    void CheckClassType(const rttr::type& clsType) const;

private:
    uint8_t m_countEmbeddedPins = 0;
    uint8_t m_countInputPins = 0;
    uint8_t m_countOutputPins = 0;
    rttr::property* m_props = nullptr;
    rttr::type m_clsType = rttr::type::get<bool>();
};

}
