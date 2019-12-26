#pragma once

#include "platforms/window_events.h"


class DebugWindowEventsHandler : public WindowEventsHandler {
public:
    DebugWindowEventsHandler() = default;
    ~DebugWindowEventsHandler() override = default;

    void Update() override;
    void OnWindowSizeEvent(uint32_t width, uint32_t height) override;
    void OnKeyEvent(KeyAction action, Key code, uint8_t modifiers) override;
    void OnCharEvent(char16_t ch) override;
    void OnCursorPosition(double x, double y) override;
    void OnScroll(int delta) override;
};
