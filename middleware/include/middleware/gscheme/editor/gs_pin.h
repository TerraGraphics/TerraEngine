#pragma once

#include <string>
#include <memory>
#include <cstdint>

#include "core/common/ctor.h"


namespace rttr {
    class variant;
    class property;
}

class GSType;
class GSInputPin : Fixed {
public:
    GSInputPin() = delete;
    GSInputPin(uintptr_t id, rttr::variant& instance, const rttr::property& property);
    ~GSInputPin();

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
    GSOutputPin(uintptr_t id, const std::string& name);
    ~GSOutputPin();

    void Draw(uint8_t alpha) const;

private:
    uintptr_t m_id;
    std::string m_name;
    bool m_isConnected = false;
};
