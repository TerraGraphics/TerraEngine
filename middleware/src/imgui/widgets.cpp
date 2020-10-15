#include "middleware/imgui/widgets.h"

#include <cmath>
#include <memory>
#include <utility>
#include <istream>
#include <cstring>
#include <charconv>
#include <system_error>

#include "fmt/fmt.h"
#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "middleware/imgui/font.h"
#include "core/common/exception.h"
#include "middleware/imgui/imgui_math.h"

namespace {

struct ApplyOperation {
    ApplyOperation(bool isAdd, const gui::detail::NumberVariant& step, const gui::detail::NumberVariant& minValue, const gui::detail::NumberVariant& maxValue)
    : isAdd(isAdd)
    , step(step)
    , minValue(minValue)
    , maxValue(maxValue)
    {}

    template<typename T> void operator()(T& value) {
        if (isAdd) {
            value = ImAddClampOverflow(value, std::get<T>(step), std::get<T>(minValue), std::get<T>(maxValue));
        } else {
            value = ImSubClampOverflow(value, std::get<T>(step), std::get<T>(minValue), std::get<T>(maxValue));
        }
    }

    bool isAdd;
    const gui::detail::NumberVariant& step;
    const gui::detail::NumberVariant& minValue;
    const gui::detail::NumberVariant& maxValue;
};

struct FormatVariant {
    explicit FormatVariant(const char* format) : format(format) {}

    template<typename T> void operator()(T& value) {
        if (format == nullptr) {
            fmt::format_to(buffer, "{}", value);
        } else {
            fmt::format_to(buffer, format, value);
        }
        buffer[buffer.size()] = 0;
    }

    fmt::basic_memory_buffer<char, 64> buffer;
    const char* format;
};

struct Parse {
    Parse(const char* buffer, const gui::detail::NumberVariant& minValue, const gui::detail::NumberVariant& maxValue)
    : buffer(buffer)
    , minValue(minValue)
    , maxValue(maxValue)
    {}

    template<typename T> bool operator()(T& value) {
        // TODO: wait from_chars for float and double
        if constexpr (std::is_integral_v<T>) {
            T tmp;
            if(auto [p, ec] = std::from_chars(buffer, buffer + std::strlen(buffer), tmp); ec == std::errc()) {
                if ((tmp < std::get<T>(minValue)) || (tmp > std::get<T>(maxValue))) {
                    return false;
                }

                value = tmp;
                return true;
            }

            return false;
        } else if constexpr (std::is_floating_point_v<T>) {
            try {
                std::istringstream ss(buffer);
                T tmp;
                ss >> tmp;
                if ((tmp < std::get<T>(minValue)) || (tmp > std::get<T>(maxValue))) {
                    return false;
                }
                value = tmp;
                return true;
            } catch(const std::exception& e) {
                return false;
            }
        }

        return false;
    }

    const char* buffer;
    const gui::detail::NumberVariant& minValue;
    const gui::detail::NumberVariant& maxValue;
};

}

namespace gui {
namespace detail {

bool InputScalar(const char* label, NumberVariant& value, const NumberVariant& step, const NumberVariant& minValue, const NumberVariant& maxValue, const char* format) {
    bool valueChanged = false;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return valueChanged;
    }

    FormatVariant formater(format);
    std::visit(formater, value);
    char* buffer = formater.buffer.data();
    size_t bufferSize = formater.buffer.capacity();

    ImGuiContext& g = *GImGui;
    const float buttonSize = ImGui::GetFrameHeight();
    const ImGuiInputTextFlags flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoMarkEdited;
    ImGuiButtonFlags buttonFlags = ImGuiButtonFlags_Repeat | ImGuiButtonFlags_DontClosePopups;
    if (flags & ImGuiInputTextFlags_ReadOnly) {
        buttonFlags |= ImGuiButtonFlags_Disabled;
    }

