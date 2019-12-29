#pragma once

#include "platforms/window.h"


typedef struct _XDisplay Display;
class WindowGLLinux : public RenderWindow {
public:
    WindowGLLinux() = delete;
    WindowGLLinux(const std::shared_ptr<WindowEventsHandler>& handler);
    ~WindowGLLinux() override;

    uint32_t GetWindow() noexcept { return m_window; }
    Display* GetDisplay() noexcept { return m_display; }

    void Create(int16_t posX, int16_t posY, uint16_t width, uint16_t height, const std::string& name);
    void Destroy() override;

    void SetTitle(const std::string& title) override;
    void SetCursor(CursorType value) override;
    void ProcessEvents() override;

private:
    void CreateCursors();
    void DestroyCursors();
    void HandleKeyEvent(KeyAction action, uint code, uint state);
    void HandleMouseButtonEvent(KeyAction action, uint code, uint state);

private:
    uint32_t m_window = 0;
    uint32_t m_cursors[static_cast<uint>(CursorType::LastStandartCursor) + 1] = { 0 };
    uint32_t m_hiddenCursor = 0;
    CursorType m_currentCursorType = CursorType::Disabled;
    Display* m_display = nullptr;
    uint64_t m_atomWMDeleteWindow = 0;
};
