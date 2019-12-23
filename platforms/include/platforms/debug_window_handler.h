#pragma once

#include "platforms/window_events.h"


class DebugWindowEventsHandler : public WindowEventsHandler {
public:
    DebugWindowEventsHandler() = default;
    ~DebugWindowEventsHandler() override = default;

    void OnKeyEvent(KeyAction action, Key key, uint8_t modifiers) override;
    void OnCursorPosition(int64_t x, int64_t y) override;
    void OnScroll(int delta) override;
};
