#pragma once

#include <memory>
#include "window_events.h"


class RenderWindow {
public:
    RenderWindow() = delete;
    RenderWindow(const std::shared_ptr<WindowEventsHandler>& handler);
    virtual ~RenderWindow() = default;

    virtual void Destroy() = 0;
    virtual void SetTitle(const std::string& title) = 0;
    virtual void ProcessEvents() = 0;

protected:
    std::shared_ptr<WindowEventsHandler> m_eventHandler;
};
