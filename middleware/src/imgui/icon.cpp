#include "middleware/imgui/icon.h"

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/gui_helpers.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

class IconDriver {
public:
    IconDriver(ImDrawList* drawList, float sideSize, math::RectF rect, bool filled, uint32_t color)
        : m_drawList(drawList)
        , m_thickness(sideSize / 12.0f)
        , m_rect(rect)
        , m_filled(filled)
        , m_color(color) {}

    void DrawAdditionalTriangle(float triangleStart) {
        const auto triangleTip = triangleStart + m_rect.w * (0.45f - 0.32f);

        m_drawList->AddTriangleFilled(
            ImVec2(ceilf(triangleTip), m_rect.Top() + m_rect.h * 0.5f),
            ImVec2(triangleStart, m_rect.CenterY() + 0.15f * m_rect.h),
            ImVec2(triangleStart, m_rect.CenterY() - 0.15f * m_rect.h),
            m_color);
    }

    void DrawSquare(bool rounded, bool withTriange) {
        const auto triangleStart = m_rect.CenterX() + 0.32f * m_rect.w;
        m_rect.x -= (m_rect.w * 0.25f * 0.25f);

        if (m_filled) {
            const auto r  = 0.5f * m_rect.w / 2.0f;
            const auto rounding = rounded ? r * 0.5f : 0;
            const auto p0 = ToImGui(m_rect.Center()) - ImVec2(r, r);
            const auto p1 = ToImGui(m_rect.Center()) + ImVec2(r, r);

            m_drawList->AddRectFilled(p0, p1, m_color, rounding, ImDrawCornerFlags_All);
        } else {
            const auto r = 0.5f * m_rect.w / 2.0f - 0.5f;
            const auto rounding = rounded ? r * 0.5f : 0;
            const auto p0 = ToImGui(m_rect.Center()) - ImVec2(r, r);
            const auto p1 = ToImGui(m_rect.Center()) + ImVec2(r, r);

            m_drawList->AddRect(p0, p1, m_color, rounding, ImDrawCornerFlags_All, m_thickness);
        }

        if (withTriange) {
            DrawAdditionalTriangle(triangleStart);
        }
    }

private:
    ImDrawList* m_drawList;
    float m_thickness;
    math::RectF m_rect;
    bool m_filled;
    uint32_t m_color;
};

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

void DrawFlow(ImDrawList* drawList, math::RectF rect, bool filled, uint32_t color, uint32_t fillColor) {
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
        if (fillColor & 0xFF000000) {
            drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, fillColor);
        }

        drawList->PathStroke(color, true, thickness);
    } else {
        drawList->PathFillConvex(color);
    }
}

void DrawCircle(ImDrawList* drawList, math::RectF rect, bool filled, uint32_t color, uint32_t fillColor) {
    const auto thickness = GetThickness(rect);
    const auto numSegments = GetNumSegments(rect, 12);
    const auto triangleStart = rect.CenterX() + 0.32f * rect.w;
    rect.x -= (rect.w * 0.25f * 0.25f);

    const auto c = ToImGui(rect.Center());

    if (!filled) {
        const auto r = 0.5f * rect.w / 2.0f - 0.5f;

        if (fillColor & 0xFF000000)
            drawList->AddCircleFilled(c, r, fillColor, numSegments);
        drawList->AddCircle(c, r, color, numSegments, thickness);
    } else {
        drawList->AddCircleFilled(c, 0.5f * rect.w / 2.0f, color, numSegments);
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

void DrawDiamond(ImDrawList* drawList, math::RectF rect, bool filled, uint32_t color, uint32_t fillColor) {
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

        if (fillColor & 0xFF000000) {
            drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, fillColor);
        }

        drawList->PathStroke(color, true, thickness);
    }
}

math::RectF Icon(IconType type, bool filled, const IconStyle& style, math::SizeF minSize) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return math::RectF();
    }

    const math::SizeF drawSize(style.sideSize, style.sideSize);

    math::RectF drawRect;
    math::RectF widgetRect;
    if (!PlaceWidget(static_cast<const Style*>(&style), minSize, drawSize, drawRect, widgetRect)) {
        return widgetRect;
    }

    if (!IsRectVisible(drawRect)) {
        return widgetRect;
    }

    auto drawList = window->DrawList;
    const auto color = style.color.value;
    const auto fillColor = style.fillColor.value;

    IconDriver driver(drawList, style.sideSize, drawRect, filled, color);
    switch (type) {
    case IconType::Square:
        driver.DrawSquare(false, false);
        break;
    case IconType::SquareTriangle:
        driver.DrawSquare(false, true);
        break;
    case IconType::RoundSquare:
        driver.DrawSquare(true, false);
        break;
    case IconType::RoundSquareTriangle:
        driver.DrawSquare(true, true);
        break;
    case IconType::Flow:
        DrawFlow(drawList, drawRect, filled, color, fillColor);
        break;
    case IconType::Circle:
        DrawCircle(drawList, drawRect, filled, color, fillColor);
        break;
    case IconType::Grid:
        DrawGrid(drawList, drawRect, filled, color);
        break;
    case IconType::Diamond:
        DrawDiamond(drawList, drawRect, filled, color, fillColor);
        break;
    }


    return widgetRect;
}

} // end namespace gui
