#include "middleware/imgui/widgets.h"

#include <imgui.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>


static inline ImVec2 ToImGui(math::Size value) {
    return ImVec2(static_cast<float>(value.w), static_cast<float>(value.h));
}

static inline ImVec2 ToImGui(math::Pointf value) {
    return ImVec2(value.x, value.y);
}

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

    return math::Rectf(math::Pointf(bb.Min.x, bb.Min.y), math::Pointf(bb.Max.x, bb.Max.y));
}
