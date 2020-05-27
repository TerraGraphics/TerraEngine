#include "middleware/gscheme/editor/gs_node.h"

#include <vector>
#include <memory>

#include "core/math/types.h"
#include "middleware/imgui/imgui.h"
#include "middleware/gscheme/rttr/variant.h"
#include "middleware/gscheme/editor/gs_id.h"
#include "middleware/gscheme/editor/gs_pin.h"
#include "middleware/imgui/imgui_node_editor.h"


struct GSNode::Impl {
    Impl(uintptr_t id, std::string_view name, const rttr::type& nodeType);

    uintptr_t m_id;
    std::string_view m_name;
    rttr::type m_type;
    rttr::variant m_instance;
    std::vector<std::unique_ptr<GSInputPin>> m_inputPins;
    std::vector<std::unique_ptr<GSOutputPin>> m_outputPins;
};

GSNode::Impl::Impl(uintptr_t id, std::string_view name, const rttr::type& nodeType)
    : m_id(id)
    , m_name(name)
    , m_type(nodeType)
    , m_instance(nodeType.create()) {

    auto props = m_type.get_properties();
    m_inputPins.reserve(props.size());
    for(const auto& prop : props) {
        m_inputPins.push_back(std::make_unique<GSInputPin>(GSGetNextID(), prop.get_name().to_string()));
    }

    auto methods = m_type.get_methods();
    m_outputPins.reserve(methods.size());
    for(const auto& method : methods) {
        m_outputPins.push_back(std::make_unique<GSOutputPin>(GSGetNextID(), method.get_name().to_string()));
    }
}

GSNode::GSNode(uintptr_t id, std::string_view name, const rttr::type& nodeType)
    : impl(id, name, nodeType) {

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
    ne::NodeId id(impl->m_id);
    ne::BeginNode(id);

    // Header
    ImGui::BeginGroup();
    ImGui::TextUnformatted(impl->m_name.data());
    auto headerMin = ImGui::GetItemRectMin();
    auto headerMax = ImGui::GetItemRectMax();
    auto headerSize = ImGui::GetItemRectSize();
    ImGui::Dummy(ImVec2(1.0f, nodePaddingTop));
    ImGui::EndGroup();

    // Input pins
    if (!impl->m_inputPins.empty()) {
        ImGui::BeginGroup();
        for(const auto& pin: impl->m_inputPins) {
            pin->Draw(alpha);
        }
        ImGui::EndGroup();
        ImGui::SameLine();
    }

    // Output pins
    if (!impl->m_outputPins.empty()) {
        ImGui::BeginGroup();
        for(const auto& pin: impl->m_outputPins) {
            pin->Draw(alpha);
        }
        ImGui::EndGroup();
    }

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
