#include "middleware/gscheme/editor/gs_storage.h"

#include <memory>
#include <cstdint>
#include <utility>
#include <unordered_map>

#include "core/dg/texture.h"
#include "core/common/hash.h"
#include "core/dg/graphics_types.h"
#include "middleware/imgui/imgui.h"
#include "middleware/gscheme/rttr/type.h"
#include "middleware/gscheme/rttr/variant.h"
#include "middleware/gscheme/editor/gs_node.h"
#include "middleware/gscheme/editor/gs_node_type.h"
#include "middleware/gscheme/reflection/gs_metadata.h"


struct GSStorage::Impl {
    Impl(TexturePtr& texBackground);

    float m_texBackgroundWidht = 1.f;
    float m_texBackgroundheight = 1.f;
    TextureViewPtr m_texBackground;

    uintptr_t m_nextId = 1;
    std::unordered_map<uintptr_t, std::shared_ptr<GSNode>> m_nodes;
    std::unordered_map<std::string, std::shared_ptr<GSNodeType>> m_nodeTypes;
};

GSStorage::Impl::Impl(TexturePtr& texBackground)
    : m_texBackground(texBackground->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE)) {

    m_texBackgroundWidht = static_cast<float>(texBackground->GetDesc().Width);
    m_texBackgroundheight = static_cast<float>(texBackground->GetDesc().Height);
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
    if (const auto it = impl->m_nodeTypes.find(name); it != impl->m_nodeTypes.cend()) {
        impl->m_nodes[impl->m_nextId] = it->second->NewInstance(impl->m_nextId);
        ++impl->m_nextId;
        return true;
    }

    return false;
}

void GSStorage::Draw() {
    auto alpha = static_cast<uint8_t>(ImGui::GetStyle().Alpha * 255.0f);
    auto texBackgroundRaw = impl->m_texBackground.RawPtr();

    for (auto& [_, node]: impl->m_nodes) {
        node->Draw(alpha, texBackgroundRaw, impl->m_texBackgroundWidht, impl->m_texBackgroundheight);
    }
}
