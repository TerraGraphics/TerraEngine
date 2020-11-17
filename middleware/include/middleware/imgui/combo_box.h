#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <string_view>

#include "middleware/imgui/style.h"


namespace gui {

struct ComboBoxStyle : Style {
    float width = 50.f;
};

bool ComboBox(std::string_view strId, size_t& index, const std::vector<std::string>& items, const ComboBoxStyle& style = ComboBoxStyle(), math::RectF* outWidgetRect = nullptr);

} // end namespace gui
