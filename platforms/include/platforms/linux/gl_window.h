#pragma once

#include "platforms/window.h"


typedef struct _XDisplay Display;
typedef union _XEvent XEvent;
class X11InputHandler;
class WindowGLLinux : public RenderWindow {
public:
    WindowGLLinux() = delete;
    WindowGLLinux(const WindowDesc& desc, const std::shared_ptr<WindowEventsHandler>& handler);
    ~WindowGLLinux() override;

    Display* GetDisplay() noexcept { return m_display; }

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

private:
    void GetAtoms();
    void CreateCursors();
    void DestroyCursors();
    void DisableCursor();
    void EnableCursor();
    void HandleSizeEvent(uint32_t width, uint32_t height);
    void HandleKeyEvent(KeyAction action, uint code, uint state);
    void HandleMouseButtonEvent(KeyAction action, uint code, uint state);
    std::string HandleSelectionNotify(const XEvent* event);

private:
    Display* m_display = nullptr;
    uint64_t m_window = 0;
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
    uint64_t* m_atoms = nullptr;
    bool m_focused = true;
};
