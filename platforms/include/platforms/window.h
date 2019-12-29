#pragma once

#include <memory>
#include "window_events.h"

enum class CursorType : uint8_t {
    Arrow = 0,
    IBeam = 1,
    Crosshair = 2,
    Hand = 3,
    ResizeH = 4,
    ResizeV = 5,

    Hidden = 6,
    Disabled = 7,

    LastStandartCursor = ResizeV,
};


class RenderWindow {
public:
    RenderWindow() = delete;
    RenderWindow(const std::shared_ptr<WindowEventsHandler>& handler);
    virtual ~RenderWindow() = default;

    virtual void Destroy() = 0;
    virtual void SetTitle(const std::string& title) = 0;
    virtual void SetCursor(CursorType value) = 0;
    virtual void ProcessEvents() = 0;

protected:
    std::shared_ptr<WindowEventsHandler> m_eventHandler;
};
