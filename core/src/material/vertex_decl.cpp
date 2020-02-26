#include "core/material/vertex_decl.h"

#include <cstdint>
#include <utility>
#include <fmt/format.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/InputLayout.h>


VertexDecl::VertexDecl(std::initializer_list<Layout> layouts) {
    uint32_t index = 0;
    std::string prefix = dg::LayoutElement{}.HLSLSemantic;
    std::vector<msh::SemanticDecl> vertexInputDecl;
    vertexInputDecl.reserve(layouts.size());
    m_inputLayoutDesc.reserve(layouts.size());
    for (const auto& item: layouts) {
        auto dgLayout = item.dgLayout;
        dgLayout.InputIndex = index;
        vertexInputDecl.emplace_back(item.mshLayout.name, item.mshLayout.type, prefix + fmt::format_int(index).str());
        m_inputLayoutDesc.push_back(item.dgLayout);
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

VertexDecl& VertexDeclCache::GetFullVertexDecl(const VertexDecl& vertexDecl, const VertexDecl& additionalVertexDecl) {
    uint64_t key = (static_cast<uint64_t>(vertexDecl.GetId()) << uint64_t(32)) + static_cast<uint64_t>(additionalVertexDecl.GetId());
    auto it = m_cache.find(key);
    if (it == m_cache.cend()) {
        it = m_cache.emplace(key, VertexDecl(vertexDecl, additionalVertexDecl)).first;
    }

    return it->second;
}
