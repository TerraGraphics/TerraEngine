#pragma once

#include <string>
#include <memory>
#include "window_events.h"


class RenderWindow {
public:
    RenderWindow() = default;
    virtual ~RenderWindow() = default;

    bool IsWindowShouldClose() const noexcept { return m_windowShouldClose; }
    uint16_t GetWidth() const noexcept { return m_width; }
    uint16_t GetHeight() const noexcept { return m_height; }

    void SetEventHandler(std::unique_ptr<WindowEventsHandler>&& handler);
    virtual void Destroy() = 0;

    virtual void ProcessEvents() = 0;
protected:
    bool m_windowShouldClose = false;
    uint16_t m_width = 0;
    uint16_t m_height = 0;
    std::unique_ptr<WindowEventsHandler> m_eventHandler;
};
