#include "core/material/vdecl_storage.h"

#include <memory>
#include <utility>
#include <unordered_set>
#include <unordered_map>

#include <DiligentCore/Graphics/GraphicsEngine/interface/InputLayout.h>

#include "core/str/format.h"
#include "core/common/hash.h"
#include "core/common/exception.h"
#include "core/dg/graphics_types.h"
#include "core/material/vdecl_item.h"
#include "core/material/microshader_types.h"


namespace std {
    template<> struct hash<VDeclItem> {
        size_t operator()(const VDeclItem& value) const {
            return value.Hash();
        }
    };
}

namespace {

static msh::SemanticDecl GetSemanticDecl(const VDeclItem& item, uint32_t index) {
    static const auto prefix = dg::LayoutElement{}.HLSLSemantic;
    auto semantic = prefix + fmt::format_int(index).str();

    switch (item.varType) {
        case VDeclType::Float:
            return msh::SemanticDecl(item.varName, "float", semantic);
        case VDeclType::Float2:
            return msh::SemanticDecl(item.varName, "float2", semantic);
        case VDeclType::Float3:
            return msh::SemanticDecl(item.varName, "float3", semantic);
        case VDeclType::Float4:
            return msh::SemanticDecl(item.varName, "float4", semantic);
        case VDeclType::Color3:
            return msh::SemanticDecl(item.varName, "float3", semantic);
        case VDeclType::Color4:
            return msh::SemanticDecl(item.varName, "float4", semantic);
    }

    throw EngineError("VDeclItem: wrong varType value = {}", item.varType);
}

static dg::LayoutElement GetLayoutElement(const VDeclItem& item, uint32_t inputIndex) {
    auto frequency = item.perVertex ? dg::INPUT_ELEMENT_FREQUENCY_PER_VERTEX : dg::INPUT_ELEMENT_FREQUENCY_PER_INSTANCE;
    switch (item.varType) {
        case VDeclType::Float:
            return dg::LayoutElement(inputIndex, item.bufferSlot, 1, dg::VT_FLOAT32, false, frequency);
        case VDeclType::Float2:
            return dg::LayoutElement(inputIndex, item.bufferSlot, 2, dg::VT_FLOAT32, false, frequency);
        case VDeclType::Float3:
            return dg::LayoutElement(inputIndex, item.bufferSlot, 3, dg::VT_FLOAT32, false, frequency);
        case VDeclType::Float4:
            return dg::LayoutElement(inputIndex, item.bufferSlot, 4, dg::VT_FLOAT32, false, frequency);
        case VDeclType::Color3:
            return dg::LayoutElement(inputIndex, item.bufferSlot, 3, dg::VT_UINT8, true, frequency);
        case VDeclType::Color4:
            return dg::LayoutElement(inputIndex, item.bufferSlot, 4, dg::VT_UINT8, true, frequency);
    }

    throw EngineError("VDeclItem: wrong varType value = {}", item.varType);
}

static uint32_t VarNameCacheKey(VDeclStorage::VarNameId varNameId, uint16_t vDeclId) {
    return (static_cast<uint32_t>(varNameId) << uint32_t(16)) | static_cast<uint32_t>(vDeclId);
}

}

using VDeclItems = std::vector<VDeclItem>;

struct VDeclStorage::Impl {
    uint16_t m_nextIndex = 1;
    std::unordered_set<uint32_t> m_varNames;
    std::unordered_map<VDeclItems, uint16_t, ContainerHasher<VDeclItems>> m_vDecls;
    std::vector<msh::SemanticDecls> m_semanticDeclsStorage;
    std::vector<std::vector<dg::LayoutElement>> m_layoutElementsStorage;
    // vDeclVertex + vDeclinstance => VDeclID
    std::unordered_map<uint32_t, uint16_t> m_joinCache;
    std::vector<std::string> m_varIdToName;
    std::unordered_map<std::string, VDeclStorage::VarNameId> m_varNameToId;
};

VDeclStorage::VDeclStorage() {

}

VDeclStorage::~VDeclStorage() {

}

