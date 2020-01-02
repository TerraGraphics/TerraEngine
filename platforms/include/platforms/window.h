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
    Undefined = 8,

    LastStandartCursor = ResizeV,
};


class RenderWindow {
public:
    RenderWindow() = delete;
    RenderWindow(const std::shared_ptr<WindowEventsHandler>& handler);
    virtual ~RenderWindow() = default;

    uint16_t GetMonitorWidth() const noexcept { return m_monitorWidth; }
    uint16_t GetMonitorHeight() const noexcept { return m_monitorHeight; }

    virtual void Destroy() = 0;
    virtual void SetTitle(const std::string& title) = 0;
    virtual void GetCursorPos(int& x, int& y) = 0;
    virtual void SetCursorPos(int x, int y) = 0;
    virtual void SetCursor(CursorType value) = 0;
    virtual void ProcessEvents() = 0;

protected:
    uint16_t m_monitorWidth = 0;
    uint16_t m_monitorHeight = 0;
    std::shared_ptr<WindowEventsHandler> m_eventHandler;
};
