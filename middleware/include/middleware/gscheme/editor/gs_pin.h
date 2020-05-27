#pragma once

#include <string>
#include <cstdint>

#include "core/common/ctor.h"


class GSInputPin : Fixed {
public:
    GSInputPin() = delete;
    GSInputPin(uintptr_t id, const std::string& name);
    ~GSInputPin();

    void Draw(uint8_t alpha) const;

private:
    uintptr_t m_id;
    std::string m_name;
    bool m_isConnected = false;
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
