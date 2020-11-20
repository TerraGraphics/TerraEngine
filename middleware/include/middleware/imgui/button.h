#pragma once

#include <cstdint>
#include <string_view>

#include "core/math/types.h"
#include "middleware/imgui/style.h"


namespace gui {

enum class StepButtonAction : uint8_t {
    None,
    Up,
    Down
};

struct ButtonStyle : Style {
    math::SizeF drawSize;
};

bool ArrowButton(std::string_view strId, Direction dir, const ButtonStyle& style = ButtonStyle(), math::RectF* outRect = nullptr);
StepButtonAction StepButtons(std::string_view strId, const ButtonStyle& style = ButtonStyle(), math::RectF* outRect = nullptr);

} // end namespace gui
