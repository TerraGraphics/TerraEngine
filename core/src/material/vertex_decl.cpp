#include "core/material/vertex_decl.h"

#include <cstdint>
#include <utility>
#include <fmt/format.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/InputLayout.h>

#include "core/common/exception.h"
#include "core/dg/graphics_types.h"


VDeclItem::VDeclItem(const std::string& varName, VDeclType varType, uint32_t bufferSlot, bool perVertex)
    : m_varName(varName)
    , m_bufferSlot(bufferSlot)
    , m_varType(varType)
    , m_perVertex(perVertex) {
    if ((varType < VDeclType::Float) || (varType > VDeclType::Color4)) {
        throw EngineError("VDeclItem: wrong varType value = {}", varType);
    }
}

msh::SemanticDecl VDeclItem::GetSemanticDecl(const std::string& semantic) const {
    switch (m_varType) {
        case VDeclType::Float:
            return msh::SemanticDecl(m_varName, "float", semantic);
        case VDeclType::Float2:
            return msh::SemanticDecl(m_varName, "float2", semantic);
        case VDeclType::Float3:
            return msh::SemanticDecl(m_varName, "float3", semantic);
        case VDeclType::Float4:
            return msh::SemanticDecl(m_varName, "float4", semantic);
        case VDeclType::Color3:
            return msh::SemanticDecl(m_varName, "float3", semantic);
        case VDeclType::Color4:
            return msh::SemanticDecl(m_varName, "float4", semantic);
    }

    throw EngineError("VDeclItem: wrong varType value = {}", m_varType);
}

dg::LayoutElement VDeclItem::GetLayoutElement(uint32_t inputIndex) const {
    auto frequency = m_perVertex ? dg::INPUT_ELEMENT_FREQUENCY_PER_VERTEX : dg::INPUT_ELEMENT_FREQUENCY_PER_INSTANCE;
    switch (m_varType) {
        case VDeclType::Float:
            return dg::LayoutElement(inputIndex, m_bufferSlot, 1, dg::VT_FLOAT32, false, frequency);
        case VDeclType::Float2:
            return dg::LayoutElement(inputIndex, m_bufferSlot, 2, dg::VT_FLOAT32, false, frequency);
        case VDeclType::Float3:
            return dg::LayoutElement(inputIndex, m_bufferSlot, 3, dg::VT_FLOAT32, false, frequency);
        case VDeclType::Float4:
            return dg::LayoutElement(inputIndex, m_bufferSlot, 4, dg::VT_FLOAT32, false, frequency);
        case VDeclType::Color3:
            return dg::LayoutElement(inputIndex, m_bufferSlot, 3, dg::VT_UINT8, true, frequency);
        case VDeclType::Color4:
            return dg::LayoutElement(inputIndex, m_bufferSlot, 4, dg::VT_UINT8, true, frequency);
    }

    throw EngineError("VDeclItem: wrong varType value = {}", m_varType);
}

VertexDecl::VertexDecl(std::initializer_list<VDeclItem> items) {
    uint32_t index = 0;
    std::string prefix = dg::LayoutElement{}.HLSLSemantic;
    std::vector<msh::SemanticDecl> vertexInputDecl;
    vertexInputDecl.reserve(items.size());
    m_inputLayoutDesc.reserve(items.size());
    for (const auto& item: items) {
        vertexInputDecl.push_back(item.GetSemanticDecl(prefix + fmt::format_int(index).str()));
        m_inputLayoutDesc.push_back(item.GetLayoutElement(index));
        ++index;
    }

    m_vertexInputDecl.SetData(std::move(vertexInputDecl));
}

VertexDecl::VertexDecl(const VertexDecl& first, const VertexDecl& second)
    : m_inputLayoutDesc(first.m_inputLayoutDesc) {

    uint32_t index = 0;
    std::string prefix = dg::LayoutElement{}.HLSLSemantic;
    auto vertexInputDecl = first.m_vertexInputDecl.GetData();
    const auto& secondArr = second.m_vertexInputDecl.GetData();
    vertexInputDecl.insert(vertexInputDecl.cend(), secondArr.cbegin(), secondArr.cend());
    for (auto& vDecl : vertexInputDecl) {
        vDecl.semantic = prefix + fmt::format_int(index).str();
        ++index;
    }
    m_vertexInputDecl.SetData(std::move(vertexInputDecl));

    m_inputLayoutDesc.insert(m_inputLayoutDesc.cend(), second.m_inputLayoutDesc.cbegin(), second.m_inputLayoutDesc.cend());
    index = 0;
    for (auto& item: m_inputLayoutDesc) {
        item.InputIndex = index;
        ++index;
    }
}

 dg::InputLayoutDesc VertexDecl::GetInputLayoutDesc() const {
    return dg::InputLayoutDesc(m_inputLayoutDesc.data(), static_cast<uint32_t>(m_inputLayoutDesc.size()));
}

VertexDecl& VertexDeclCache::GetFullVertexDecl(const VertexDecl& vertexDecl, const VertexDecl& additionalVertexDecl) {
    uint64_t key = (static_cast<uint64_t>(vertexDecl.GetId()) << uint64_t(32)) + static_cast<uint64_t>(additionalVertexDecl.GetId());
    auto it = m_cache.find(key);
    if (it == m_cache.cend()) {
        it = m_cache.emplace(key, VertexDecl(vertexDecl, additionalVertexDecl)).first;
    }

    return it->second;
}
