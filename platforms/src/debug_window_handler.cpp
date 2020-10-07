#include "platforms/debug_window_handler.h"

#include <iostream>


void DebugWindowEventsHandler::OnNewFrame() {
    DefaultWindowEventsHandler::OnNewFrame();
}

void DebugWindowEventsHandler::OnWindowDestroy() {
    DefaultWindowEventsHandler::OnWindowDestroy();
    std::cout << "Window destroy" << std::endl;
}

void DebugWindowEventsHandler::OnWindowSizeEvent(uint32_t width, uint32_t height) {
    DefaultWindowEventsHandler::OnWindowSizeEvent(width, height);
    std::cout << "Window width = " << width <<", height = " << height << std::endl;
}

void DebugWindowEventsHandler::OnKeyEvent(KeyAction action, Key key, uint8_t modifiers) {
    DefaultWindowEventsHandler::OnKeyEvent(action, key, modifiers);
    bool noPrintModifiers = (key >= Key::FirstMod && key <= Key::LastMod);
    std::string delimiter;
    std::string strModifiers;
    if (!noPrintModifiers && (modifiers & KeyModifier::Shift)) {
        strModifiers += delimiter + ToString(KeyModifier::Shift);
        delimiter = "+";
    }
    if (!noPrintModifiers && (modifiers & KeyModifier::Control)) {
        strModifiers += delimiter + ToString(KeyModifier::Control);
        delimiter = "+";
    }
    if (!noPrintModifiers && (modifiers & KeyModifier::Alt)) {
        strModifiers += delimiter + ToString(KeyModifier::Alt);
        delimiter = "+";
    }
    if (!noPrintModifiers && (modifiers & KeyModifier::Super)) {
        strModifiers += delimiter + ToString(KeyModifier::Super);
        delimiter = "+";
    }

    std::cout << ToString(action) << " " << strModifiers << delimiter << ToString(key) << std::endl;
}

void DebugWindowEventsHandler::OnInputEvent(const std::wstring& s) {
    DefaultWindowEventsHandler::OnInputEvent(s);
    std::wcout << "input = " << s << std::endl;
}

void DebugWindowEventsHandler::OnCursorPosition(double x, double y) {
    DefaultWindowEventsHandler::OnCursorPosition(x, y);
    std::cout << "Cursor x = " << x <<", y = " << y << std::endl;
}

void DebugWindowEventsHandler::OnScroll(int delta) {
    DefaultWindowEventsHandler::OnScroll(delta);
    std::cout << "Scroll delta = " << delta << std::endl;
}

void DebugWindowEventsHandler::OnFocusChange(bool isFocus) {
    DefaultWindowEventsHandler::OnFocusChange(isFocus);
    std::cout << "is focused = " << isFocus << std::endl;
}
