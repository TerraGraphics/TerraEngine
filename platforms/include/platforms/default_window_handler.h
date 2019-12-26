#pragma once

#include "platforms/window_events.h"


class DefaultWindowEventsHandler : public WindowEventsHandler {
public:
    DefaultWindowEventsHandler() = default;
    ~DefaultWindowEventsHandler() override = default;

public:
    bool GetWindowSize(uint32_t& width, uint32_t& height) const noexcept;

    void GetCursorPosition(float& posX, float& posY) const noexcept;
    void GetCursorOffet(float& offsetX, float& offsetY) const noexcept;

    float GetScrollOffset() const noexcept { return m_scrollOffset; }

    bool IsKeyPressedFirstTime(Key code) const noexcept;
    bool IsKeyReleasedFirstTime(Key code) const noexcept;
    // Current state
    bool IsKeyDown(Key code) const noexcept;
    // Down state per frame
    bool IsKeyStickyDown(Key code) const noexcept;
    bool GetKeyStickyDownState(Key code, uint8_t& mods) const noexcept;
    void FillKeyboardKeysDown(bool* keysDown) const noexcept;

    std::u16string GetInput() const noexcept;

protected:
    void Update() override;
    void OnWindowSizeEvent(uint32_t width, uint32_t height) override;
    void OnKeyEvent(KeyAction action, Key code, uint8_t modifiers) override;
    void OnCharEvent(char16_t ch) override;
    void OnCursorPosition(double x, double y) override;
    void OnScroll(int delta) override;

private:
    bool m_sizeChanged = false;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    double m_cursorPosX = 0;
    double m_cursorPosY = 0;
    double m_cursorLastPosX = 0;
    double m_cursorLastPosY = 0;
    int m_scrollOffset = 0;
    // FirstRelease, FirstPress, IsStickyDown, IsDown, Super, Alt, Control, Shift
    uint8_t m_isKeyDown[static_cast<size_t>(Key::Last) + 1] = { 0 };
    std::u16string m_userInput;
};
