#include "middleware/gscheme/editor/gs_draw.h"

#include "dg/texture.h"
#include "eigen/core.h"
#include "imgui/imgui.h"
#include "cpgf/variant.h"
#include "imgui/node_editor.h"
#include "dg/graphics_types.h"
#include "core/common/exception.h"
#include "middleware/imgui/widgets.h"
#include "middleware/imgui/imgui_math.h"


namespace gs {

Draw::Draw(TexturePtr& texBackground)
    : m_texBackground(texBackground->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE)) {

    const auto& desc = texBackground->GetDesc();
    m_texBackgroundSize = math::SizeF(static_cast<float>(desc.Width), static_cast<float>(desc.Height));
}

Draw::~Draw() {

}

void Draw::OnStartDrawGraph() {
    m_alpha = static_cast<uint8_t>(ImGui::GetStyle().Alpha * 255.0f);

    ne::PushStyleVar(ne::StyleVar_NodeBorderWidth, 0.f);
    ne::PushStyleVar(ne::StyleVar_HoveredNodeBorderWidth, 2.f);
    ne::PushStyleVar(ne::StyleVar_SelectedNodeBorderWidth, 2.f);

    for (auto& it: m_actualNodes) {
        it = 0;
    }
}

void Draw::OnFinishDrawGraph() {
    for (size_t i=0; i!=m_actualNodes.size(); ++i) {
        if (m_actualNodes[i] == 0) {
            m_maxOutputPinNameWidthPerNode[i] = 0;
        }
    }
    ne::PopStyleVar(3);
}

void Draw::OnStartDrawNode(uintptr_t id, const std::string& prettyName) {
    ne::PushStyleVar(ne::StyleVar_NodePadding, ImVec4(m_nodePaddingLeft, m_nodePaddingTop, m_nodePaddingRight, m_nodePaddingBottom));
    ne::BeginNode(ne::NodeId(id));
    m_nodeId = id;
    m_existsInputPins = false;
    auto nodeIndex = m_nodeId - 1;
    if (nodeIndex >= m_actualNodes.size()) {
        m_actualNodes.resize(nodeIndex + 1, false);
        m_maxOutputPinNameWidthPerNode.resize(nodeIndex + 1, 0);
    }
    m_actualNodes[nodeIndex] = 1;

    // Header
    gui::BeginGroup();
    gui::Text(prettyName);
    m_headerMin = gui::ToPointF(ImGui::GetItemRectMin());
    m_headerSize = gui::ToSizeF(ImGui::GetItemRectSize());
    gui::Dummy(math::SizeF(1.0f, m_nodePaddingTop));
    gui::EndGroup();
}

void Draw::OnFinishDrawNode() {
    const auto headerColor = math::Color(0, 125, 0, m_alpha).value;
    const auto headerLineAlpha = static_cast<uint8_t>(96 * static_cast<int>(m_alpha) / (3 * 255));
    const auto headerLineColor = math::Color(255, 255, 255, headerLineAlpha).value;

    m_headerSize.w = ImGui::GetItemRectMax().x - m_headerMin.x;
    ne::EndNode();

    // Header
    if (ImGui::IsItemVisible()) {
        auto drawList = ne::GetNodeBackgroundDrawList(ne::NodeId(m_nodeId));
        const auto texBackgroundID = reinterpret_cast<ImTextureID>(m_texBackground.RawPtr());
        const auto rounding = ne::GetStyle().NodeRounding;
        const auto roundingCorners = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight;
        const auto uvMin = ImVec2(0.0f, 0.0f);
        const auto uvMax = ImVec2(m_headerSize.w / m_texBackgroundSize.w, m_headerSize.h / m_texBackgroundSize.h);

        const auto halfBorderWidth = ne::GetStyle().NodeBorderWidth * 0.5f;
        const auto imageTopLeft = ImVec2(
            m_headerMin.x - m_nodePaddingLeft + halfBorderWidth,
            m_headerMin.y - m_nodePaddingTop + halfBorderWidth);
        const auto imageBottomRight = ImVec2(
            m_headerMin.x + m_headerSize.w + m_nodePaddingRight - halfBorderWidth,
            m_headerMin.y + m_headerSize.h + m_nodePaddingTop);
        const auto imageBottomLeft = ImVec2(imageTopLeft.x, imageBottomRight.y);

        drawList->AddImageRounded(texBackgroundID, imageTopLeft, imageBottomRight, uvMin, uvMax, headerColor, rounding, roundingCorners);
        drawList->AddLine(imageBottomLeft, imageBottomRight, headerLineColor, 1.0f);
    }
    ne::PopStyleVar(1);
}

void Draw::OnStartDrawInputPins() {
    m_existsInputPins = true;
    gui::BeginGroup();
}

void Draw::OnFinishDrawInputPins() {
    gui::EndGroup();
}

