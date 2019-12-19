#pragma once

#include "window_events.h"


class DebugWindowEventsHandler : public WindowEventsHandler {
public:
    DebugWindowEventsHandler() = default;
    ~DebugWindowEventsHandler() override = default;

    void OnKeyEvent(KeyAction action, Key key, uint8_t modifiers) override;
};
