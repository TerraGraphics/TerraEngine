#pragma once

#include <string>


class RenderWindow {
public:
    RenderWindow() = default;
    virtual ~RenderWindow() = default;

    uint16_t GetWidth() noexcept { return m_width; }
    uint16_t GetHeight() noexcept { return m_height; }

    virtual void Destroy() = 0;

protected:
    uint16_t m_width = 0;
    uint16_t m_height = 0;
};
