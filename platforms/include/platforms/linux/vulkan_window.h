#pragma once

#include "platforms/window.h"


struct xcb_intern_atom_reply_t;
typedef struct xcb_connection_t xcb_connection_t;
typedef struct _XCBKeySymbols xcb_key_symbols_t;
class WindowVulkanLinux : public RenderWindow {
public:
    WindowVulkanLinux() = default;
    ~WindowVulkanLinux() override;

    uint32_t GetWindow() noexcept { return m_window; }
    xcb_connection_t* GetConnection() noexcept { return m_connection; }

    void Create(int16_t posX, int16_t posY, uint16_t width, uint16_t height, int screenNumber, const std::string& name);
    void Destroy() override;

    void SetTitle(const std::string& title) override;
    void ProcessEvents() override;

private:
    void HandleKeyEvent(KeyAction action, uint8_t code, uint state);
    void HandleMouseButtonEvent(KeyAction action, uint8_t code, uint state);

private:
    uint32_t m_window = 0;
    xcb_key_symbols_t *m_keySymbols = nullptr;
    xcb_connection_t* m_connection = nullptr;
    xcb_intern_atom_reply_t* m_atomWMDeleteWindow = nullptr;
};
