#include "middleware/imgui/widgets.h"

#include "middleware/imgui/imgui_math.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>


math::Rectf Image(dg::ITextureView* texture, math::Size size, bool isOpenGL, math::Pointf uv0, math::Pointf uv1, math::Color tintCol) {

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return math::Rectf();
    }

    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ToImGui(size));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, 0)) {
        return math::Rectf();
    }

    if (isOpenGL) {
        std::swap(uv0.y, uv1.y);
    }

    window->DrawList->AddImage(reinterpret_cast<ImTextureID>(texture), bb.Min, bb.Max, ToImGui(uv0), ToImGui(uv1), tintCol.value);

    return math::Rectf(ToPointf(bb.Min), ToPointf(bb.Max));
}
