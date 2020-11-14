#pragma once

#include <string_view>

#include "core/math/types.h"
#include "middleware/imgui/style.h"


namespace gui {

enum class ButtonDir {
    Up,
    Down
};

enum class StepButtonAction {
    None,
    Up,
    Down
};

struct ButtonStyle : Style {
};

bool ArrowButton(std::string_view strId, ButtonDir dir, const ButtonStyle& style = ButtonStyle(), math::RectF* outWidgetRect = nullptr);
StepButtonAction StepButtons(std::string_view strId, const ButtonStyle& style = ButtonStyle(), math::RectF* outWidgetRect = nullptr);

} // end namespace gui
