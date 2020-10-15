#include "middleware/gscheme/editor/gs_node.h"

#include "imgui/node_editor.h"
#include "middleware/imgui/label.h"
#include "middleware/imgui/widgets.h"


namespace gs {

void Node::OnStartDrawNode(uintptr_t id, std::string_view prettyName, uint8_t alpha) {
    m_alpha = alpha;
    m_nodeId = id;
    gui::BeginGroup();
    m_headerRect = gui::Label(prettyName);
    gui::EndGroup();

    m_existsInputPins = false;
    m_maxOutputPinNameWidth = m_maxOutputPinNameWidthFrame;
    m_maxOutputPinNameWidthFrame = 0;
}

void Node::OnFinishDrawNode(void* texBackground, math::SizeF texBackgroundSize) {
    const auto headerColor = math::Color(0, 125, 0, m_alpha).value;
    const auto headerLineAlpha = static_cast<uint8_t>(96 * static_cast<int>(m_alpha) / (3 * 255));
    const auto headerLineColor = math::Color(255, 255, 255, headerLineAlpha).value;

    auto headerSize = math::SizeF(ImGui::GetItemRectMax().x - m_headerRect.x, m_headerRect.Height());
    ne::EndNode();

    // Header
    if (ImGui::IsItemVisible()) {
        auto drawList = ne::GetNodeBackgroundDrawList(ne::NodeId(m_nodeId));
        const auto texBackgroundID = reinterpret_cast<ImTextureID>(texBackground);
        const auto rounding = ne::GetStyle().NodeRounding;
        const auto roundingCorners = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight;
        const auto uvMin = ImVec2(0.0f, 0.0f);
        const auto uvMax = ImVec2(headerSize.w / texBackgroundSize.w, headerSize.h / texBackgroundSize.h);

        const auto halfBorderWidth = ne::GetStyle().NodeBorderWidth * 0.5f;
        const auto imageTopLeft = ImVec2(
            m_headerRect.x - m_nodePaddingLeft + halfBorderWidth,
            m_headerRect.y - m_nodePaddingTop + halfBorderWidth);
        const auto imageBottomRight = ImVec2(
            m_headerRect.x + headerSize.w + m_nodePaddingRight - halfBorderWidth,
            m_headerRect.y + headerSize.h + m_nodePaddingTop);
        const auto imageBottomLeft = ImVec2(imageTopLeft.x, imageBottomRight.y);

        drawList->AddImageRounded(texBackgroundID, imageTopLeft, imageBottomRight, uvMin, uvMax, headerColor, rounding, roundingCorners);
        drawList->AddLine(imageBottomLeft, imageBottomRight, headerLineColor, 1.0f);
    }
    ne::PopStyleVar(1);
}

void Node::OnStartDrawInputPins() {
    m_existsInputPins = true;
    gui::BeginGroup();
}

void Node::OnFinishDrawInputPins() {
    gui::EndGroup();
}

void Node::OnStartDrawOutputPins() {
    if (m_existsInputPins) {
        gui::SameLine();
    }
    gui::BeginGroup();
}

void Node::OnFinishDrawOutputPins() {
    gui::EndGroup();
}

void Node::OnDrawPin(uintptr_t id, bool isInput, bool isConnected, std::string_view prettyName) {
    const auto iconSize = math::Size(24, 24);
    const auto pinColor = math::Color(0, 255, 0, m_alpha);
    const auto innerPinColor = math::Color(32, 32, 32, m_alpha);
    gui::LabelStyle inputLabelStyle;
    inputLabelStyle.horisontalAlign = gui::HorisontalAlign::Left;
    inputLabelStyle.verticalAlign = gui::VerticalAlign::Center;
    gui::LabelStyle outputLabelStyle;
    outputLabelStyle.horisontalAlign = gui::HorisontalAlign::Right;
    outputLabelStyle.verticalAlign = gui::VerticalAlign::Center;
    outputLabelStyle.padding.left = m_existsInputPins ? 10.f : 0;
    math::SizeF minSize(0, static_cast<float>(iconSize.h));

     if (isInput) {
        ne::BeginPin(ne::PinId(id), ne::PinKind::Input);
            gui::NodeIcon(iconSize, gui::IconType::Circle, isConnected, pinColor, innerPinColor);
        ne::EndPin();
        gui::SameLine();
        gui::Label(prettyName, inputLabelStyle, minSize);
    } else {
        minSize.w = m_maxOutputPinNameWidth;
        m_maxOutputPinNameWidthFrame = std::max(m_maxOutputPinNameWidthFrame, gui::Label(prettyName, outputLabelStyle, minSize).w);
        gui::SameLine();
        ne::BeginPin(ne::PinId(id), ne::PinKind::Output);
            gui::NodeIcon(iconSize, gui::IconType::Circle, isConnected, pinColor, innerPinColor);
        ne::EndPin();
    }
}

}
