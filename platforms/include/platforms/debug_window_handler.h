#pragma once

#include "platforms/default_window_handler.h"


class DebugWindowEventsHandler : public DefaultWindowEventsHandler {
public:
    DebugWindowEventsHandler() = default;
    ~DebugWindowEventsHandler() override = default;

public:
    void OnNewFrame() override;
    void OnWindowDestroy() override;
    void OnWindowSizeEvent(uint32_t width, uint32_t height) override;
    void OnKeyEvent(KeyAction action, Key code, uint8_t modifiers) override;
    void OnInputEvent(const std::wstring& s) override;
    void OnCursorPosition(double x, double y) override;
    void OnScroll(int delta) override;
    void OnFocusChange(bool isFocus) override;
};
