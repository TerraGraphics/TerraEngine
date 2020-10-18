#include "middleware/imgui/icon.h"

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/gui_helpers.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

float GetThickness(math::RectF rect) {
    return rect.w / 12.0f;
}

int GetNumSegments(math::RectF rect, int additional) {
    return static_cast<int>(rect.w / 12.0f) + additional;
}

void DrawTriangle(ImDrawList* drawList, math::RectF rect, float triangleStart, uint32_t color) {
    const auto triangleTip = triangleStart + rect.w * (0.45f - 0.32f);

    drawList->AddTriangleFilled(
        ImVec2(ceilf(triangleTip), rect.Top() + rect.h * 0.5f),
        ImVec2(triangleStart, rect.CenterY() + 0.15f * rect.h),
        ImVec2(triangleStart, rect.CenterY() - 0.15f * rect.h),
        color);
}

void DrawFlow(ImDrawList* drawList, math::RectF rect, bool filled, uint32_t color, uint32_t innerColor) {
    const auto thickness = GetThickness(rect);
    const auto originScale = rect.w / 24.0f;
    const auto offsetX  = 1.0f * originScale;
    const auto offsetY  = 0.0f * originScale;
    const auto margin   = (filled ? 2.0f : 2.0f) * originScale;
    const auto rounding = 0.1f * originScale;
    const auto tipRound = 0.7f; // percentage of triangle edge (for tip)

    const auto canvas = math::RectF(
        rect.x + margin + offsetX,
        rect.y + margin + offsetY,
        rect.w - margin * 2.0f,
        rect.h - margin * 2.0f);

    const auto left   = canvas.x + canvas.w            * 0.5f * 0.3f;
    const auto right  = canvas.x + canvas.w - canvas.w * 0.5f * 0.3f;
    const auto top    = canvas.y + canvas.h            * 0.5f * 0.2f;
    const auto bottom = canvas.y + canvas.h - canvas.h * 0.5f * 0.2f;
    const auto center_y = (top + bottom) * 0.5f;

    const auto tipTop    = ImVec2(canvas.x + canvas.w * 0.5f, top);
    const auto tipRight  = ImVec2(right, center_y);
    const auto tipBottom = ImVec2(canvas.x + canvas.w * 0.5f, bottom);

    drawList->PathLineTo(ImVec2(left, top) + ImVec2(0, rounding));
    drawList->PathBezierCurveTo(
        ImVec2(left, top),
        ImVec2(left, top),
        ImVec2(left, top) + ImVec2(rounding, 0));
    drawList->PathLineTo(tipTop);
    drawList->PathLineTo(tipTop + (tipRight - tipTop) * tipRound);
    drawList->PathBezierCurveTo(
        tipRight,
        tipRight,
        tipBottom + (tipRight - tipBottom) * tipRound);
    drawList->PathLineTo(tipBottom);
    drawList->PathLineTo(ImVec2(left, bottom) + ImVec2(rounding, 0));
    drawList->PathBezierCurveTo(
        ImVec2(left, bottom),
        ImVec2(left, bottom),
        ImVec2(left, bottom) - ImVec2(0, rounding));

    if (!filled) {
        if (innerColor & 0xFF000000) {
            drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColor);
        }

        drawList->PathStroke(color, true, thickness);
    } else {
        drawList->PathFillConvex(color);
    }
}

void DrawCircle(ImDrawList* drawList, math::RectF rect, bool filled, uint32_t color, uint32_t innerColor) {
    const auto thickness = GetThickness(rect);
    const auto numSegments = GetNumSegments(rect, 12);
    const auto triangleStart = rect.CenterX() + 0.32f * rect.w;
    rect.x -= (rect.w * 0.25f * 0.25f);

    const auto c = ToImGui(rect.Center());

    if (!filled) {
        const auto r = 0.5f * rect.w / 2.0f - 0.5f;

        if (innerColor & 0xFF000000)
            drawList->AddCircleFilled(c, r, innerColor, numSegments);
        drawList->AddCircle(c, r, color, numSegments, thickness);
    } else {
        drawList->AddCircleFilled(c, 0.5f * rect.w / 2.0f, color, numSegments);
    }

    DrawTriangle(drawList, rect, triangleStart, color);
}

