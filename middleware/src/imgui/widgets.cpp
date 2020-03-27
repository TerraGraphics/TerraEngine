#include "middleware/imgui/widgets.h"

#include <utility>
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include "middleware/imgui/imgui_math.h"


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
