#pragma once

#include <memory>
#include <cstdint>
#include <vector>

#include "core/common/ctor.h"


namespace rttr {
    class variant;
    class property;
}

class GSNode;
class GSType;
class GSInputPin : Fixed {
public:
    GSInputPin() = delete;
    GSInputPin(uintptr_t id, rttr::variant& instance, const rttr::property& property);
    ~GSInputPin();

    bool IsConnected() const noexcept { return m_isConnected; }
    void SetConnected(bool value);

    void SetValue(const rttr::variant& value);

    void Draw(uint8_t alpha) const;
    void DrawEditGui();

private:
    uintptr_t m_id;
    bool m_isConnected = false;
    std::shared_ptr<GSType> m_type;
};

class GSOutputPin : Fixed {
    struct Attachment {
        uint8_t pinNumber;
        std::shared_ptr<GSNode> node;
    };
public:
    GSOutputPin() = delete;
    GSOutputPin(uintptr_t id, rttr::variant& instance, const rttr::property& property);
    ~GSOutputPin();

    void Attach(uint8_t pinNumber, const std::shared_ptr<GSNode>& node);

    void Draw(uint8_t alpha) const;

private:
    uintptr_t m_id;
    std::shared_ptr<GSType> m_type;
    std::vector<Attachment> m_attachments;
};
