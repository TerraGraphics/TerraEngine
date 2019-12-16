#pragma once

#include <X11/Xlib.h>
#include "window.h"


class WindowGLLinux : public Window {
public:
    WindowGLLinux() = default;
    ~WindowGLLinux() override;

    Window GetWindow() noexcept { return m_window; }
    Display* GetDisplay() noexcept { return m_display; }

    void Create(int16_t posX, int16_t posY, uint16_t width, uint16_t height, const std::string& title);
    void Destroy() override;

private:
    Window m_window = 0;
    Display* m_display = nullptr;
};