    ImGui::BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    ImGui::PushID(label);
    ImGui::SetNextItemWidth(ImMax(1.0f, ImGui::CalcItemWidth() - buttonSize));
    if (ImGui::InputText("", buffer, bufferSize, flags)) { // PushId(label) + "" gives us the expected ID from outside point of view
        valueChanged = std::visit(Parse(buffer, minValue, maxValue), value);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    ImGui::SameLine(0, 0);
    ImGui::BeginGroup();
    auto systemDisplayOffsetY = g.Font->DisplayOffset.y;
    g.Font->DisplayOffset.y = -5;
    if (ImGui::ButtonEx(ICON_FA_ANGLE_UP, ImVec2(buttonSize, buttonSize / 2), buttonFlags)) {
        std::visit(ApplyOperation(true, step, minValue, maxValue), value);
        valueChanged = true;
    }
    if (ImGui::ButtonEx(ICON_FA_ANGLE_DOWN, ImVec2(buttonSize, buttonSize / 2), buttonFlags)) {
        std::visit(ApplyOperation(false, step, minValue, maxValue), value);
        valueChanged = true;
    }
    g.Font->DisplayOffset.y = systemDisplayOffsetY;
    ImGui::EndGroup();
    ImGui::PopStyleVar(1);

    const char* labelEnd = ImGui::FindRenderedTextEnd(label);
    if (label != labelEnd) {
        ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
        ImGui::TextEx(label, labelEnd);
    }

    ImGui::PopID();
    ImGui::EndGroup();

    if (valueChanged) {
        ImGui::MarkItemEdited(window->DC.LastItemId);
    }

    return valueChanged;
}

bool InputScalarN(const char* label, NumberVariant* values, size_t components, const NumberVariant& step, const NumberVariant& minValue, const NumberVariant& maxValue, const char* format) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    bool valueChanged = false;
    if (window->SkipItems) {
        return valueChanged;
    }

    ImGuiContext& g = *GImGui;
    ImGui::BeginGroup();
    ImGui::PushID(label);
    ImGui::PushMultiItemsWidths(static_cast<int>(components), ImGui::CalcItemWidth());
    for (size_t i = 0; i!=components; ++i) {
        ImGui::PushID(static_cast<int>(i));
        if (i > 0) {
            ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
        }
        valueChanged |= InputScalar("", values[i], step, minValue, maxValue, format);
        ImGui::PopID();
        ImGui::PopItemWidth();
    }
    ImGui::PopID();

    const char* label_end = ImGui::FindRenderedTextEnd(label);
    if (label != label_end) {
        ImGui::SameLine(0.0f, g.Style.ItemInnerSpacing.x);
        ImGui::TextEx(label, label_end);
    }

    ImGui::EndGroup();
    return valueChanged;
}

bool Combo(const char* label, size_t& currentIndex, const char* const* itemNames, const size_t numberItems) {
    bool changed = false;
    const char* itemCurrent = itemNames[currentIndex];
    if (ImGui::BeginCombo(label, itemCurrent, ImGuiComboFlags(0))) {
        for (size_t i=0; i!=numberItems; ++i) {
            bool selected = (currentIndex == i);
            if (ImGui::Selectable(itemNames[i], selected)) {
                changed = (i != currentIndex);
                currentIndex = i;
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    return changed;
}

} // end namespace gui::detail

void BeginGroup() {
    ImGui::BeginGroup();
}

void EndGroup() {
    ImGui::EndGroup();
}

void Dummy(math::SizeF size) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return;
    }

    const auto imSize = ToImGui(size);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + imSize);
    ImGui::ItemSize(imSize);
    ImGui::ItemAdd(bb, 0);
}

void Text(std::string_view text) {
    ImGui::TextEx(text.cbegin(), text.cend(), ImGuiTextFlags_NoWidthForLargeClippedText);
}

math::Rect Image(TextureViewRaw texture, math::Size size, bool isOpenGL, math::PointF uv0, math::PointF uv1, math::Color tintCol, math::Color borderCol) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return math::Rect();
    }

    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ToImGui(size));
    if (borderCol.alpha > 0) {
        bb.Max += ImVec2(2, 2);
    }

    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, 0)) {
        return math::Rect();
    }

    if (isOpenGL) {
        std::swap(uv0.y, uv1.y);
    }

    if (borderCol.alpha > 0) {
        window->DrawList->AddRect(bb.Min, bb.Max, borderCol.value, 0.0f);
        window->DrawList->AddImage(reinterpret_cast<ImTextureID>(texture), bb.Min + ImVec2(1, 1), bb.Max - ImVec2(1, 1), ToImGui(uv0), ToImGui(uv1), tintCol.value);
    } else {
        window->DrawList->AddImage(reinterpret_cast<ImTextureID>(texture), bb.Min, bb.Max, ToImGui(uv0), ToImGui(uv1), tintCol.value);
    }

    return math::Rect(ToPointU(bb.Min), ToPointU(bb.Max));
}

} // end namespace gui
