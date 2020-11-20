#include "middleware/imgui/image.h"

#include <utility>

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/widget_draw.h"
#include "imgui/widget_placement.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

class IconDrawer {
public:
    IconDrawer(ImDrawList* drawList, math::RectF rect, uint32_t color, bool filled) noexcept
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

    float GetOffset() const noexcept {
        return m_rect.w / 6.f;
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

    void DrawFlow() const {
        const auto originScale = m_rect.w / 24.0f;
        const auto offsetX  = 1.0f * originScale;
        const auto offsetY  = 0.0f * originScale;
        const auto margin   = (m_filled ? 0.0f : 1.5f) * originScale;
        const auto rounding = 0.1f * originScale;
        const auto tipRound = 0.7f; // percentage of triangle edge (for tip)

        const auto canvas = math::RectF(
            m_rect.x + margin + offsetX,
            m_rect.y + margin + offsetY,
            m_rect.w - margin * 2.0f,
            m_rect.h - margin * 2.0f);

        const auto left    = canvas.x + canvas.w            * 0.5f * 0.3f;
        const auto right   = canvas.x + canvas.w - canvas.w * 0.5f * 0.3f;
        const auto top     = canvas.y + canvas.h            * 0.5f * 0.2f;
        const auto bottom  = canvas.y + canvas.h - canvas.h * 0.5f * 0.2f;
        const auto centerY = (top + bottom) * 0.5f;

        const auto leftTop    = ImVec2(left, top);
        const auto leftBottom = ImVec2(left, bottom);
        const auto tipTop     = ImVec2(canvas.x + canvas.w * 0.5f, top);
        const auto tipRight   = ImVec2(right, centerY);
        const auto tipBottom  = ImVec2(canvas.x + canvas.w * 0.5f, bottom);

        m_drawList->PathLineTo(leftTop + ImVec2(0, rounding));
        m_drawList->PathBezierCurveTo(
            leftTop,
            leftTop,
            leftTop + ImVec2(rounding, 0));
        m_drawList->PathLineTo(tipTop);
        m_drawList->PathLineTo(tipTop + (tipRight - tipTop) * tipRound);
        m_drawList->PathBezierCurveTo(
            tipRight,
            tipRight,
            tipBottom + (tipRight - tipBottom) * tipRound);
        m_drawList->PathLineTo(tipBottom);
        m_drawList->PathLineTo(leftBottom + ImVec2(rounding, 0));
        m_drawList->PathBezierCurveTo(
            leftBottom,
            leftBottom,
            leftBottom - ImVec2(0, rounding));

        if (m_filled) {
            m_drawList->PathFillConvex(m_color);
        } else {
            m_drawList->PathStroke(m_color, true, GetThickness());
        }
    }

private:
    ImDrawList* m_drawList;
    const math::RectF m_rect;
    const uint32_t m_color;
    const bool m_filled;
};

void Icon(IconType type, bool filled, const IconStyle& style, math::RectF* outRect) {
    ImGuiWindow* window = GetCheckedCurrentWindow(outRect);
    if (window == nullptr) {
        return;
    }

    const math::SizeF drawSize(style.sideSize, style.sideSize);

    math::RectF drawRect;
    if (!WidgetPlace(0, &style, drawSize, &drawRect, outRect)) {
        return;
    }

    if (!IsRectVisible(drawRect)) {
        return;
    }

    IconDrawer drawer(window->DrawList, drawRect, style.color.value, filled);
    switch (type) {
    case IconType::Circle:
        drawer.DrawCircle(false);
        break;
    case IconType::CircleTriangle:
        drawer.DrawCircle(true);
        break;
    case IconType::Hexagon:
        drawer.DrawNgon(6, false);
        break;
    case IconType::HexagonTriangle:
        drawer.DrawNgon(6, true);
        break;
    case IconType::Tetragon:
        drawer.DrawNgon(4, false);
        break;
    case IconType::RoundSquare:
        drawer.DrawSquare(true, false);
        break;
    case IconType::RoundSquareTriangle:
        drawer.DrawSquare(true, true);
        break;
    case IconType::Square:
        drawer.DrawSquare(false, false);
        break;
    case IconType::SquareTriangle:
        drawer.DrawSquare(false, true);
        break;
    case IconType::Triangle:
        drawer.DrawTriangle();
        break;
    case IconType::Flow:
        drawer.DrawFlow();
        break;
    }
    DrawTooltip(&style);
}

void Image(math::SizeF drawSize, const ImageStyle& style, math::RectF* outRect) {
    ImGuiWindow* window = GetCheckedCurrentWindow(outRect);
    if (window == nullptr) {
        return;
    }

    math::RectF drawRect;
    if (!WidgetPlace(0, &style, drawSize, &drawRect, outRect)) {
        return;
    }

    window->DrawList->AddRectFilled(ToImGui(drawRect.Min()), ToImGui(drawRect.Max()), style.color.value);
    DrawTooltip(&style);
}

void Image(math::SizeF drawSize, TextureViewPtr texture, const ImageStyle& style, math::RectF* outRect) {
    ImGuiWindow* window = GetCheckedCurrentWindow(outRect);
    if (window == nullptr) {
        return;
    }

    math::RectF drawRect;
    if (!WidgetPlace(0, &style, drawSize, &drawRect, outRect)) {
        return;
    }

    auto uv0 = ToImGui(style.uv.Min());
    auto uv1 = ToImGui(style.uv.Max());
    if (isOpenGL()) {
        std::swap(uv0.y, uv1.y);
    }

    window->DrawList->AddImage(reinterpret_cast<ImTextureID>(texture.RawPtr()), ToImGui(drawRect.Min()), ToImGui(drawRect.Max()), uv0, uv1, style.color.value);
    DrawTooltip(&style);
}

} // end namespace gui
