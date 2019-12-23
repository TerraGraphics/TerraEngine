#include "platforms/window_events.h"


std::string ToString(Key value) {
    switch (value) {
        case Key::F1: return "F1";
        case Key::F2: return "F2";
        case Key::F3: return "F3";
        case Key::F4: return "F4";
        case Key::F5: return "F5";
        case Key::F6: return "F6";
        case Key::F7: return "F7";
        case Key::F8: return "F8";
        case Key::F9: return "F9";
        case Key::F10: return "F10";
        case Key::F11: return "F11";
        case Key::F12: return "F12";
        case Key::F13: return "F13";
        case Key::F14: return "F14";
        case Key::F15: return "F15";
        case Key::F16: return "F16";
        case Key::F17: return "F17";
        case Key::F18: return "F18";
        case Key::F19: return "F19";
        case Key::F20: return "F20";
        case Key::F21: return "F21";
        case Key::F22: return "F22";
        case Key::F23: return "F23";
        case Key::F24: return "F24";
        case Key::F25: return "F25";
        case Key::Escape: return "Escape";
        case Key::Enter: return "Enter";
        case Key::Tab: return "Tab";
        case Key::Backspace: return "Backspace";
        case Key::CapsLock: return "CapsLock";
        case Key::ScrollLock: return "ScrollLock";
        case Key::NumLock: return "NumLock";
        case Key::Space: return "Space";
        case Key::Insert: return "Insert";
        case Key::Delete: return "Delete";
        case Key::PageUp: return "PageUp";
        case Key::PageDown: return "PageDown";
        case Key::Home: return "Home";
        case Key::End: return "End";
        case Key::Apostrophe: return "Apostrophe";
        case Key::ArrowRight: return "ArrowRight";
        case Key::ArrowLeft: return "ArrowLeft";
        case Key::ArrowDown: return "ArrowDown";
        case Key::ArrowUp: return "ArrowUp";
        case Key::Comma: return "Comma";
        case Key::Minus: return "Minus";
        case Key::Period: return "Period";
        case Key::Slash: return "Slash";
        case Key::Digit0: return "Digit0";
        case Key::Digit1: return "Digit1";
        case Key::Digit2: return "Digit2";
        case Key::Digit3: return "Digit3";
        case Key::Digit4: return "Digit4";
        case Key::Digit5: return "Digit5";
        case Key::Digit6: return "Digit6";
        case Key::Digit7: return "Digit7";
        case Key::Digit8: return "Digit8";
        case Key::Digit9: return "Digit9";
        case Key::PrintScreen: return "PrintScreen";
        case Key::Semicolon: return "Semicolon";
        case Key::Pause: return "Pause";
        case Key::Equal: return "Equal";
        case Key::NonUS1: return "NonUS1";
        case Key::NonUS2: return "NonUS2";
        case Key::Menu: return "Menu";
        case Key::A: return "A";
        case Key::B: return "B";
        case Key::C: return "C";
        case Key::D: return "D";
        case Key::E: return "E";
        case Key::F: return "F";
        case Key::G: return "G";
        case Key::H: return "H";
        case Key::I: return "I";
        case Key::J: return "J";
        case Key::K: return "K";
        case Key::L: return "L";
        case Key::M: return "M";
        case Key::N: return "N";
        case Key::O: return "O";
        case Key::P: return "P";
        case Key::Q: return "Q";
        case Key::R: return "R";
        case Key::S: return "S";
        case Key::T: return "T";
        case Key::U: return "U";
        case Key::V: return "V";
        case Key::W: return "W";
        case Key::X: return "X";
        case Key::Y: return "Y";
        case Key::Z: return "Z";
        case Key::LeftBracket: return "LeftBracket";
        case Key::Backslash: return "Backslash";
        case Key::RightBracket: return "RightBracket";
        case Key::GraveAccent: return "GraveAccent";
        case Key::KeyPad0: return "KeyPad0";
        case Key::KeyPad1: return "KeyPad1";
        case Key::KeyPad2: return "KeyPad2";
        case Key::KeyPad3: return "KeyPad3";
        case Key::KeyPad4: return "KeyPad4";
        case Key::KeyPad5: return "KeyPad5";
        case Key::KeyPad6: return "KeyPad6";
        case Key::KeyPad7: return "KeyPad7";
        case Key::KeyPad8: return "KeyPad8";
        case Key::KeyPad9: return "KeyPad9";
        case Key::KeyPadDecimal: return "KeyPadDecimal";
        case Key::KeyPadDivide: return "KeyPadDivide";
        case Key::KeyPadMultiply: return "KeyPadMultiply";
        case Key::KeyPadSubtract: return "KeyPadSubtract";
        case Key::KeyPadAdd: return "KeyPadAdd";
        case Key::KeyPadEnter: return "KeyPadEnter";
        case Key::KeyPadEqual: return "KeyPadEqual";
        case Key::LeftShift: return "LeftShift";
        case Key::LeftControl: return "LeftControl";
        case Key::LeftAlt: return "LeftAlt";
        case Key::LeftSuper: return "LeftSuper";
        case Key::RightShift: return "RightShift";
        case Key::RightControl: return "RightControl";
        case Key::RightAlt: return "RightAlt";
        case Key::RightSuper: return "RightSuper";
        case Key::Shift: return "Shift";
        case Key::Control: return "Control";
        case Key::Alt: return "Alt";
        case Key::Super: return "Super";
        case Key::MouseLeft: return "MouseLeft";
        case Key::MouseRight: return "MouseRight";
        case Key::MouseMiddle: return "MouseMiddle";
        case Key::Unknown:
        default: return "Unknown";
    }
}

std::string ToString(KeyModifier value) {
    switch (value) {
        case KeyModifier::Shift: return "Shift";
        case KeyModifier::Control: return "Control";
        case KeyModifier::Alt: return "Alt";
        case KeyModifier::Super: return "Super";
        default: return "Unknown";
    }
}

std::string ToString(KeyAction value) {
    switch (value) {
        case KeyAction::Release: return "Release";
        case KeyAction::Press: return "Press";
        default: return "Unknown";
    }
}
