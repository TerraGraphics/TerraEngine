#pragma once

#include <string_view>

#include "core/math/types.h"
#include "middleware/imgui/style.h"


namespace gui {

enum class ButtonDir {
    Up,
    Down
};

struct ButtonStyle : Style {
};

bool ButtonArrow(std::string_view strId, ButtonDir dir, const ButtonStyle& style = ButtonStyle(), math::RectF* outWidgetRect = nullptr);

} // end namespace gui
