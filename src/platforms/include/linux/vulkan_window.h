#pragma once

#include <xcb/xcb.h>
#include "window.h"


class WindowVulkanLinux : public RenderWindow {
public:
    WindowVulkanLinux() = default;
    ~WindowVulkanLinux() override;

    uint32_t GetWindow() noexcept { return m_window; }
    xcb_connection_t* GetConnection() noexcept { return m_connection; }
    xcb_intern_atom_reply_t* GetAtomWMDeleteWindow() noexcept { return m_atomWMDeleteWindow; }

    void Create(int16_t posX, int16_t posY, uint16_t width, uint16_t height, int screenNumber, const std::string& title);
    void Destroy() override;

    void Process(const xcb_generic_event_t* event);
private:
    uint32_t m_window = 0;
    xcb_connection_t* m_connection = nullptr;
    xcb_intern_atom_reply_t* m_atomWMDeleteWindow = nullptr;
};
