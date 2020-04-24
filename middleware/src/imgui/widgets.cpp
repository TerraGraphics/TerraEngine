#include "middleware/imgui/widgets.h"

#include <utility>
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include "core/common/exception.h"
#include "middleware/imgui/imgui_math.h"

namespace {

static ImGuiDataType_ ToImGui(detail::DataType value) {
    switch (value) {
        case detail::DataType::S8: return ImGuiDataType_S8;
        case detail::DataType::U8: return ImGuiDataType_U8;
        case detail::DataType::S16: return ImGuiDataType_S16;
        case detail::DataType::U16: return ImGuiDataType_U16;
        case detail::DataType::S32: return ImGuiDataType_S32;
        case detail::DataType::U32: return ImGuiDataType_U32;
        case detail::DataType::S64: return ImGuiDataType_S64;
        case detail::DataType::U64: return ImGuiDataType_U64;
        case detail::DataType::Float: return ImGuiDataType_Float;
        case detail::DataType::Double: return ImGuiDataType_Double;
        default:
            throw EngineError("unknown value of widget type: '{}'", static_cast<uint8_t>(value));
    }
}

}

namespace detail {

bool InputScalar(const char* label, DataType dataType, void* value, const void* step, const void* stepFast, const char* format) {
    return ImGui::InputScalar(label, ToImGui(dataType), value, step, stepFast, format, ImGuiInputTextFlags(0));
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

void DrawNodeIcon(ImDrawList* drawList, math::RectI rect, IconType type, bool filled, ImU32 color, ImU32 innerColor) {
    const auto outline_scale  = rect.w / 24.0f;
    const auto extra_segments = static_cast<int>(2 * outline_scale); // for full circle

    if (type == IconType::Flow) {
        const auto origin_scale = rect.w / 24.0f;

        const auto offset_x  = 1.0f * origin_scale;
        const auto offset_y  = 0.0f * origin_scale;
        const auto margin     = (filled ? 2.0f : 2.0f) * origin_scale;
        const auto rounding   = 0.1f * origin_scale;
        const auto tip_round  = 0.7f; // percentage of triangle edge (for tip)
        //const auto edge_round = 0.7f; // percentage of triangle edge (for corner)
        // const auto canvas = rectf(
        const auto canvas = math::RectF(
            rect.x + margin + offset_x,
            rect.y + margin + offset_y,
            rect.w - margin * 2.0f,
            rect.h - margin * 2.0f);

        const auto left   = canvas.x + canvas.w            * 0.5f * 0.3f;
        const auto right  = canvas.x + canvas.w - canvas.w * 0.5f * 0.3f;
        const auto top    = canvas.y + canvas.h            * 0.5f * 0.2f;
        const auto bottom = canvas.y + canvas.h - canvas.h * 0.5f * 0.2f;
        const auto center_y = (top + bottom) * 0.5f;
        //const auto angle = AX_PI * 0.5f * 0.5f * 0.5f;

        const auto tip_top    = ImVec2(canvas.x + canvas.w * 0.5f, top);
        const auto tip_right  = ImVec2(right, center_y);
        const auto tip_bottom = ImVec2(canvas.x + canvas.w * 0.5f, bottom);

        drawList->PathLineTo(ImVec2(left, top) + ImVec2(0, rounding));
        drawList->PathBezierCurveTo(
            ImVec2(left, top),
            ImVec2(left, top),
            ImVec2(left, top) + ImVec2(rounding, 0));
        drawList->PathLineTo(tip_top);
        drawList->PathLineTo(tip_top + (tip_right - tip_top) * tip_round);
        drawList->PathBezierCurveTo(
            tip_right,
            tip_right,
            tip_bottom + (tip_right - tip_bottom) * tip_round);
        drawList->PathLineTo(tip_bottom);
        drawList->PathLineTo(ImVec2(left, bottom) + ImVec2(rounding, 0));
        drawList->PathBezierCurveTo(
            ImVec2(left, bottom),
            ImVec2(left, bottom),
            ImVec2(left, bottom) - ImVec2(0, rounding));

        if (!filled) {
            if (innerColor & 0xFF000000)
                drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColor);

            drawList->PathStroke(color, true, 2.0f * outline_scale);
        } else {
            drawList->PathFillConvex(color);
        }
    } else {
        auto triangleStart = rect.CenterX() + 0.32f * rect.w;

        rect.x -= static_cast<int>(rect.w * 0.25f * 0.25f);

        if (type == IconType::Circle) {
            const auto c = ToImGui(rect.Center());

            if (!filled) {
                const auto r = 0.5f * rect.w / 2.0f - 0.5f;

                if (innerColor & 0xFF000000)
                    drawList->AddCircleFilled(c, r, innerColor, 12 + extra_segments);
                drawList->AddCircle(c, r, color, 12 + extra_segments, 2.0f * outline_scale);
            } else {
                drawList->AddCircleFilled(c, 0.5f * rect.w / 2.0f, color, 12 + extra_segments);
            }
        }

        if (type == IconType::Square) {
            if (filled) {
                const auto r  = 0.5f * rect.w / 2.0f;
                const auto p0 = ToImGui(rect.Center()) - ImVec2(r, r);
                const auto p1 = ToImGui(rect.Center()) + ImVec2(r, r);

                drawList->AddRectFilled(p0, p1, color, 0, 15 + extra_segments);
            } else {
                const auto r = 0.5f * rect.w / 2.0f - 0.5f;
                const auto p0 = ToImGui(rect.Center()) - ImVec2(r, r);
                const auto p1 = ToImGui(rect.Center()) + ImVec2(r, r);

                if (innerColor & 0xFF000000) {
                    drawList->AddRectFilled(p0, p1, innerColor, 0, 15 + extra_segments);
                }

                drawList->AddRect(p0, p1, color, 0, 15 + extra_segments, 2.0f * outline_scale);
            }
        }

        if (type == IconType::Grid) {
            const auto r = 0.5f * rect.w / 2.0f;
            const auto w = ceilf(r / 3.0f);

            const auto baseTl = ImVec2(floorf(rect.CenterX() - w * 2.5f), floorf(rect.CenterY() - w * 2.5f));
            const auto baseBr = ImVec2(floorf(baseTl.x + w), floorf(baseTl.y + w));

            auto tl = baseTl;
            auto br = baseBr;
            for (int i = 0; i < 3; ++i) {
                tl.x = baseTl.x;
                br.x = baseBr.x;
                drawList->AddRectFilled(tl, br, color);
                tl.x += w * 2;
                br.x += w * 2;
                if (i != 1 || filled)
                    drawList->AddRectFilled(tl, br, color);
                tl.x += w * 2;
                br.x += w * 2;
                drawList->AddRectFilled(tl, br, color);

                tl.y += w * 2;
                br.y += w * 2;
            }

            triangleStart = br.x + w + 1.0f / 24.0f * rect.w;
        }

        if (type == IconType::RoundSquare) {
            if (filled) {
                const auto r  = 0.5f * rect.w / 2.0f;
                const auto cr = r * 0.5f;
                const auto p0 = ToImGui(rect.Center()) - ImVec2(r, r);
                const auto p1 = ToImGui(rect.Center()) + ImVec2(r, r);

                drawList->AddRectFilled(p0, p1, color, cr, 15);
            } else {
                const auto r = 0.5f * rect.w / 2.0f - 0.5f;
                const auto cr = r * 0.5f;
                const auto p0 = ToImGui(rect.Center()) - ImVec2(r, r);
                const auto p1 = ToImGui(rect.Center()) + ImVec2(r, r);

                if (innerColor & 0xFF000000) {
                    drawList->AddRectFilled(p0, p1, innerColor, cr, 15);
                }

                drawList->AddRect(p0, p1, color, cr, 15, 2.0f * outline_scale);
            }
        } else if (type == IconType::Diamond) {
            if (filled) {
                const auto r = 0.607f * rect.w / 2.0f;
                const auto c = rect.Center();

                drawList->PathLineTo(ToImGui(c) + ImVec2( 0, -r));
                drawList->PathLineTo(ToImGui(c) + ImVec2( r,  0));
                drawList->PathLineTo(ToImGui(c) + ImVec2( 0,  r));
                drawList->PathLineTo(ToImGui(c) + ImVec2(-r,  0));
                drawList->PathFillConvex(color);
            } else {
                const auto r = 0.607f * rect.w / 2.0f - 0.5f;
                const auto c = rect.Center();

                drawList->PathLineTo(ToImGui(c) + ImVec2( 0, -r));
                drawList->PathLineTo(ToImGui(c) + ImVec2( r,  0));
                drawList->PathLineTo(ToImGui(c) + ImVec2( 0,  r));
                drawList->PathLineTo(ToImGui(c) + ImVec2(-r,  0));

                if (innerColor & 0xFF000000) {
                    drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColor);
                }

                drawList->PathStroke(color, true, 2.0f * outline_scale);
            }
        } else {
            const auto triangleTip = triangleStart + rect.w * (0.45f - 0.32f);

            drawList->AddTriangleFilled(
                ImVec2(ceilf(triangleTip), rect.Top() + rect.h * 0.5f),
                ImVec2(triangleStart, rect.CenterY() + 0.15f * rect.h),
                ImVec2(triangleStart, rect.CenterY() - 0.15f * rect.h),
                color);
        }
    }
}

} // end namespace gui::detail


