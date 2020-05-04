#include "core/material/vdecl_storage.h"

#include <memory>
#include <string>
#include <utility>
#include <fmt/format.h>
#include <unordered_map>

#include <DiligentCore/Graphics/GraphicsEngine/interface/InputLayout.h>

#include "core/common/hash.h"
#include "core/common/exception.h"
#include "core/material/vertex_decl.h"
#include "core/material/microshader_types.h"


namespace std {
    template<> struct hash<VDeclItem> {
        size_t operator()(const VDeclItem& value) const {
            return value.Hash();
        }
    };
}

using VDeclItems = std::vector<VDeclItem>;

struct VDeclStorage::Impl {
    uint32_t m_nextIndex = 0;
    std::unordered_map<VDeclItems, uint32_t, ContainerHasher<VDeclItems>> m_vDecls;
    std::vector<msh::SemanticDecls> m_semanticDeclsStorage;
    std::vector<std::vector<dg::LayoutElement>> m_layoutElementsStorage;
    // vDeclVertex + vDeclinstance => VDeclID
    std::unordered_map<uint64_t, uint32_t> m_joinCache;
};

uint32_t VDeclStorage::Add(std::vector<VDeclItem>&& items) {
    auto itemsSize = items.size();
    auto [it, ok] = impl->m_vDecls.emplace(std::move(items), impl->m_nextIndex);
    if (!ok) {
        return it->second;
    }

    std::vector<msh::SemanticDecl> semanticDecls;
    semanticDecls.reserve(itemsSize);

    std::vector<dg::LayoutElement> layoutElements;
    layoutElements.reserve(itemsSize);

    static const auto prefix = dg::LayoutElement{}.HLSLSemantic;
    uint32_t index = 0;
    for (const auto& item: items) {
        semanticDecls.push_back(item.GetSemanticDecl(prefix + fmt::format_int(index).str()));
        layoutElements.push_back(item.GetLayoutElement(index));
        ++index;
    }

    if (impl->m_semanticDeclsStorage.size() != impl->m_nextIndex) {
        throw EngineError("VDeclStorage: wrong size for m_semanticDeclsStorage");
    }
    if (impl->m_layoutElementsStorage.size() != impl->m_nextIndex) {
        throw EngineError("VDeclStorage: wrong size for m_layoutElementsStorage");
    }
    impl->m_semanticDeclsStorage.emplace_back(std::move(semanticDecls));
    impl->m_layoutElementsStorage.emplace_back(std::move(layoutElements));

    return impl->m_nextIndex++;
}

uint32_t VDeclStorage::Join(uint32_t vDeclVertex, uint32_t vDeclinstance) {
    uint64_t key = ((static_cast<uint64_t>(vDeclVertex) << uint64_t(32)) | static_cast<uint64_t>(vDeclinstance));
    auto it = impl->m_joinCache.find(key);
    if (it == impl->m_joinCache.cend()) {
        return it->second;
    }

    if (impl->m_semanticDeclsStorage.size() <= vDeclVertex) {
        throw EngineError("VDeclStorage: wrong vDeclVertex arg");
    }
    if (impl->m_semanticDeclsStorage.size() <= vDeclinstance) {
        throw EngineError("VDeclStorage: wrong vDeclinstance arg");
    }

    const auto& semanticDeclsVertex = impl->m_semanticDeclsStorage[vDeclVertex].GetDataRef();
    const auto& semanticDeclslinstance = impl->m_semanticDeclsStorage[vDeclinstance].GetDataRef();
    const auto& layoutElementsVertex = impl->m_layoutElementsStorage[vDeclVertex];
    const auto& layoutElementslinstance = impl->m_layoutElementsStorage[vDeclinstance];

    uint32_t resultSize = static_cast<uint32_t>(semanticDeclsVertex.size() + semanticDeclslinstance.size());

    std::vector<msh::SemanticDecl> semanticDecls;
    semanticDecls.reserve(resultSize);
    semanticDecls.insert(semanticDecls.cend(), semanticDeclsVertex.cbegin(), semanticDeclsVertex.cend());
    semanticDecls.insert(semanticDecls.cend(), semanticDeclslinstance.cbegin(), semanticDeclslinstance.cend());

    std::vector<dg::LayoutElement> layoutElements;
    layoutElements.reserve(resultSize);
    layoutElements.insert(layoutElements.cend(), layoutElementsVertex.cbegin(), layoutElementsVertex.cend());
    layoutElements.insert(layoutElements.cend(), layoutElementslinstance.cbegin(), layoutElementslinstance.cend());

    static const auto prefix = dg::LayoutElement{}.HLSLSemantic;
    for (uint32_t index = 0; index!=resultSize; ++index) {
        semanticDecls[index].semantic = prefix + fmt::format_int(index).str();
        layoutElements[index].InputIndex = index;
    }

    if (impl->m_semanticDeclsStorage.size() != impl->m_nextIndex) {
        throw EngineError("VDeclStorage: wrong size for m_semanticDeclsStorage");
    }
    if (impl->m_layoutElementsStorage.size() != impl->m_nextIndex) {
        throw EngineError("VDeclStorage: wrong size for m_layoutElementsStorage");
    }
    impl->m_semanticDeclsStorage.emplace_back(std::move(semanticDecls));
    impl->m_layoutElementsStorage.emplace_back(std::move(layoutElements));
    impl->m_joinCache[key] = impl->m_nextIndex;

    return impl->m_nextIndex++;
}

const msh::SemanticDecls& VDeclStorage::GetSemanticDecls(uint32_t id) const {
    return impl->m_semanticDeclsStorage[id];
}

const std::vector<dg::LayoutElement>& VDeclStorage::GetLayoutElements(uint32_t id) const {
    return impl->m_layoutElementsStorage[id];
}
