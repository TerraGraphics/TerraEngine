#include "middleware/gscheme/editor/gs_node.h"

#include <utility>

#include "core/math/types.h"
#include "middleware/imgui/imgui.h"
#include "middleware/gscheme/rttr/variant.h"
#include "middleware/imgui/imgui_node_editor.h"


struct GSNode::Impl {
    Impl(const std::string& name, rttr::variant&& instance);

    std::string m_name;
    rttr::variant m_instance;
};

GSNode::Impl::Impl(const std::string& name, rttr::variant&& instance)
    : m_name(name)
    , m_instance(std::move(instance)) {

}

GSNode::GSNode(const std::string& name, rttr::variant&& instance)
    : impl(name, std::move(instance)) {

}

GSNode::~GSNode() {

}

void GSNode::Draw(uint8_t alpha, TextureViewRaw texBackground, float texWidth, float texHeight) {
    const float nodePaddingLeft = 8;
    const float nodePaddingRight = 8;
    const float nodePaddingTop = 4;
    const float nodePaddingBottom = 8;
    const auto headerColor = math::Color(0, 125, 0, alpha).value;
    const auto headerLineAlpha = static_cast<uint8_t>(96 * static_cast<int>(alpha) / (3 * 255));
    const auto headerLineColor = math::Color(255, 255, 255, headerLineAlpha).value;


    ne::PushStyleVar(ne::StyleVar_NodePadding, ImVec4(nodePaddingLeft, nodePaddingTop, nodePaddingRight, nodePaddingBottom));
    ne::NodeId id(this);
    ne::BeginNode(id);

    // Header
    ImGui::BeginGroup();
    ImGui::TextUnformatted(impl->m_name.c_str());
    auto headerMin = ImGui::GetItemRectMin();
    auto headerMax = ImGui::GetItemRectMax();
    auto headerSize = ImGui::GetItemRectSize();
    ImGui::Dummy(ImVec2(1.0f, nodePaddingTop));
    ImGui::EndGroup();

    ne::EndNode();

    // Header
    if (ImGui::IsItemVisible()) {
        auto drawList = ne::GetNodeBackgroundDrawList(id);
        const auto texBackgroundID = reinterpret_cast<ImTextureID>(texBackground);
        const auto rounding = ne::GetStyle().NodeRounding;
        const auto roundingCorners = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight;
        const auto uvMin = ImVec2(0.0f, 0.0f);
        const auto uvMax = ImVec2(headerSize.x / texWidth, headerSize.y / texHeight);

        const auto halfBorderWidth = ne::GetStyle().NodeBorderWidth * 0.5f;
        const auto imageTopLeft = ImVec2(
            headerMin.x - nodePaddingLeft + halfBorderWidth,
            headerMin.y - nodePaddingTop + halfBorderWidth);
        const auto imageBottomRight = ImVec2(
            headerMax.x + nodePaddingRight - halfBorderWidth,
            headerMax.y + nodePaddingTop);
        const auto imageBottomLeft = ImVec2(imageTopLeft.x, imageBottomRight.y);

        drawList->AddImageRounded(texBackgroundID, imageTopLeft, imageBottomRight, uvMin, uvMax, headerColor, rounding, roundingCorners);
        drawList->AddLine(imageBottomLeft, imageBottomRight, headerLineColor, 1.0f);
    }
    ne::PopStyleVar(1);
}
