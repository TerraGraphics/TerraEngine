#pragma once

#include <cstdint>
#include <string_view>

#include "core/math/types.h"
#include "middleware/imgui/style.h"


namespace gui {

enum class ButtonDir : uint8_t {
    Up,
    Down
};

enum class StepButtonAction : uint8_t {
    None,
    Up,
    Down
};

struct ButtonStyle : Style {
};

bool ArrowButton(std::string_view strId, ButtonDir dir, const ButtonStyle& style = ButtonStyle(), math::RectF* outWidgetRect = nullptr);
StepButtonAction StepButtons(std::string_view strId, const ButtonStyle& style = ButtonStyle(), math::RectF* outWidgetRect = nullptr);

} // end namespace gui
