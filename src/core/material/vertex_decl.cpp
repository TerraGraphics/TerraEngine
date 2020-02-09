#include "core/material/vertex_decl.h"


VertexDecl::VertexDecl(std::initializer_list<Layout> layouts) {
    uint32_t index = 0;
    std::vector<msh::Decl> vertexInputDecl;
    vertexInputDecl.reserve(layouts.size());
    m_inputLayoutDesc.reserve(layouts.size());
    for (const auto& item: layouts) {
        auto dgLayout = item.dgLayout;
        dgLayout.InputIndex = index;
        ++index;
        vertexInputDecl.push_back(item.mshLayout);
        m_inputLayoutDesc.push_back(item.dgLayout);
    }
    m_vertexInputDecl.SetData(std::move(vertexInputDecl));
}

VertexDecl::VertexDecl(const VertexDecl& first, const VertexDecl& second)
    : m_vertexInputDecl(first.m_vertexInputDecl)
    , m_inputLayoutDesc(first.m_inputLayoutDesc) {
    m_vertexInputDecl.Append(second.m_vertexInputDecl);
    m_inputLayoutDesc.insert(m_inputLayoutDesc.cend(), second.m_inputLayoutDesc.cbegin(), second.m_inputLayoutDesc.cend());
    uint32_t index = 0;
    for (auto& item: m_inputLayoutDesc) {
        item.InputIndex = index;
        ++index;
    }
}

VertexDecl& VertexDeclCache::GetFullVertexDecl(const VertexDecl& value) {
    auto it = m_cache.find(value.GetId());
    if (it == m_cache.cend()) {
        it = m_cache.emplace(value.GetId(), VertexDecl(value, m_additionalVertexDecl)).first;
    }

    return it->second;
}

void VertexDeclCache::SetAdditionalVertexDecl(const VertexDecl& value) {
    m_additionalVertexDecl = value;
}
