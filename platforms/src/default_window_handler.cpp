#include "platforms/default_window_handler.h"


static constexpr const uint8_t IsDownMask = 1 << 4;
static constexpr const uint8_t IsStickyDownMask = 1 << 5;
static constexpr const uint8_t FirstPressMask = 1 << 6;
static constexpr const uint8_t FirstReleaseMask = 1 << 7;
static constexpr const uint8_t KeyModifierMask = KeyModifier::Shift | KeyModifier::Control | KeyModifier::Alt | KeyModifier::Super;

bool DefaultWindowEventsHandler::GetWindowSize(uint32_t& width, uint32_t& height) const noexcept {
    width = m_width;
    height = m_height;

    return m_sizeChanged;
}

void DefaultWindowEventsHandler::GetCursorPosition(float& posX, float& posY) const noexcept {
    posX = static_cast<float>(m_cursorPosX);
    posY = static_cast<float>(m_cursorPosY);
}

void DefaultWindowEventsHandler::GetCursorOffet(float& offsetX, float& offsetY) const noexcept {
    offsetX = static_cast<float>(m_cursorLastPosX - m_cursorPosX);
    offsetY = static_cast<float>(m_cursorLastPosY - m_cursorPosY);
}

bool DefaultWindowEventsHandler::IsKeyPressedFirstTime(Key code) const noexcept {
    if (code > Key::Last) {
        return false;
    }

    return (m_isKeyDown[static_cast<size_t>(code)] & FirstPressMask) != 0;
}

bool DefaultWindowEventsHandler::IsKeyReleasedFirstTime(Key code) const noexcept {
    if (code > Key::Last) {
        return false;
    }

    return (m_isKeyDown[static_cast<size_t>(code)] & FirstReleaseMask) != 0;
}

bool DefaultWindowEventsHandler::IsKeyDown(Key code) const noexcept {
    if (code > Key::Last) {
        return false;
    }

    return (m_isKeyDown[static_cast<size_t>(code)] & IsDownMask) != 0;
}

bool DefaultWindowEventsHandler::IsKeyStickyDown(Key code) const noexcept {
    if (code > Key::Last) {
        return false;
    }

    return (m_isKeyDown[static_cast<size_t>(code)] & IsStickyDownMask) != 0;
}

bool DefaultWindowEventsHandler::GetKeyStickyDownState(Key code, uint8_t& mods) const noexcept {
    if (code > Key::Last) {
        return false;
    }

    uint8_t state = m_isKeyDown[static_cast<size_t>(code)];
    mods = state & KeyModifierMask;
    return (state & IsStickyDownMask) != 0;
}

void DefaultWindowEventsHandler::FillKeyboardKeysDown(bool* keysDown) const noexcept {
    if (keysDown == nullptr) {
        return;
    }
    for(size_t i=0; i!=(static_cast<size_t>(Key::LastKeyboard) + 1); ++i) {
        keysDown[i] = (m_isKeyDown[i] & IsDownMask) != 0;
    }
}

std::u16string DefaultWindowEventsHandler::GetInput() const noexcept {
    return m_userInput;
}

void DefaultWindowEventsHandler::OnWindowDestroy() {
    m_windowShouldClose = true;
}

void DefaultWindowEventsHandler::OnNewFrame() {
    m_scrollOffset = 0;
    m_sizeChanged = false;

    m_userInput.clear();
    for(size_t i=0; i!=(static_cast<size_t>(Key::Last) + 1); ++i) {
        if ((m_isKeyDown[i] & IsDownMask) != 0) {
            // clear FirstPress and FirstRelease bits
            m_isKeyDown[i] &= ~(FirstPressMask | FirstReleaseMask);
        } else {
            m_isKeyDown[i] = 0;
        }
    }
}

void DefaultWindowEventsHandler::OnWindowSizeEvent(uint32_t width, uint32_t height) {
    if ((m_width != width) || (m_height != height)) {
        m_sizeChanged = true;
    }
    m_width = width;
    m_height = height;
}

void DefaultWindowEventsHandler::OnKeyEvent(KeyAction action, Key code, uint8_t modifiers) {
    if ((code > Key::LastKeyboard) || (action > KeyAction::Last)) {
        return;
    }

    uint8_t state = m_isKeyDown[static_cast<size_t>(code)] | (modifiers & KeyModifierMask);

    if (action == KeyAction::Release) {
        state |= FirstReleaseMask;
        // clear KeyModifier bits and IsDownMask bit
        state &= ~(KeyModifierMask | IsDownMask);
    } else{
        if ((state & IsDownMask) == 0) {
            state |= FirstPressMask;
        }
        state |= IsStickyDownMask;
        state |= IsDownMask;
    }
    m_isKeyDown[static_cast<size_t>(code)] = state;

    // virtual key processing
    switch (code) {
    case Key::LeftShift:
    case Key::RightShift:
        m_isKeyDown[static_cast<size_t>(Key::Shift)] = m_isKeyDown[static_cast<size_t>(Key::LeftShift)] | m_isKeyDown[static_cast<size_t>(Key::RightShift)];
        break;

    case Key::LeftControl:
    case Key::RightControl:
        m_isKeyDown[static_cast<size_t>(Key::Control)] = m_isKeyDown[static_cast<size_t>(Key::LeftControl)] | m_isKeyDown[static_cast<size_t>(Key::RightControl)];
        break;

    case Key::LeftAlt:
    case Key::RightAlt:
        m_isKeyDown[static_cast<size_t>(Key::Alt)] = m_isKeyDown[static_cast<size_t>(Key::LeftAlt)] | m_isKeyDown[static_cast<size_t>(Key::RightAlt)];
        break;

    case Key::LeftSuper:
    case Key::RightSuper:
        m_isKeyDown[static_cast<size_t>(Key::Super)] = m_isKeyDown[static_cast<size_t>(Key::LeftSuper)] | m_isKeyDown[static_cast<size_t>(Key::RightSuper)];
        break;

    default:
        break;
    }
}

void DefaultWindowEventsHandler::OnCharEvent(char16_t ch) {
    m_userInput += ch;
}

void DefaultWindowEventsHandler::OnCursorPosition(double x, double y) {
    m_cursorLastPosX = m_cursorPosX;
    m_cursorLastPosY = m_cursorPosY;
    m_cursorPosX = x;
    m_cursorPosY = y;
}

void DefaultWindowEventsHandler::OnScroll(int delta) {
    m_scrollOffset += delta;
}
