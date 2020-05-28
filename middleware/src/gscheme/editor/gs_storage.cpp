#include "middleware/gscheme/editor/gs_storage.h"

#include <memory>
#include <cstdint>
#include <utility>
#include <type_traits>
#include <unordered_map>

#include "core/dg/texture.h"
#include "core/common/hash.h"
#include "core/dg/graphics_types.h"
#include "middleware/imgui/imgui.h"
#include "middleware/gscheme/rttr/type.h"
#include "middleware/gscheme/rttr/variant.h"
#include "middleware/gscheme/editor/gs_id.h"
#include "middleware/gscheme/editor/gs_node.h"
#include "middleware/imgui/imgui_node_editor.h"
#include "middleware/gscheme/editor/gs_node_type.h"
#include "middleware/gscheme/reflection/gs_metadata.h"


struct LinkInfo {
    uintptr_t srcPin;
    uintptr_t dstPin;
};

struct GSStorage::Impl {
    Impl(TexturePtr& texBackground);

    bool AddNode(const std::string& name);
    bool AddLink(uintptr_t pinFirst, uintptr_t pinSecond, bool checkOnly);
    void Draw();

    float m_texBackgroundWidht = 1.f;
    float m_texBackgroundheight = 1.f;
    TextureViewPtr m_texBackground;

    std::shared_ptr<GSNode> m_selectedNode;
    std::unordered_map<uintptr_t, LinkInfo> m_links;
    std::unordered_map<uintptr_t, std::shared_ptr<GSNode>> m_nodes;
    std::unordered_map<std::string, std::shared_ptr<GSNodeType>> m_nodeTypes;
};

GSStorage::Impl::Impl(TexturePtr& texBackground)
    : m_texBackground(texBackground->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE)) {

    m_texBackgroundWidht = static_cast<float>(texBackground->GetDesc().Width);
    m_texBackgroundheight = static_cast<float>(texBackground->GetDesc().Height);
}

bool GSStorage::Impl::AddNode(const std::string& name) {
    if (const auto it = m_nodeTypes.find(name); it != m_nodeTypes.cend()) {
        auto id = GSGetNextID();
        m_nodes[id] = it->second->NewInstance(id);
        return true;
    }

    return false;
}

bool GSStorage::Impl::AddLink(uintptr_t pinFirst, uintptr_t pinSecond, bool checkOnly) {
    if ((pinFirst == 0) ||
        (pinSecond == 0) ||
        (pinFirst == pinSecond)) {
        return false;
    }

    if (!checkOnly) {
        auto id = GSGetNextID();
        m_links[id] = LinkInfo{pinFirst, pinSecond};
    }

    return true;
}

void GSStorage::Impl::Draw() {
    auto alpha = static_cast<uint8_t>(ImGui::GetStyle().Alpha * 255.0f);
    auto texBackgroundRaw = m_texBackground.RawPtr();

    auto doubleClickedNodeId = ne::GetDoubleClickedNode().Get();
    for (auto& [nodeId, node]: m_nodes) {
        node->Draw(alpha, texBackgroundRaw, m_texBackgroundWidht, m_texBackgroundheight);
        if (doubleClickedNodeId == nodeId) {
           m_selectedNode = node;
        }
    }

    for (const auto& [linkId, info] : m_links) {
        ne::Link(ne::LinkId(linkId), ne::PinId(info.srcPin), ne::PinId(info.dstPin));
    }

    if (ne::IsBackgroundClicked()) {
        m_selectedNode.reset();
    }
}

GSStorage::GSStorage(TexturePtr& texBackground)
    : impl(texBackground) {

}

GSStorage::~GSStorage() {

}

void GSStorage::Create() {
    for(const auto& t : rttr::type::get_types()) {
        if (t.get_metadata(GSMetaTypes::GS_CLASS).is_valid()) {
            std::string name = t.get_name().to_string();
            impl->m_nodeTypes[name] = std::make_shared<GSNodeType>(name, t);
        }
    }
}

bool GSStorage::AddNode(const std::string& name) {
    return impl->AddNode(name);
}

bool GSStorage::AddLink(uintptr_t pinFirst, uintptr_t pinSecond, bool checkOnly) {
    return impl->AddLink(pinFirst, pinSecond, checkOnly);
}

void GSStorage::Draw() {
    impl->Draw();
}

void GSStorage::DrawProperty() {
    if (impl->m_selectedNode) {
        impl->m_selectedNode->DrawEditGui();
    }
}
