#pragma once

#include <memory>
#include "window_events.h"


class RenderWindow {
public:
    RenderWindow() = delete;
    RenderWindow(const std::shared_ptr<WindowEventsHandler>& handler);
    virtual ~RenderWindow() = default;

    bool IsWindowShouldClose() const noexcept { return m_windowShouldClose; }
    uint16_t GetWidth() const noexcept { return m_width; }
    uint16_t GetHeight() const noexcept { return m_height; }

    virtual void Destroy() = 0;

    virtual void SetTitle(const std::string& title) = 0;
    virtual void ProcessEvents() = 0;

protected:
    bool m_windowShouldClose = false;
    uint16_t m_width = 0;
    uint16_t m_height = 0;
    std::shared_ptr<WindowEventsHandler> m_eventHandler;
};
