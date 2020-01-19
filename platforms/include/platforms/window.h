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

struct WindowDesc {
    enum class Position : uint8_t {
        Absolute = 0,
        CenterOfScreen = 1,
        Default = 2,
    };
    int16_t positionX = 0; // for Position::Absolute
    int16_t positionY = 0; // for Position::Absolute
    Position positionType = Position::Default;

    enum class Size : uint8_t {
        Absolute = 0,
        PercentOfScreen = 1,
        Default = 2,
    };
    uint16_t width = 0; // for Size::Absolute
    uint16_t height = 0; // for Size::Absolute
    float widthPercent = 0.0f; // for Size::PercentOfScreen
    float heightPercent = 0.0f; // for Size::PercentOfScreen
    Size sizeType = Size::Default;

    uint16_t minWidth = 320;
    uint16_t minHeight = 240;

    // application name
    std::string name;
};

class RenderWindow {
public:
    RenderWindow() = delete;
    RenderWindow(const WindowDesc& desc, const std::shared_ptr<WindowEventsHandler>& handler);
    virtual ~RenderWindow() = default;

    uint16_t GetMonitorWidth() const noexcept { return m_monitorWidth; }
    uint16_t GetMonitorHeight() const noexcept { return m_monitorHeight; }

    virtual void* GetNativeWindowHandler() const = 0;
    virtual void SetTitle(const std::string& title) = 0;

    virtual void SetClipboard(const std::string& string) = 0;
    virtual std::string GetClipboard() = 0;

    virtual void GetCursorPos(int& x, int& y) = 0;
    virtual void SetCursorPos(int x, int y) = 0;

    CursorType GetCursor() const noexcept { return m_currentCursorType; }
    virtual void SetCursor(CursorType value) = 0;

    virtual void Create() = 0;
    virtual void Destroy() = 0;
    virtual void ProcessEvents() = 0;

protected:
    void SetMonitorSize(uint16_t monitorWidth, uint16_t monitorHeight);

protected:
    WindowDesc m_desc;
    uint16_t m_monitorWidth = 0;
    uint16_t m_monitorHeight = 0;
    CursorType m_currentCursorType = CursorType::Hidden;
    std::shared_ptr<WindowEventsHandler> m_eventHandler;
};
