#pragma once

#include <string>
#include <xcb/xcb.h>


class WindowVulkanLinux {
public:
    WindowVulkanLinux() = default;
    ~WindowVulkanLinux() = default;

    void Create(int16_t posX, int16_t posY, uint16_t width, uint16_t height, int screenNumber, const std::string& title);
    void Destroy();

    void Process(const xcb_generic_event_t* event);

    uint16_t GetWidth() noexcept { return m_width; }
    uint16_t GetHeight() noexcept { return m_height; }
    uint32_t GetWindow() noexcept { return m_window; }
    xcb_connection_t* GetConnection() noexcept { return m_connection; }
    xcb_intern_atom_reply_t* GetAtomWMDeleteWindow() noexcept { return m_atomWMDeleteWindow; }

private:
    uint16_t m_width;
    uint16_t m_height;
    uint32_t m_window = 0;
    xcb_connection_t* m_connection = nullptr;
    xcb_intern_atom_reply_t* m_atomWMDeleteWindow = nullptr;
};
