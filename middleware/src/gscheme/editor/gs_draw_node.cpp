#include "middleware/gscheme/editor/gs_draw_node.h"

#include <string>
#include <algorithm>

#include "eigen/core.h"
#include "dg/texture.h"
#include "imgui/imgui.h"
#include "cpgf/variant.h"
#include "imgui/node_editor.h"
#include "core/common/exception.h"
#include "middleware/imgui/image.h"
#include "middleware/imgui/label.h"
#include "middleware/imgui/button.h"
#include "middleware/imgui/layout.h"
#include "core/math/generator_type.h"
#include "middleware/imgui/imgui_math.h"
#include "middleware/generator/texture/section_plane.h"
#include "middleware/generator/texture/generator2d_to_texture.h"


namespace gs {

DrawNode::~DrawNode() {
    if (m_texture) {
        m_texture->Release();
    }
    if (m_textureGenerator != nullptr) {
        delete m_textureGenerator;
    }
}

void DrawNode::OnStartDrawNode(uintptr_t id, std::string_view prettyName, uint8_t alpha) {
    m_nodeId = id;
    m_alpha = alpha;

    ne::BeginNode(ne::NodeId(id));
    gui::BeginVertical();

    // draw header
    gui::BeginHorizontal();

    math::RectF headerRect;
    gui::LabelStyle headerStyle;
    headerStyle.margin.left = 0;
    headerStyle.margin.bottom += ne::GetStyle().NodePadding.y; // NodePadding.top;
    gui::Label(prettyName, headerStyle, &headerRect);
    gui::SameLine();

    math::RectF buttonRect;
    gui::ButtonStyle buttonStyle;
    buttonStyle.margin.left = 4.f;
    buttonStyle.margin.bottom = headerStyle.padding.bottom;

    float dt = m_inputPinsWidth + m_outputPinsWidth - headerRect.Width() - m_headerButtonWidth - buttonStyle.margin.Horizontal();
    if (m_showPinPreview) {
        dt += m_pinPreviewSize.w;
    }

    if (dt > 0) {
        buttonStyle.margin.left += dt;
    }

    std::string buttonId(std::to_string(id) + "##show_pin_preview");
    auto dir = m_showPinPreview ? gui::ButtonDir::Up : gui::ButtonDir::Down;
    if (gui::ButtonArrow(buttonId, dir, buttonStyle, &buttonRect)) {
        m_showPinPreview = !m_showPinPreview;
    }

    headerRect = gui::EndHorizontal();
    m_headerWidth = headerRect.Width();
    m_headerBottom = headerRect.Bottom();
    m_headerButtonWidth = buttonRect.Width();

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

static uint8_t floatChannelToUint8(float v) {
    return static_cast<uint8_t>(std::max(std::min(static_cast<int32_t>(v * 255.f), 255), 0));
}

void DrawNode::OnDrawPinPreview(TypeId typeId, const cpgf::GVariant& value) {
    float dt = m_headerWidth - m_inputPinsWidth - m_outputPinsWidth;
    if (!m_showPinPreview) {
        if (dt > 0) {
            gui::Dummy(math::SizeF(dt, 0));
        }
        return;
    }

    dt = (dt - m_pinPreviewSize.w) * 0.5f;

    gui::ImageStyle style;
    style.margin = (dt > 0) ? math::RectOffsetF(dt, dt, 0, 0) : math::RectOffsetF();
    style.padding = math::RectOffsetF();

    if (typeId == TypeId::Float) {
        const auto tmp = cpgf::fromVariant<float>(value);
        style.color.red = floatChannelToUint8(tmp);
        style.color.green = 0;
        style.color.blue = 0;
        gui::Image(m_pinPreviewSize, style);
    } else if (typeId == TypeId::Vector2f) {
        const auto tmp = cpgf::fromVariant<Eigen::Vector2f>(value);
        style.color.red = floatChannelToUint8(tmp[0]);
        style.color.green = floatChannelToUint8(tmp[1]);
        style.color.blue = 0;
        gui::Image(m_pinPreviewSize, style);
    } else if (typeId == TypeId::Vector3f) {
        const auto tmp = cpgf::fromVariant<Eigen::Vector3f>(value);
        style.color.red = floatChannelToUint8(tmp[0]);
        style.color.green = floatChannelToUint8(tmp[1]);
        style.color.blue = floatChannelToUint8(tmp[2]);
        gui::Image(m_pinPreviewSize, style);
    } else if (typeId == TypeId::Vector4f) {
        const auto tmp = cpgf::fromVariant<Eigen::Vector4f>(value);
        style.color.red = floatChannelToUint8(tmp[0]);
        style.color.green = floatChannelToUint8(tmp[1]);
        style.color.blue = floatChannelToUint8(tmp[2]);
        gui::Image(m_pinPreviewSize, style);
    } else if (typeId == TypeId::Generator2d) {
        const auto tmp = cpgf::fromVariant<math::Generator2d>(value);
        FillTexture(tmp);
        gui::Image(m_pinPreviewSize, m_texture, style);
    } else if (typeId == TypeId::Generator3d) {
        const auto tmp = cpgf::fromVariant<math::Generator3d>(value);
        auto sPlane = SectionPlaneX0Y();
        sPlane.SetInput(tmp);
        FillTexture(sPlane.Result());
        gui::Image(m_pinPreviewSize, m_texture, style);
    } else {
        throw EngineError("gs::DrawNode::OnDrawPinPreview: unknown value type (id = {})", typeId);
    }
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

void DrawNode::FillTexture(const math::Generator2d& v) {
    if (m_frameNum++ != 0) {
        m_frameNum %= 100;
        return;
    }

    if (m_textureGenerator == nullptr) {
        m_textureGenerator = new Generator2dToTexture();
    }

    m_textureGenerator->SetInput(v);
    m_texture = m_textureGenerator->Result()->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
}

}
