#pragma once

#include <memory>
#include <string>
#include <cstdint>


enum class CursorType : uint8_t {
    Arrow = 0,
    TextInput = 1,
    ResizeAll = 2,
    ResizeNS = 3,
    ResizeEW = 4,
    ResizeNESW = 5,
    ResizeNWSE = 6,
    Hand = 7,
    NotAllowed = 8,
    Empty = 9,

    Disabled = 10,
    Undefined = 11,

    CountStandartCursor = NotAllowed + 1,
    CountRealCursor = Empty + 1,
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

class WindowEventsHandler;
class RenderWindow {
public:
    RenderWindow() = delete;
    RenderWindow(const WindowDesc& desc, const std::shared_ptr<WindowEventsHandler>& handler);
    virtual ~RenderWindow() = default;

    uint16_t GetMonitorWidth() const noexcept { return m_monitorWidth; }
    uint16_t GetMonitorHeight() const noexcept { return m_monitorHeight; }

    virtual void* GetNativeWindowHandler() const = 0;
    virtual void SetTitle(const char* text) = 0;

    virtual void SetClipboard(const char* text) = 0;
    virtual const char* GetClipboard() = 0;

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
    CursorType m_currentCursorType = CursorType::Empty;
    std::shared_ptr<WindowEventsHandler> m_eventHandler;
};
