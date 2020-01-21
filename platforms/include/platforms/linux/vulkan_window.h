#pragma once

#include <vector>
#include <xcb/xcb.h>
#include "platforms/window.h"


typedef struct _XDisplay Display;
typedef struct _XCBKeySymbols xcb_key_symbols_t;
typedef struct xcb_cursor_context_t xcb_cursor_context_t;
class X11InputHandler;
class WindowVulkanLinux : public RenderWindow {
public:
    WindowVulkanLinux() = delete;
    WindowVulkanLinux(const WindowDesc& desc, const std::shared_ptr<WindowEventsHandler>& handler);
    ~WindowVulkanLinux() override;

    Display* GetDisplay() noexcept { return m_display; }

    uint32_t GetWindow() noexcept { return m_window; }
    xcb_connection_t* GetConnection() noexcept { return m_connection; }

    void* GetNativeWindowHandler() const override { return reinterpret_cast<void*>(m_window); };
    void SetTitle(const std::string& title) override;

    void SetClipboard(const std::string& string) override;
    std::string GetClipboard() override;

    void GetCursorPos(int& x, int& y) override;
    void SetCursorPos(int x, int y) override;
    void SetCursor(CursorType value) override;

    void Create() override;
    void Destroy() override;
    void ProcessEvents() override;
    void ProcessEvent(xcb_generic_event_t* event);

private:
    void GetAtoms();
    void CreateCursors();
    void DestroyCursors();
    void DisableCursor();
    void EnableCursor();
    void HandleSizeEvent(uint32_t width, uint32_t height);
    void HandleKeyEvent(KeyAction action, uint8_t code, uint state);
    void HandleMouseButtonEvent(KeyAction action, uint8_t code, uint state);
    std::string HandleSelectionNotify(const xcb_selection_notify_event_t* event);

private:
    Display* m_display = nullptr;
    uint32_t m_window = 0;
    X11InputHandler* m_inputParser = nullptr;
    uint32_t m_cursors[static_cast<uint>(CursorType::LastStandartCursor) + 1] = { 0 };
    uint32_t m_hiddenCursor = 0;
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
    uint32_t* m_atoms = nullptr;
    std::vector<xcb_generic_event_t*> m_events;
    xcb_cursor_context_t *m_cursorContext = nullptr;
    bool m_focused = true;
};