math::Rect Image(TextureViewRaw texture, math::Size size, bool isOpenGL, math::PointF uv0, math::PointF uv1, math::Color tintCol) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return math::Rect();
    }

    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ToImGui(size));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, 0)) {
        return math::Rect();
    }

    if (isOpenGL) {
        std::swap(uv0.y, uv1.y);
    }

    window->DrawList->AddImage(reinterpret_cast<ImTextureID>(texture), bb.Min, bb.Max, ToImGui(uv0), ToImGui(uv1), tintCol.value);

    return math::Rect(ToPoint(bb.Min), ToPoint(bb.Max));
}

void NodeIcon(const math::Size& size, IconType type, bool filled, math::Color color, math::Color innerColor) {
    const ImVec2 imSize = ToImGui(size);
    if (ImGui::IsRectVisible(imSize)) {
        auto cursorPos = ImGui::GetCursorScreenPos();
        auto drawList  = ImGui::GetWindowDrawList();

        math::RectI rect(static_cast<int32_t>(cursorPos.x), static_cast<int32_t>(cursorPos.y), static_cast<int32_t>(size.w), static_cast<int32_t>(size.h));
        detail::DrawNodeIcon(drawList, rect, type, filled, color.value, innerColor.value);
    }

    ImGui::Dummy(imSize);
}
