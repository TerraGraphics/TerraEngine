#pragma once

#include <string>
#include <string_view>

#include "middleware/imgui/style.h"


namespace gui {

struct FieldStyle : Style {
    float width = 50.f;
};

bool TextField(std::string_view strId, std::string& text, const FieldStyle& style = FieldStyle(), math::RectF* outWidgetRect = nullptr);

} // end namespace gui
