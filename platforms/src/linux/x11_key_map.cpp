#include "platforms/linux/x11_key_map.h"

#include <X11/X.h>
#include <X11/keysym.h>


Key KeySymToKey(uint32_t value) {
    switch (value) {
        case XK_F1:             return Key::F1;
        case XK_F2:             return Key::F2;
        case XK_F3:             return Key::F3;
        case XK_F4:             return Key::F4;
        case XK_F5:             return Key::F5;
        case XK_F6:             return Key::F6;
        case XK_F7:             return Key::F7;
        case XK_F8:             return Key::F8;
        case XK_F9:             return Key::F9;
        case XK_F10:            return Key::F10;
        case XK_F11:            return Key::F11;
        case XK_F12:            return Key::F12;
        case XK_F13:            return Key::F13;
        case XK_F14:            return Key::F14;
        case XK_F15:            return Key::F15;
        case XK_F16:            return Key::F16;
        case XK_F17:            return Key::F17;
        case XK_F18:            return Key::F18;
        case XK_F19:            return Key::F19;
        case XK_F20:            return Key::F20;
        case XK_F21:            return Key::F21;
        case XK_F22:            return Key::F22;
        case XK_F23:            return Key::F23;
        case XK_F24:            return Key::F24;
        case XK_F25:            return Key::F25;
        case XK_Escape:         return Key::Escape;
        case XK_Return:         return Key::Enter;
        case XK_Tab:            return Key::Tab;
        case XK_BackSpace:      return Key::Backspace;
        case XK_Caps_Lock:      return Key::CapsLock;
        case XK_Scroll_Lock:    return Key::ScrollLock;
        case XK_Num_Lock:       return Key::NumLock;
        case XK_space:          return Key::Space;
        case XK_Insert:         return Key::Insert;
        case XK_Delete:         return Key::Delete;
        case XK_Page_Up:        return Key::PageUp;
        case XK_Page_Down:      return Key::PageDown;
        case XK_Home:           return Key::Home;
        case XK_End:            return Key::End;
        case XK_apostrophe:     return Key::Apostrophe;
        case XK_Right:          return Key::ArrowRight;
        case XK_Left:           return Key::ArrowLeft;
        case XK_Down:           return Key::ArrowDown;
        case XK_Up:             return Key::ArrowUp;
        case XK_comma:          return Key::Comma;
        case XK_minus:          return Key::Minus;
        case XK_period:         return Key::Period;
        case XK_slash:          return Key::Slash;
        case XK_0:              return Key::Digit0;
        case XK_1:              return Key::Digit1;
        case XK_2:              return Key::Digit2;
        case XK_3:              return Key::Digit3;
        case XK_4:              return Key::Digit4;
        case XK_5:              return Key::Digit5;
        case XK_6:              return Key::Digit6;
        case XK_7:              return Key::Digit7;
        case XK_8:              return Key::Digit8;
        case XK_9:              return Key::Digit9;
        case XK_Print:          return Key::PrintScreen;
        case XK_semicolon:      return Key::Semicolon;
        case XK_Pause:          return Key::Pause;
        case XK_equal:          return Key::Equal;
        case XK_Menu:           return Key::Menu;
        case XK_a:              return Key::A;
        case XK_b:              return Key::B;
        case XK_c:              return Key::C;
        case XK_d:              return Key::D;
        case XK_e:              return Key::E;
        case XK_f:              return Key::F;
        case XK_g:              return Key::G;
        case XK_h:              return Key::H;
        case XK_i:              return Key::I;
        case XK_j:              return Key::J;
        case XK_k:              return Key::K;
        case XK_l:              return Key::L;
        case XK_m:              return Key::M;
        case XK_n:              return Key::N;
        case XK_o:              return Key::O;
        case XK_p:              return Key::P;
        case XK_q:              return Key::Q;
        case XK_r:              return Key::R;
        case XK_s:              return Key::S;
        case XK_t:              return Key::T;
        case XK_u:              return Key::U;
        case XK_v:              return Key::V;
        case XK_w:              return Key::W;
        case XK_x:              return Key::X;
        case XK_y:              return Key::Y;
        case XK_z:              return Key::Z;
        case XK_bracketleft:    return Key::LeftBracket;
        case XK_backslash:      return Key::Backslash;
        case XK_bracketright:   return Key::RightBracket;
        case XK_grave:          return Key::GraveAccent;
        case XK_KP_Insert:      return Key::KeyPad0;
        case XK_KP_End:         return Key::KeyPad1;
        case XK_KP_Down:        return Key::KeyPad2;
        case XK_KP_Page_Down:   return Key::KeyPad3;
        case XK_KP_Left:        return Key::KeyPad4;
        case XK_KP_Begin:       return Key::KeyPad5;
        case XK_KP_Right:       return Key::KeyPad6;
        case XK_KP_Home:        return Key::KeyPad7;
        case XK_KP_Up:          return Key::KeyPad8;
        case XK_KP_Page_Up:     return Key::KeyPad9;
        case XK_KP_Delete:      return Key::KeyPadDecimal;
        case XK_KP_Divide:      return Key::KeyPadDivide;
        case XK_KP_Multiply:    return Key::KeyPadMultiply;
        case XK_KP_Subtract:    return Key::KeyPadSubtract;
        case XK_KP_Add:         return Key::KeyPadAdd;
        case XK_KP_Enter:       return Key::KeyPadEnter;
        case XK_KP_Equal:       return Key::KeyPadEqual;
        case XK_Shift_L:        return Key::LeftShift;
        case XK_Control_L:      return Key::LeftControl;
        case XK_Meta_L:
        case XK_Alt_L:          return Key::LeftAlt;
        case XK_Super_L:        return Key::LeftSuper;
        case XK_Shift_R:        return Key::RightShift;
        case XK_Control_R:      return Key::RightControl;
        case XK_Mode_switch: // Mapped to Alt_R on many keyboards
        case XK_ISO_Level3_Shift: // AltGr on at least some machines
        case XK_Meta_R:
        case XK_Alt_R:          return Key::RightAlt;
        case XK_Super_R:        return Key::RightSuper;
        default:                return Key::Unknown;
    }
}

uint8_t StateToModifiers(uint state) {
    uint8_t modifiers = 0;
    if (state & ShiftMask) {
        modifiers |= KeyModifier::Shift;
    }
    if (state & ControlMask) {
        modifiers |= KeyModifier::Control;
    }
    if (state & Mod1Mask) {
        modifiers |= KeyModifier::Alt;
    }
    if (state & Mod4Mask) {
        modifiers |= KeyModifier::Super;
    }

    return modifiers;
}
