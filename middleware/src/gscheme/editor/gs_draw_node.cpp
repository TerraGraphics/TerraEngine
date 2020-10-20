#include "middleware/gscheme/editor/gs_draw_node.h"

#include "imgui/node_editor.h"
#include "middleware/imgui/icon.h"
#include "middleware/imgui/label.h"
#include "middleware/imgui/layout.h"


namespace gs {

void DrawNode::OnStartDrawNode(uintptr_t id, std::string_view prettyName, uint8_t alpha) {
    m_alpha = alpha;
    m_nodeId = id;
    m_addSameLine = false;

    ne::PushStyleVar(ne::StyleVar_NodePadding, ImVec4(m_nodePaddingLeft, m_nodePaddingTop, m_nodePaddingRight, m_nodePaddingBottom));
    ne::BeginNode(ne::NodeId(id));

    gui::BeginGroup();
    m_nodeRect = gui::Label(prettyName);
    m_headerSize = m_nodeRect.Size();
    gui::EndGroup();
}

void DrawNode::OnFinishDrawNode(void* texBackground, math::SizeF texBackgroundSize) {
    const auto headerColor = math::Color(0, 125, 0, m_alpha).value;
    const auto headerLineAlpha = static_cast<uint8_t>(96 * static_cast<int>(m_alpha) / (3 * 255));
    const auto headerLineColor = math::Color(255, 255, 255, headerLineAlpha).value;

    auto headerSize = math::SizeF(m_nodeRect.w, m_headerSize.h);
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
            m_nodeRect.x - m_nodePaddingLeft + halfBorderWidth,
            m_nodeRect.y - m_nodePaddingTop + halfBorderWidth);
        const auto imageBottomRight = ImVec2(
            m_nodeRect.x + headerSize.w + m_nodePaddingRight - halfBorderWidth,
            m_nodeRect.y + headerSize.h + m_nodePaddingTop);
        const auto imageBottomLeft = ImVec2(imageTopLeft.x, imageBottomRight.y);

        drawList->AddImageRounded(texBackgroundID, imageTopLeft, imageBottomRight, uvMin, uvMax, headerColor, rounding, roundingCorners);
        drawList->AddLine(imageBottomLeft, imageBottomRight, headerLineColor, 1.0f);
    }
    ne::PopStyleVar(1);
}

void DrawNode::OnDrawInputPins(const std::vector<IDraw::Pin>& pins) {
    if (pins.empty()) {
        m_inputPinsWidth = 0.f;
        return;
    }

    m_addSameLine = true;

    gui::BeginGroup();

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

    const auto rect = gui::EndGroup();
    m_nodeRect += rect;
    m_inputPinsWidth = rect.Width();
}

void DrawNode::OnDrawPreview() {
    float dt = m_headerSize.w - m_inputPinsWidth - m_outputPinsWidth;
    if (dt <= 0) {
        return;
    }

    if (m_addSameLine) {
        gui::SameLine();
    }
    m_addSameLine = true;

    gui::BeginGroup();
    gui::Dummy(math::SizeF(dt, 0));
    m_nodeRect += gui::EndGroup();
}

void DrawNode::OnDrawOutputPins(const std::vector<IDraw::Pin>& pins) {
    if (pins.empty()) {
        m_outputPinsWidth = 0.f;
        return;
    }

    if (m_addSameLine) {
        gui::SameLine();
    }
    m_addSameLine = true;

    gui::BeginGroup();

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

    const auto rect = gui::EndGroup();
    m_nodeRect += rect;
    m_outputPinsWidth = rect.Width();
}

}
