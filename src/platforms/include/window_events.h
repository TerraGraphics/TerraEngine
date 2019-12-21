#pragma once

#include <string>


enum class Key : uint8_t {
        F1             = 0,
        F2             = 1,
        F3             = 2,
        F4             = 3,
        F5             = 4,
        F6             = 5,
        F7             = 6,
        F8             = 7,
        F9             = 8,
        F10            = 9,
        F11            = 10,
        F12            = 11,
        F13            = 12,
        F14            = 13,
        F15            = 14,
        F16            = 15,
        F17            = 16,
        F18            = 17,
        F19            = 18,
        F20            = 19,
        F21            = 20,
        F22            = 21,
        F23            = 22,
        F24            = 23,
        F25            = 24,
        Escape         = 25,
        Enter          = 26,
        Tab            = 27,
        Backspace      = 28,
        CapsLock       = 29,
        ScrollLock     = 30,
        NumLock        = 31,
        Space          = 32,           // ascii
        Insert         = 33,
        Delete         = 34,
        PageUp         = 35,
        PageDown       = 36,
        Home           = 37,
        End            = 38,
        Apostrophe     = 39,  /* ' */  // ascii
        ArrowRight     = 40,
        ArrowLeft      = 41,
        ArrowDown      = 42,
        ArrowUp        = 43,
        Comma          = 44,  /* , */  // ascii
        Minus          = 45,  /* - */  // ascii
        Period         = 46,  /* . */  // ascii
        Slash          = 47,  /* / */  // ascii
        Digit0         = 48,           // ascii
        Digit1         = 49,           // ascii
        Digit2         = 50,           // ascii
        Digit3         = 51,           // ascii
        Digit4         = 52,           // ascii
        Digit5         = 53,           // ascii
        Digit6         = 54,           // ascii
        Digit7         = 55,           // ascii
        Digit8         = 56,           // ascii
        Digit9         = 57,           // ascii
        PrintScreen    = 58,
        Semicolon      = 59,  /* ; */  // ascii
        Pause          = 60,
        Equal          = 61,  /* = */  // ascii
        NonUS1         = 62,           // not used
        NonUS2         = 63,           // not used
        Menu           = 64,
        A              = 65,           // ascii
        B              = 66,           // ascii
        C              = 67,           // ascii
        D              = 68,           // ascii
        E              = 69,           // ascii
        F              = 70,           // ascii
        G              = 71,           // ascii
        H              = 72,           // ascii
        I              = 73,           // ascii
        J              = 74,           // ascii
        K              = 75,           // ascii
        L              = 76,           // ascii
        M              = 77,           // ascii
        N              = 78,           // ascii
        O              = 79,           // ascii
        P              = 80,           // ascii
        Q              = 81,           // ascii
        R              = 82,           // ascii
        S              = 83,           // ascii
        T              = 84,           // ascii
        U              = 85,           // ascii
        V              = 86,           // ascii
        W              = 87,           // ascii
        X              = 88,           // ascii
        Y              = 89,           // ascii
        Z              = 90,           // ascii
        LeftBracket    = 91,  /* [ */  // ascii
        Backslash      = 92,  /* \ */  // ascii
        RightBracket   = 93,  /* ] */  // ascii
        GraveAccent    = 94,  /* ` */
        KeyPad0        = 95,
        KeyPad1        = 96,
        KeyPad2        = 97,
        KeyPad3        = 98,
        KeyPad4        = 99,
        KeyPad5        = 100,
        KeyPad6        = 101,
        KeyPad7        = 102,
        KeyPad8        = 103,
        KeyPad9        = 104,
        KeyPadDecimal  = 105,
        KeyPadDivide   = 106,
        KeyPadMultiply = 107,
        KeyPadSubtract = 108,
        KeyPadAdd      = 109,
        KeyPadEnter    = 110,
        KeyPadEqual    = 111,
        LeftShift      = 112,
        LeftControl    = 113,
        LeftAlt        = 114,
        LeftSuper      = 115,
        RightShift     = 116,
        RightControl   = 117,
        RightAlt       = 118,
        RightSuper     = 119,

        // Virtual
        Shift          = 120,          // LeftShift || RightShift
        Control        = 121,          // LeftControl || RightControl
        Alt            = 122,          // LeftAlt || RightAlt
        Super          = 123,          // LeftSuper || RightSuper

        // Mouse
        MouseLeft      = 124,          // GLFW_MOUSE_BUTTON_LEFT
        MouseRight     = 125,          // GLFW_MOUSE_BUTTON_RIGHT
        MouseMiddle    = 126,          // GLFW_MOUSE_BUTTON_MIDDLE

        Unknown        = 127,

        FirstMod       = LeftShift,
        LastKeyboard   = RightSuper,
        FirstMouse     = MouseLeft,
        LastMouse      = MouseMiddle,
        Last           = MouseMiddle,
};

enum KeyModifier : uint8_t {
    Shift   = 1,
    Control = 2,
    Alt     = 4,
    Super   = 8,
};

enum KeyAction : uint8_t {
    Release = 0,
    Press   = 1,
    Last    = Press,
};

std::string ToString(Key value);
std::string ToString(KeyModifier value);
std::string ToString(KeyAction value);

class WindowEventsHandler {
public:
    WindowEventsHandler() = default;
    virtual ~WindowEventsHandler() = default;

    virtual void OnKeyEvent(KeyAction /* action */, Key /* key */, uint8_t /* modifiers */) = 0;
    virtual void OnCursorPosition(int64_t x, int64_t y) = 0;
    virtual void OnScroll(int delta) = 0;
};
