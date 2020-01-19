#pragma once

#include <memory>
#include <cstdint>
#include <X11/Xlib.h>

class WindowEventsHandler;
class X11InputHandler {
public:
    X11InputHandler() = delete;
    X11InputHandler(const std::shared_ptr<WindowEventsHandler>& handler);
    ~X11InputHandler();

    void Create(Display* display, Window window);
    void Handle(XKeyEvent* xKeyEvent);
    void FocusChange(bool isFocus);

private:
    // XIM input method
    XIM m_im = nullptr;
    XIC m_ic = nullptr;

    std::shared_ptr<WindowEventsHandler> m_handler;
};
