#include "middleware/gschema/editor/gs_draw_node.h"

#include <string>
#include <utility>
#include <algorithm>

#include "imgui/imgui.h"
#include "imgui/node_editor.h"
#include "middleware/imgui/image.h"
#include "middleware/imgui/label.h"
#include "middleware/imgui/button.h"
#include "middleware/imgui/layout.h"
#include "middleware/imgui/imgui_math.h"
#include "middleware/gschema/editor/gs_draw_preview.h"


namespace gs {

DrawNode::DrawNode(DrawNode&& o) noexcept {
    std::swap(m_nodeId, o.m_nodeId);
    std::swap(m_drawed, o.m_drawed);
    std::swap(m_showPreview, o.m_showPreview);
    std::swap(m_alpha, o.m_alpha);
    std::swap(m_preview, o.m_preview);

    std::swap(m_headerWidth, o.m_headerWidth);
    std::swap(m_headerBottom, o.m_headerBottom);
    std::swap(m_inputPinsWidth, o.m_inputPinsWidth);
    std::swap(m_outputPinsWidth, o.m_outputPinsWidth);
}

DrawNode::~DrawNode() {
    if (m_preview != nullptr) {
        delete m_preview;
    }
}

void DrawNode::OnStartDrawGraph() {
    if (!m_drawed) {
        if (m_preview != nullptr) {
            m_preview->Reset();
        }
        m_showPreview = false;
        m_headerWidth = 0.f;
        m_headerBottom = 0.f;
        m_inputPinsWidth = 0.f;
        m_outputPinsWidth = 0.f;
    }

    m_drawed = false;
}

void DrawNode::OnStartDrawNode(uintptr_t id, std::string_view displayName, uint8_t alpha, bool fullPreview) {
    m_nodeId = id;
    m_drawed = true;
    m_alpha = alpha;

    ne::BeginNode(ne::NodeId(id));
    gui::BeginVertical();

    // draw header
    gui::BeginHorizontal();

    math::RectF headerLabelRect;
    gui::LabelStyle labelStyle;
    labelStyle.margin.top = 2.f;
    labelStyle.margin.bottom = ne::GetStyle().NodePadding.y; // NodePadding.top;

    if (fullPreview) {
        gui::BeginVertical();
        gui::Label(displayName, labelStyle);
        gui::Label("Previewing...", labelStyle);
        headerLabelRect = gui::EndVertical();
    } else {
        gui::Label(displayName, labelStyle, &headerLabelRect);
    }
    gui::SameLine();

    float requiredWidth = m_inputPinsWidth + m_outputPinsWidth;
    if (m_showPreview) {
        requiredWidth += m_previewSize.w;
    }

    math::RectF headerButtonRect;
    gui::ButtonStyle buttonStyle;
    buttonStyle.horisontalAlign = gui::HorisontalAlign::Right;
    buttonStyle.margin.left = 4.f;
    buttonStyle.margin.bottom = 1.f;
    buttonStyle.drawSize = math::SizeF(ImGui::GetFontSize() + labelStyle.margin.Vertical());
    buttonStyle.availableSize.w = std::max(requiredWidth - headerLabelRect.Width(), buttonStyle.drawSize.w + buttonStyle.margin.Horizontal());

    std::string buttonId(std::to_string(id) + ".show_pin_preview");
    auto dir = m_showPreview ? gui::Direction::Up : gui::Direction::Down;
    if (gui::ArrowButton(buttonId, dir, buttonStyle, &headerButtonRect)) {
        m_showPreview = !m_showPreview;
    }

    const math::RectF headerRect = gui::EndHorizontal();
    m_headerWidth = headerRect.Width();
    m_headerBottom = headerRect.Bottom();

    gui::BeginHorizontal();
}

void DrawNode::OnFinishDrawNode(bool isValid, std::string_view errorMessage, void* texBackground, math::SizeF texBackgroundSize) {
    auto nodePartWidht = gui::EndHorizontal().w;

    float footerRectTop = 0;
    if (!isValid) {
        gui::LabelStyle footerStyle;
        footerStyle.horisontalAlign = gui::HorisontalAlign::Center;
        footerStyle.margin.top = ne::GetStyle().NodePadding.w; // NodePadding.bottom;
        footerStyle.availableSize.w = std::max(nodePartWidht, m_headerWidth);
        footerStyle.tooltip = errorMessage;
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

    // TODO: add OpenGL check
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
    iconStyle.margin.right = 8.f;

    gui::LabelStyle labelStyle;
    labelStyle.horisontalAlign = gui::HorisontalAlign::Left;
    labelStyle.verticalAlign = gui::VerticalAlign::Center;
    labelStyle.margin.right = 8.f;
    labelStyle.availableSize.h = m_iconSideSize;

    for (const auto& pin: pins) {
        ne::BeginPin(ne::PinId(pin.id), ne::PinKind::Input);
            gui::Icon(gui::IconType::CircleTriangle, pin.isConnected, iconStyle);
        ne::EndPin();
        gui::SameLine();
        gui::Label(pin.displayName, labelStyle);
    }

    m_inputPinsWidth = gui::EndVertical().Width();
}

void DrawNode::OnDrawPreview(TypeId valueTypeId, const cpgf::GVariant& value, uint8_t valueVersion) {
    float dt = m_headerWidth - m_inputPinsWidth - m_outputPinsWidth;
    if (!m_showPreview) {
        if (dt > 0) {
            gui::Dummy(math::SizeF(dt, 0));
        }
        return;
    }

    dt = (dt - m_previewSize.w) * 0.5f;

    gui::ImageStyle previewStyle;
    previewStyle.margin = (dt > 0) ? math::RectOffsetF(dt, dt, 0, 0) : math::RectOffsetF();

    if (m_preview == nullptr) {
        m_preview = new DrawPreview(false);
    }
    m_preview->Draw(valueTypeId, value, valueVersion, previewStyle, m_previewSize);
}

void DrawNode::OnDrawOutputPins(const std::vector<IDraw::Pin>& pins) {
    if (pins.empty()) {
        m_outputPinsWidth = 0.f;
        return;
    }

    gui::BeginVertical();

    gui::IconStyle iconStyle;
    iconStyle.margin.left = 8.f;
    iconStyle.sideSize = m_iconSideSize;
    iconStyle.color = math::Color(0, 255, 0, m_alpha);

    gui::LabelStyle labelStyle;
    labelStyle.margin.left = 8.f;
    labelStyle.horisontalAlign = gui::HorisontalAlign::Right;
    labelStyle.verticalAlign = gui::VerticalAlign::Center;

    float labelWidth = 0;
    for (const auto& pin: pins) {
        labelWidth = std::max(labelWidth, gui::LabelCalc(pin.displayName, labelStyle).w);
    }

    labelStyle.availableSize = math::SizeF(labelWidth, m_iconSideSize);

    for (const auto& pin: pins) {
        gui::Label(pin.displayName, labelStyle);
        gui::SameLine();
        ne::BeginPin(ne::PinId(pin.id), ne::PinKind::Output);
            gui::Icon(gui::IconType::CircleTriangle, pin.isConnected, iconStyle);
        ne::EndPin();
    }

    m_outputPinsWidth = gui::EndVertical().Width();
}

}