uint16_t VDeclStorage::Add(std::vector<VDeclItem>&& items) {
    auto itemsSize = items.size();
    auto [it, ok] = impl->m_vDecls.emplace(std::move(items), impl->m_nextIndex);
    if (!ok) {
        return it->second;
    }

    if (impl->m_semanticDeclsStorage.size() != (impl->m_nextIndex - 1)) {
        throw EngineError("VDeclStorage: wrong size for m_semanticDeclsStorage");
    }
    if (impl->m_layoutElementsStorage.size() != (impl->m_nextIndex - 1)) {
        throw EngineError("VDeclStorage: wrong size for m_layoutElementsStorage");
    }

    std::vector<msh::SemanticDecl> semanticDecls;
    semanticDecls.reserve(itemsSize);

    std::vector<dg::LayoutElement> layoutElements;
    layoutElements.reserve(itemsSize);

    uint32_t index = 0;
    for (const auto& item: it->first) {
        semanticDecls.push_back(GetSemanticDecl(item, index));
        layoutElements.push_back(GetLayoutElement(item, index));
        impl->m_varNames.insert(VarNameCacheKey(GetVarNameId(item.varName), impl->m_nextIndex));
        ++index;
    }

    impl->m_semanticDeclsStorage.emplace_back(std::move(semanticDecls));
    impl->m_layoutElementsStorage.emplace_back(std::move(layoutElements));

    return impl->m_nextIndex++;
}

uint16_t VDeclStorage::Join(uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance) {
    uint32_t key = ((static_cast<uint32_t>(vDeclIdPerVertex) << uint32_t(16)) | static_cast<uint32_t>(vDeclIdPerInstance));
    if (const auto it=impl->m_joinCache.find(key); it != impl->m_joinCache.cend()) {
        return it->second;
    }

    if (vDeclIdPerVertex == 0) {
        throw EngineError("VDeclStorage: wrong vDeclIdPerVertex arg (equal 0)");
    }
    if (vDeclIdPerInstance == 0) {
        throw EngineError("VDeclStorage: wrong vDeclIdPerInstance arg (equal 0)");
    }
    if (impl->m_semanticDeclsStorage.size() < vDeclIdPerVertex) {
        throw EngineError("VDeclStorage: wrong vDeclIdPerVertex arg");
    }
    if (impl->m_semanticDeclsStorage.size() < vDeclIdPerInstance) {
        throw EngineError("VDeclStorage: wrong vDeclIdPerInstance arg");
    }
    if (impl->m_semanticDeclsStorage.size() != (impl->m_nextIndex - 1)) {
        throw EngineError("VDeclStorage: wrong size for m_semanticDeclsStorage");
    }
    if (impl->m_layoutElementsStorage.size() != (impl->m_nextIndex - 1)) {
        throw EngineError("VDeclStorage: wrong size for m_layoutElementsStorage");
    }

    const auto& semanticDeclsVertex = impl->m_semanticDeclsStorage[vDeclIdPerVertex - 1].GetData();
    const auto& semanticDeclslinstance = impl->m_semanticDeclsStorage[vDeclIdPerInstance - 1].GetData();
    const auto& layoutElementsVertex = impl->m_layoutElementsStorage[vDeclIdPerVertex - 1];
    const auto& layoutElementslinstance = impl->m_layoutElementsStorage[vDeclIdPerInstance - 1];

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
        impl->m_varNames.insert(VarNameCacheKey(GetVarNameId(semanticDecls[index].name), impl->m_nextIndex));
    }

    impl->m_semanticDeclsStorage.emplace_back(std::move(semanticDecls));
    impl->m_layoutElementsStorage.emplace_back(std::move(layoutElements));
    impl->m_joinCache[key] = impl->m_nextIndex;

    return impl->m_nextIndex++;
}

VDeclStorage::VarNameId VDeclStorage::GetVarNameId(const std::string& name) {
    if (auto it = impl->m_varNameToId.find(name); it != impl->m_varNameToId.cend()) {
        return it->second;
    }

    auto id = VarNameId(static_cast<uint16_t>(impl->m_varIdToName.size() + 1));
    impl->m_varNameToId[name] = id;
    impl->m_varIdToName.push_back(name);

    return id;
}

bool VDeclStorage::IsNameExists(VarNameId varNameId, uint16_t vDeclId) const {
    const auto key = (static_cast<uint32_t>(varNameId) << uint32_t(16)) | static_cast<uint32_t>(vDeclId);
    return (impl->m_varNames.find(key) != impl->m_varNames.cend());
}

const msh::SemanticDecls& VDeclStorage::GetSemanticDecls(uint16_t id) const {
    if (id == 0) {
        throw EngineError("VDeclStorage: wrong id (equal 0)");
    }
     if (impl->m_semanticDeclsStorage.size() < id) {
        throw EngineError("VDeclStorage: wrong index for m_semanticDeclsStorage");
    }

    return impl->m_semanticDeclsStorage[id - 1];
}

const std::vector<dg::LayoutElement>& VDeclStorage::GetLayoutElements(uint16_t id) const {
    if (id == 0) {
        throw EngineError("VDeclStorage: wrong id (equal 0)");
    }
     if (impl->m_layoutElementsStorage.size() < id) {
        throw EngineError("VDeclStorage: wrong index for m_layoutElementsStorage");
    }

    return impl->m_layoutElementsStorage[id - 1];
}
