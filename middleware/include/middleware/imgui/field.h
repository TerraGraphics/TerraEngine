#pragma once

#include <string>
#include <cstdint>
#include <string_view>

#include "middleware/imgui/style.h"


namespace gui {

struct FieldStyle : Style {
    float width = 50.f;
};

bool TextField(std::string_view strId, std::string& text, const FieldStyle& style = FieldStyle(), math::RectF* outRect = nullptr);

struct NumberFieldStyle : FieldStyle {
    bool isInteger = true;
    bool showStepButtons = false;
};

enum class NumberFieldAction : uint8_t {
    None,
    StepUp,
    StepDown,
    Changed,
};

NumberFieldAction NumberField(std::string_view strId, std::string& text, const NumberFieldStyle& style = NumberFieldStyle(), math::RectF* outRect = nullptr);

} // end namespace gui
