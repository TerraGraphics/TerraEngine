#include "debug_window_handler.h"

#include <iostream>


void DebugWindowEventsHandler::OnKeyEvent(KeyAction action, Key key, uint8_t modifiers) {
    std::string delimiter;
    std::string strModifiers;
    if (modifiers & KeyModifier::Shift) {
        strModifiers += delimiter + ToString(KeyModifier::Shift);
        delimiter = "+";
    }
    if (modifiers & KeyModifier::Control) {
        strModifiers += delimiter + ToString(KeyModifier::Control);
        delimiter = "+";
    }
    if (modifiers & KeyModifier::Alt) {
        strModifiers += delimiter + ToString(KeyModifier::Alt);
        delimiter = "+";
    }
    if (modifiers & KeyModifier::Super) {
        strModifiers += delimiter + ToString(KeyModifier::Super);
        delimiter = "+";
    }
    if (!strModifiers.empty()) {
        strModifiers += "+";
    }

    std::cout << ToString(action) << " " << strModifiers << ToString(key) << std::endl;
}