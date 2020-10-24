#include "middleware/gscheme/editor/gs_draw_node.h"

#include <string>
#include <algorithm>

#include "imgui/imgui.h"
#include "imgui/node_editor.h"
#include "middleware/imgui/icon.h"
#include "middleware/imgui/label.h"
#include "middleware/imgui/layout.h"
#include "middleware/imgui/imgui_math.h"


namespace gs {

void DrawNode::OnStartDrawNode(uintptr_t id, std::string_view prettyName, uint8_t alpha) {
    m_nodeId = id;
    m_alpha = alpha;

    ne::BeginNode(ne::NodeId(id));
    gui::BeginVertical();

    gui::BeginHorizontal();

    gui::LabelStyle headerStyle;
    headerStyle.margin.left = 0;
    headerStyle.margin.right += 4.f;
    headerStyle.margin.top = 0;
    headerStyle.margin.bottom += ne::GetStyle().NodePadding.y; // NodePadding.top;
    gui::Label(prettyName, headerStyle);
    auto headerRect = gui::EndHorizontal();
    m_headerWidth = headerRect.Width();
    m_headerBottom = headerRect.Bottom();

    gui::BeginHorizontal();
}

void DrawNode::OnFinishDrawNode(bool isValid, void* texBackground, math::SizeF texBackgroundSize) {
    auto nodePartWidht = gui::EndHorizontal().w;

    float footerRectTop = 0;
    if (!isValid) {
        gui::LabelStyle footerStyle;
        footerStyle.horisontalAlign = gui::HorisontalAlign::Center;
        footerStyle.padding.left = 0;
        footerStyle.padding.top += ne::GetStyle().NodePadding.w; // NodePadding.bottom;
        footerStyle.minSize = math::SizeF(std::max(nodePartWidht, m_headerWidth), 0);
        math::RectF footerRect;
        gui::Label("Error", footerStyle, &footerRect);
        footerRectTop = footerRect.Top();
    }

    gui::EndVertical();

    ne::EndNode();
    auto nodeRect = math::RectF(gui::ToPointF(ImGui::GetItemRectMin()), gui::ToPointF(ImGui::GetItemRectMax()));

    if (!ImGui::IsItemVisible()) {
        return;
    }

    const auto headerColor = math::Color(0, 125, 0, m_alpha).value;
    const auto footerColor = math::Color(125, 0, 0, m_alpha).value;
    const auto lineAlpha = static_cast<uint8_t>(96 * static_cast<int>(m_alpha) / (3 * 255));
    const auto lineColor = math::Color(255, 255, 255, lineAlpha).value;

    auto drawList = ne::GetNodeBackgroundDrawList(ne::NodeId(m_nodeId));
    const auto texBackgroundID = reinterpret_cast<ImTextureID>(texBackground);
    const auto rounding = ne::GetStyle().NodeRounding;

    auto headerRect = nodeRect;
    headerRect.Bottom(m_headerBottom);

    const auto headerLeftTop = gui::ToImGui(headerRect.LeftTop());
    const auto headerRightBottom = gui::ToImGui(headerRect.RightBottom());
    const auto headerLeftBottom = gui::ToImGui(headerRect.LeftBottom());

    const auto uvMin = ImVec2(0.0f, 0.0f);
    const auto uvMax = ImVec2(headerRect.w / texBackgroundSize.w, headerRect.h / texBackgroundSize.h);

    drawList->AddImageRounded(texBackgroundID, headerLeftTop, headerRightBottom, uvMin, uvMax, headerColor, rounding, ImDrawCornerFlags_Top);
    drawList->AddLine(headerLeftBottom, headerRightBottom, lineColor, 1.0f);

    if (!isValid) {
        auto footerRect = nodeRect;
        footerRect.Top(footerRectTop);
        const auto footerLeftTop = gui::ToImGui(footerRect.LeftTop());
        const auto footerRightBottom = gui::ToImGui(footerRect.RightBottom());
        const auto footerRightTop = gui::ToImGui(footerRect.RightTop());

        drawList->AddRectFilled(footerLeftTop, footerRightBottom, footerColor, rounding, ImDrawCornerFlags_Bot);
        drawList->AddLine(footerLeftTop, footerRightTop, lineColor, 1.0f);
    }
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
    iconStyle.margin.left = 0;
    iconStyle.margin.right = gui::Style::DEFUALT_MARGIN.left;

    gui::LabelStyle labelStyle;
    labelStyle.horisontalAlign = gui::HorisontalAlign::Left;
    labelStyle.verticalAlign = gui::VerticalAlign::Center;
    labelStyle.margin.left = 0;
    labelStyle.margin.right = gui::Style::DEFUALT_MARGIN.left;
    labelStyle.minSize = math::SizeF(0, m_iconSideSize);

    for (const auto& pin: pins) {
        ne::BeginPin(ne::PinId(pin.id), ne::PinKind::Input);
            gui::Icon(gui::IconType::CircleTriangle, pin.isConnected, iconStyle);
        ne::EndPin();
        gui::SameLine();
        gui::Label(pin.prettyName, labelStyle);
    }

    m_inputPinsWidth = gui::EndVertical().Width();
}

void DrawNode::OnDrawPreview() {
    float dt = m_headerWidth - m_inputPinsWidth - m_outputPinsWidth;
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

    float labelWidth = 0;
    for (const auto& pin: pins) {
        labelWidth = std::max(labelWidth, gui::LabelCalc(pin.prettyName, labelStyle).w);
    }

    labelStyle.minSize = math::SizeF(labelWidth, m_iconSideSize);

    for (const auto& pin: pins) {
        gui::Label(pin.prettyName, labelStyle);
        gui::SameLine();
        ne::BeginPin(ne::PinId(pin.id), ne::PinKind::Output);
            gui::Icon(gui::IconType::CircleTriangle, pin.isConnected, iconStyle);
        ne::EndPin();
    }

    m_outputPinsWidth = gui::EndVertical().Width();
}

}
