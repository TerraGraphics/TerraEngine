#include "middleware/imgui/icon.h"

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/gui_helpers.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

class IconDriver {
public:
    IconDriver(ImDrawList* drawList, math::RectF rect, uint32_t color, bool filled) noexcept
        : m_drawList(drawList)
        , m_rect(rect)
        , m_color(color)
        , m_filled(filled) {}

    float GetAdditionalTriangleSizeX() const noexcept {
        return m_rect.w * 0.13f;
    }

    ImVec2 GetCenter() const noexcept {
        return ImVec2(m_rect.CenterX() - GetAdditionalTriangleSizeX() * 0.5f, m_rect.CenterY());
    }

    float GetRadius() const noexcept {
        float r = m_rect.w / 3.f - GetAdditionalTriangleSizeX() * 0.5f;
        if (!m_filled) {
            r -= 0.5f;
        }
        return r;
    }

    float GetThickness() const noexcept {
        return m_rect.w / 12.0f;
    }

    void DrawAdditionalTriangle() const {
        const float x1 = m_rect.Right() - 1.5f;
        const float x0 = x1 - GetAdditionalTriangleSizeX();
        const float centerY = m_rect.CenterY();

        m_drawList->AddTriangleFilled(
            ImVec2(x0, centerY - 0.15f * m_rect.h),
            ImVec2(x0, centerY + 0.15f * m_rect.h),
            ImVec2(x1, centerY),
            m_color);
    }

    void DrawCircle(bool withTriange) const {
        const int numSegments = 0;
        const auto r = GetRadius();
        const auto center = GetCenter();

        if (m_filled) {
            m_drawList->AddCircleFilled(center, r, m_color, numSegments);
        } else {
            m_drawList->AddCircle(center, r, m_color, numSegments, GetThickness());
        }

        if (withTriange) {
            DrawAdditionalTriangle();
        }
    }

    void DrawNgon(int numSegments, bool withTriange) const {
        const auto r = GetRadius();
        const auto center = GetCenter();

        if (m_filled) {
            m_drawList->AddNgonFilled(center, r, m_color, numSegments);
        } else {
            m_drawList->AddNgon(center, r, m_color, numSegments, GetThickness());
        }

        if (withTriange) {
            DrawAdditionalTriangle();
        }
    }

    void DrawSquare(bool rounded, bool withTriange) const {
        const auto r = GetRadius();
        const auto center = GetCenter();
        const auto rounding = rounded ? r * 0.5f : 0;
        const auto p0 = center - ImVec2(r, r);
        const auto p1 = center + ImVec2(r, r);

        if (m_filled) {
            m_drawList->AddRectFilled(p0, p1, m_color, rounding, ImDrawCornerFlags_All);
        } else {
            m_drawList->AddRect(p0, p1, m_color, rounding, ImDrawCornerFlags_All, GetThickness());
        }

        if (withTriange) {
            DrawAdditionalTriangle();
        }
    }

    void DrawTriangle() const {
        const auto r = GetRadius();
        const auto center = GetCenter();
        const auto p0 = center + ImVec2(-r,  r);
        const auto p1 = center + ImVec2(-r, -r);
        const auto p2 = center + ImVec2( r,  0);

        if (m_filled) {
            m_drawList->AddTriangleFilled(p0, p1, p2, m_color);
        } else {
            m_drawList->AddTriangle(p0, p1, p2, m_color, GetThickness());
        }
    }

private:
    ImDrawList* m_drawList;
    const math::RectF m_rect;
    const uint32_t m_color;
    const bool m_filled;
};

float GetThickness(math::RectF rect) {
    return rect.w / 12.0f;
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

    IconDriver driver(window->DrawList, drawRect, style.color.value, filled);
    switch (type) {
    case IconType::Circle:
        driver.DrawCircle(false);
        break;
    case IconType::CircleTriangle:
        driver.DrawCircle(true);
        break;
    case IconType::Hexagon:
        driver.DrawNgon(6, false);
        break;
    case IconType::HexagonTriangle:
        driver.DrawNgon(6, true);
        break;
    case IconType::Tetragon:
        driver.DrawNgon(4, false);
        break;
    case IconType::RoundSquare:
        driver.DrawSquare(true, false);
        break;
    case IconType::RoundSquareTriangle:
        driver.DrawSquare(true, true);
        break;
    case IconType::Square:
        driver.DrawSquare(false, false);
        break;
    case IconType::SquareTriangle:
        driver.DrawSquare(false, true);
        break;
    case IconType::Triangle:
        driver.DrawTriangle();
        break;
    case IconType::Flow:
        DrawFlow(drawList, drawRect, filled, color, fillColor);
        break;
    }

    return widgetRect;
}

} // end namespace gui
