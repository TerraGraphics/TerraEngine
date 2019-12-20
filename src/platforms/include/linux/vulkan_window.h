#pragma once

#include <xcb/xcb.h>
#include "window.h"


typedef struct _XCBKeySymbols xcb_key_symbols_t;
class WindowVulkanLinux : public RenderWindow {
public:
    WindowVulkanLinux() = default;
    ~WindowVulkanLinux() override;

    uint32_t GetWindow() noexcept { return m_window; }
    xcb_connection_t* GetConnection() noexcept { return m_connection; }
    xcb_intern_atom_reply_t* GetAtomWMDeleteWindow() noexcept { return m_atomWMDeleteWindow; }

    void Create(int16_t posX, int16_t posY, uint16_t width, uint16_t height, int screenNumber, const std::string& title);
    void Destroy() override;

    void ProcessEvents() override;

private:
    void HandleKeyEvent(KeyAction action, xcb_keycode_t code, uint state);
    void HandleMouseButtonEvent(KeyAction action, xcb_button_t code, uint state);

private:
    uint32_t m_window = 0;
    xcb_key_symbols_t *m_keySymbols = nullptr;
    xcb_connection_t* m_connection = nullptr;
    xcb_intern_atom_reply_t* m_atomWMDeleteWindow = nullptr;
};
