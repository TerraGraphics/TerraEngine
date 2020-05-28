#pragma once

#include <memory>
#include <cstdint>

#include "core/common/ctor.h"
#include "middleware/gscheme/rttr/variant.h"


class GSType;
class GSInputPin : Fixed {
public:
    GSInputPin() = delete;
    GSInputPin(uintptr_t id, rttr::variant& instance, const rttr::property& property);
    ~GSInputPin();

    void SetValue(const rttr::variant& value);

    void Draw(uint8_t alpha) const;
    void DrawEditGui();

private:
    uintptr_t m_id;
    bool m_isConnected = false;
    std::shared_ptr<GSType> m_type;
};

class GSOutputPin : Fixed {
public:
    GSOutputPin() = delete;
    GSOutputPin(uintptr_t id, rttr::variant& instance, const rttr::property& property);
    ~GSOutputPin();

    rttr::variant GetValue() const;

    void Draw(uint8_t alpha) const;

private:
    uintptr_t m_id;
    bool m_isConnected = false;
    std::shared_ptr<GSType> m_type;
};
