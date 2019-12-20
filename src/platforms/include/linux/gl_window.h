#pragma once

#include "window.h"


typedef struct _XDisplay Display;
class WindowGLLinux : public RenderWindow {
public:
    WindowGLLinux() = default;
    ~WindowGLLinux() override;

    uint32_t GetWindow() noexcept { return m_window; }
    Display* GetDisplay() noexcept { return m_display; }

    void Create(int16_t posX, int16_t posY, uint16_t width, uint16_t height, const std::string& title);
    void Destroy() override;

    void ProcessEvents() override;

private:
    void HandleKeyEvent(KeyAction action, uint code, uint state);
    void HandleMouseButtonEvent(KeyAction action, uint code, uint state);

private:
    uint32_t m_window = 0;
    Display* m_display = nullptr;
    uint64_t m_atomWMDeleteWindow = 0;
};