void Draw::OnStartDrawOutputPins() {
    if (m_existsInputPins) {
        gui::SameLine();
    }
    gui::BeginGroup();
}

void Draw::OnFinishDrawOutputPins() {
    gui::EndGroup();
}

void Draw::OnDrawPin(uintptr_t id, bool isInput, bool isConnected, const std::string& prettyName) {
    const auto pinsSeparatorSize = uint32_t(10);
    const auto iconSize = math::Size(24, 24);
    const auto pinColor = math::Color(0, 255, 0, m_alpha);
    const auto innerPinColor = math::Color(32, 32, 32, m_alpha);

    const math::Size nameSize = gui::GetTextSize(prettyName);
    const uint32_t nameOffsetH = (iconSize.h - nameSize.h + 1) / 2;

     if (isInput) {
        ne::BeginPin(ne::PinId(id), ne::PinKind::Input);
            gui::NodeIcon(iconSize, gui::IconType::Circle, isConnected, pinColor, innerPinColor);
        ne::EndPin();
        gui::SameLine();
        gui::Text(prettyName, math::Size(0, nameOffsetH), true);
    } else {
        auto nodeIndex = m_nodeId - 1;
        uint32_t maxNameWidth = std::max(m_maxOutputPinNameWidthPerNode[nodeIndex], nameSize.w);
        m_maxOutputPinNameWidthPerNode[nodeIndex] = maxNameWidth;
        uint32_t nameOffsetW = maxNameWidth - nameSize.w;
        if (m_existsInputPins) {
            nameOffsetW += pinsSeparatorSize;
        }

        gui::Text(prettyName, math::Size(nameOffsetW, nameOffsetH), true);
        gui::SameLine();
        ne::BeginPin(ne::PinId(id), ne::PinKind::Output);
            gui::NodeIcon(iconSize, gui::IconType::Circle, isConnected, pinColor, innerPinColor);
        ne::EndPin();
    }
}

void Draw::OnDrawLink(uintptr_t linkId, uintptr_t srcPinId, uintptr_t dstPinId) {
    ne::Link(ne::LinkId(linkId), ne::PinId(srcPinId), ne::PinId(dstPinId));
}

void Draw::OnDrawEditingHeader(const std::string& prettyName) {
    gui::Text(prettyName + ":");
}

IDraw::EditResult Draw::OnDrawEditingPin(const std::string& prettyName, bool /* disabled */, TypeId typeId, cpgf::GVariant& value) {
    bool isChanded = false;

    if (typeId == TypeId::Float) {
        auto tmp = cpgf::fromVariant<float>(value);
        isChanded |= gui::InputScalar<float>(prettyName.c_str(), tmp, 0.0001f, "{:.4f}");

        if (isChanded) {
            value = cpgf::copyVariantFromCopyable(tmp);
        }
    } else if (typeId == TypeId::Vector2f) {
        if (!prettyName.empty()) {
            gui::Text(prettyName + ":");
        }
        auto tmp = cpgf::fromVariant<Eigen::Vector2f>(value);

        isChanded |= gui::InputScalar<float>("R", tmp[0], 0.0001f, "{:.4f}");
        isChanded |= gui::InputScalar<float>("G", tmp[1], 0.0001f, "{:.4f}");

        if (isChanded) {
            value = cpgf::copyVariantFromCopyable(tmp);
        }
    } else if (typeId == TypeId::Vector3f) {
        if (!prettyName.empty()) {
            gui::Text(prettyName + ":");
        }
        auto tmp = cpgf::fromVariant<Eigen::Vector3f>(value);

        isChanded |= gui::InputScalar<float>("R", tmp[0], 0.0001f, "{:.4f}");
        isChanded |= gui::InputScalar<float>("G", tmp[1], 0.0001f, "{:.4f}");
        isChanded |= gui::InputScalar<float>("B", tmp[2], 0.0001f, "{:.4f}");

        if (isChanded) {
            value = cpgf::copyVariantFromCopyable(tmp);
        }
    } else if (typeId == TypeId::Vector4f) {
        if (!prettyName.empty()) {
            gui::Text(prettyName + ":");
        }
        auto tmp = cpgf::fromVariant<Eigen::Vector4f>(value);

        isChanded |= gui::InputScalar<float>("R", tmp[0], 0.0001f, "{:.4f}");
        isChanded |= gui::InputScalar<float>("G", tmp[1], 0.0001f, "{:.4f}");
        isChanded |= gui::InputScalar<float>("B", tmp[2], 0.0001f, "{:.4f}");
        isChanded |= gui::InputScalar<float>("A", tmp[3], 0.0001f, "{:.4f}");

        if (isChanded) {
            value = cpgf::copyVariantFromCopyable(tmp);
        }
    } else {
        throw EngineError("gs::Draw::OnDrawEditingPin: unknown value type (id = {})", typeId);
    }

    return (isChanded ? IDraw::EditResult::Changed : IDraw::EditResult::NotChanged);
}

}
