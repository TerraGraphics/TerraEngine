#include "middleware/gscheme/editor/gs_draw_node.h"

#include "imgui/node_editor.h"
#include "middleware/imgui/icon.h"
#include "middleware/imgui/label.h"
#include "middleware/imgui/layout.h"
#include "middleware/imgui/imgui_math.h"


namespace gs {

void DrawNode::OnStartDrawNode(uintptr_t id, std::string_view prettyName, uint8_t alpha) {
    m_alpha = alpha;
    m_nodeId = id;

    ne::PushStyleVar(ne::StyleVar_NodePadding, gui::ToImGui(m_nodePadding));
    ne::BeginNode(ne::NodeId(id));
    gui::BeginVertical();

    gui::LabelStyle headerStyle;
    headerStyle.padding.bottom = m_nodePadding.top;
    m_headerSize = gui::Label(prettyName, headerStyle).Size();

    gui::BeginHorizontal();
}

void DrawNode::OnFinishDrawNode(void* texBackground, math::SizeF texBackgroundSize) {
    const auto nodeInnerPadding = m_nodePadding - math::RectOffsetF(ne::GetStyle().NodeBorderWidth * 0.5f);

    gui::EndHorizontal();
    auto nodeRect = gui::EndVertical() + nodeInnerPadding;

    ne::EndNode();
    ne::PopStyleVar(1);

    if (!ImGui::IsItemVisible()) {
        return;
    }

    const auto headerColor = math::Color(0, 125, 0, m_alpha).value;
    const auto headerLineAlpha = static_cast<uint8_t>(96 * static_cast<int>(m_alpha) / (3 * 255));
    const auto headerLineColor = math::Color(255, 255, 255, headerLineAlpha).value;

    auto drawList = ne::GetNodeBackgroundDrawList(ne::NodeId(m_nodeId));
    const auto texBackgroundID = reinterpret_cast<ImTextureID>(texBackground);
    const auto rounding = ne::GetStyle().NodeRounding;
    const auto roundingCorners = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight;

    auto headerRect = nodeRect;
    headerRect.h = m_headerSize.h + nodeInnerPadding.top;

    const auto headerTopLeft = gui::ToImGui(headerRect.LeftTop());
    const auto headerBottomRight = gui::ToImGui(headerRect.RightBottom());
    const auto headerBottomLeft = gui::ToImGui(headerRect.LeftBottom());

    const auto uvMin = ImVec2(0.0f, 0.0f);
    const auto uvMax = ImVec2(headerRect.w / texBackgroundSize.w, headerRect.h / texBackgroundSize.h);

    drawList->AddImageRounded(texBackgroundID, headerTopLeft, headerBottomRight, uvMin, uvMax, headerColor, rounding, roundingCorners);
    drawList->AddLine(headerBottomLeft, headerBottomRight, headerLineColor, 1.0f);
}

void DrawNode::OnDrawInputPins(const std::vector<IDraw::Pin>& pins) {
    if (pins.empty()) {
        m_inputPinsWidth = 0.f;
        return;
    }

    gui::BeginVertical();

    gui::IconStyle iconStyle;
    iconStyle.sideSize = m_iconSideSize;
    iconStyle.color = math::Color(0, 255, 0, m_alpha);

    gui::LabelStyle labelStyle;
    labelStyle.horisontalAlign = gui::HorisontalAlign::Left;
    labelStyle.verticalAlign = gui::VerticalAlign::Center;
    labelStyle.padding.right = 5.f;

    for (const auto& pin: pins) {
        ne::BeginPin(ne::PinId(pin.id), ne::PinKind::Input);
            gui::Icon(gui::IconType::CircleTriangle, pin.isConnected, iconStyle);
        ne::EndPin();
        gui::SameLine();
        gui::Label(pin.prettyName, labelStyle, math::SizeF(0, m_iconSideSize));
    }

    m_inputPinsWidth = gui::EndVertical().Width();
}

void DrawNode::OnDrawPreview() {
    float dt = m_headerSize.w - m_inputPinsWidth - m_outputPinsWidth;
    if (dt <= 0) {
        return;
    }

    gui::Dummy(math::SizeF(dt, 0));
}

void DrawNode::OnDrawOutputPins(const std::vector<IDraw::Pin>& pins) {
    if (pins.empty()) {
        m_outputPinsWidth = 0.f;
        return;
    }

    gui::BeginVertical();

    gui::IconStyle iconStyle;
    iconStyle.sideSize = m_iconSideSize;
    iconStyle.color = math::Color(0, 255, 0, m_alpha);

    gui::LabelStyle labelStyle;
    labelStyle.horisontalAlign = gui::HorisontalAlign::Right;
    labelStyle.verticalAlign = gui::VerticalAlign::Center;
    labelStyle.padding.left = 5.f;

    float outputPinsLabelWidth = 0;
    for (const auto& pin: pins) {
        auto labelRect = gui::Label(pin.prettyName, labelStyle, math::SizeF(m_outputPinsLabelWidth, m_iconSideSize));
        outputPinsLabelWidth = std::max(outputPinsLabelWidth, labelRect.w);
        gui::SameLine();
        ne::BeginPin(ne::PinId(pin.id), ne::PinKind::Output);
            gui::Icon(gui::IconType::CircleTriangle, pin.isConnected, iconStyle);
        ne::EndPin();
    }
    m_outputPinsLabelWidth = outputPinsLabelWidth;

    m_outputPinsWidth = gui::EndVertical().Width();
}

}
