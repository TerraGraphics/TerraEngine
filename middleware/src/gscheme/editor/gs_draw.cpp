#include "middleware/gscheme/editor/gs_draw.h"

#include "dg/texture.h"
#include "eigen/core.h"
#include "imgui/imgui.h"
#include "cpgf/variant.h"
#include "imgui/node_editor.h"
#include "dg/graphics_types.h"
#include "core/common/exception.h"
#include "middleware/imgui/widgets.h"


namespace gs {

Draw::Draw(TexturePtr& texBackground)
    : m_texBackground(texBackground->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE)) {

    const auto& desc = texBackground->GetDesc();
    m_texBackgroundSize = math::SizeF(static_cast<float>(desc.Width), static_cast<float>(desc.Height));
    m_nodes.resize(64);
}

Draw::~Draw() {

}

void Draw::OnStartDrawGraph() {
    m_alpha = static_cast<uint8_t>(ImGui::GetStyle().Alpha * 255.0f);
    for (auto& node: m_nodes) {
        node.OnStartDrawGraph();
    }
}

void Draw::OnFinishDrawGraph() {
}

void Draw::OnStartDrawNode(uintptr_t id, std::string prettyName) {
    auto nodeIndex = id - 1;
    if (nodeIndex >= m_nodes.size()) {
        m_nodes.resize(nodeIndex + 1);
    }
    m_node = &m_nodes[nodeIndex];
    m_node->OnStartDrawNode(id, prettyName, m_alpha);
}

void Draw::OnFinishDrawNode(bool isValid) {
    m_node->OnFinishDrawNode(isValid, m_texBackground.RawPtr(), m_texBackgroundSize);
}

void Draw::OnDrawInputPins(const std::vector<IDraw::Pin>& pins) {
    m_node->OnDrawInputPins(pins);
}

void Draw::OnDrawPinPreview(TypeId typeId, const cpgf::GVariant& value) {
    m_node->OnDrawPinPreview(typeId, value);
}

void Draw::OnDrawOutputPins(const std::vector<IDraw::Pin>& pins) {
    m_node->OnDrawOutputPins(pins);
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