void DrawSquare(ImDrawList* drawList, math::RectF rect, bool filled, uint32_t color, uint32_t innerColor) {
    const auto thickness = GetThickness(rect);
    const auto numSegments = GetNumSegments(rect, 15);
    const auto triangleStart = rect.CenterX() + 0.32f * rect.w;
    rect.x -= (rect.w * 0.25f * 0.25f);

    if (filled) {
        const auto r  = 0.5f * rect.w / 2.0f;
        const auto p0 = ToImGui(rect.Center()) - ImVec2(r, r);
        const auto p1 = ToImGui(rect.Center()) + ImVec2(r, r);

        drawList->AddRectFilled(p0, p1, color, 0, numSegments);
    } else {
        const auto r = 0.5f * rect.w / 2.0f - 0.5f;
        const auto p0 = ToImGui(rect.Center()) - ImVec2(r, r);
        const auto p1 = ToImGui(rect.Center()) + ImVec2(r, r);

        if (innerColor & 0xFF000000) {
            drawList->AddRectFilled(p0, p1, innerColor, 0, numSegments);
        }

        drawList->AddRect(p0, p1, color, 0, numSegments, thickness);
    }

    DrawTriangle(drawList, rect, triangleStart, color);
}

void DrawGrid(ImDrawList* drawList, math::RectF rect, bool filled, uint32_t color) {
    rect.x -= (rect.w * 0.25f * 0.25f);

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
        if (i != 1 || filled) {
            drawList->AddRectFilled(tl, br, color);
        }
        tl.x += w * 2;
        br.x += w * 2;
        drawList->AddRectFilled(tl, br, color);

        tl.y += w * 2;
        br.y += w * 2;
    }

    const auto triangleStart = br.x + w + 1.0f / 24.0f * rect.w;
    DrawTriangle(drawList, rect, triangleStart, color);
}

void DrawRoundSquare(ImDrawList* drawList, math::RectF rect, bool filled, uint32_t color, uint32_t innerColor) {
    const auto thickness = GetThickness(rect);
    rect.x -= (rect.w * 0.25f * 0.25f);

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

        drawList->AddRect(p0, p1, color, cr, 15, thickness);
    }
}

void DrawDiamond(ImDrawList* drawList, math::RectF rect, bool filled, uint32_t color, uint32_t innerColor) {
    const auto thickness = GetThickness(rect);
    rect.x -= (rect.w * 0.25f * 0.25f);

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

        drawList->PathStroke(color, true, thickness);
    }
}

math::RectF Icon(math::SizeF size, IconType type, bool filled, math::Color color, math::Color innerColor) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return math::RectF();
    }

    const auto iconPos = ToPointF(window->DC.CursorPos);
    const math::RectF iconRect(iconPos, size);

    if (!ItemFullAdd(iconRect)) {
        return iconRect;
    }

    if (IsRectVisible(iconRect)) {
        auto drawList = window->DrawList;
        const auto clr = color.value;
        const auto innerClr = innerColor.value;

        switch (type) {
        case IconType::Flow:
            DrawFlow(drawList, iconRect, filled, clr, innerClr);
            break;
        case IconType::Circle:
            DrawCircle(drawList, iconRect, filled, clr, innerClr);
            break;
        case IconType::Square:
            DrawSquare(drawList, iconRect, filled, clr, innerClr);
            break;
        case IconType::Grid:
            DrawGrid(drawList, iconRect, filled, clr);
            break;
        case IconType::RoundSquare:
            DrawRoundSquare(drawList, iconRect, filled, clr, innerClr);
            break;
        case IconType::Diamond:
            DrawDiamond(drawList, iconRect, filled, clr, innerClr);
            break;
        }
    }

    return iconRect;
}

} // end namespace gui
