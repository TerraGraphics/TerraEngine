#pragma once

#include <string>
#include <X11/Xlib.h>


class WindowGLLinux {
public:
    WindowGLLinux() = default;
    ~WindowGLLinux() = default;

    void Create(int16_t posX, int16_t posY, uint16_t width, uint16_t height, const std::string& title);
    void Destroy();

    Window GetWindow() noexcept { return m_window; }
    Display* GetDisplay() noexcept { return m_display; }

private:
    uint16_t m_width = 0;
    uint16_t m_height = 0;
    Window m_window = 0;
    Display* m_display = nullptr;
};