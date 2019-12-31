#pragma once

#include "platforms/window.h"


struct xcb_screen_t;
struct xcb_intern_atom_reply_t;
typedef struct xcb_connection_t xcb_connection_t;
typedef struct _XCBKeySymbols xcb_key_symbols_t;
typedef struct xcb_cursor_context_t xcb_cursor_context_t;
class WindowVulkanLinux : public RenderWindow {
public:
    WindowVulkanLinux() = delete;
    WindowVulkanLinux(const std::shared_ptr<WindowEventsHandler>& handler);
    ~WindowVulkanLinux() override;

    uint32_t GetWindow() noexcept { return m_window; }
    xcb_connection_t* GetConnection() noexcept { return m_connection; }

    void Create(int16_t posX, int16_t posY, uint16_t width, uint16_t height, int screenNumber, const std::string& name);
    void Destroy() override;

    void SetTitle(const std::string& title) override;
    void SetCursor(CursorType value) override;
    void ProcessEvents() override;

private:
    void CreateCursors();
    void DestroyCursors();
    void DisableCursor();
    void EnableCursor();
    void GetCursorPos(int& x, int& y);
    void SetCursorPos(int x, int y);
    void HandleSizeEvent(uint32_t width, uint32_t height);
    void HandleKeyEvent(KeyAction action, uint8_t code, uint state);
    void HandleMouseButtonEvent(KeyAction action, uint8_t code, uint state);

private:
    uint32_t m_window = 0;
    uint32_t m_cursors[static_cast<uint>(CursorType::LastStandartCursor) + 1] = { 0 };
    uint32_t m_hiddenCursor = 0;
    CursorType m_currentCursorType = CursorType::Disabled;
    int m_windowCenterX = 0;
    int m_windowCenterY = 0;
    int m_visibleCursorPosX = 0;
    int m_visibleCursorPosY = 0;
    int m_lastCursorPosX = 0;
    int m_lastCursorPosY = 0;
    double m_virtualCursorX = 0.0;
    double m_virtualCursorY = 0.0;
    xcb_key_symbols_t *m_keySymbols = nullptr;
    xcb_connection_t* m_connection = nullptr;
    xcb_screen_t* m_screen = nullptr;
    xcb_intern_atom_reply_t* m_atomWMDeleteWindow = nullptr;
    xcb_cursor_context_t *m_cursorContext = nullptr;
};
