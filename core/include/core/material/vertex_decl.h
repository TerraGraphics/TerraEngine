#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <type_traits>
#include <unordered_map>
#include <initializer_list>
#include <DiligentCore/Graphics/GraphicsEngine/interface/InputLayout.h>

#include "core/math/basic.h"
#include "core/common/ctor.h"
#include "core/common/counter.h"
#include "core/common/exception.h"
#include "core/dg/graphics_types.h"
#include "core/material/microshader_types.h"


struct Layout {
    msh::Decl mshLayout;
    dg::LayoutElement dgLayout;
};

template<typename T> Layout ItemDecl(const std::string& name, uint32_t bufferSlot = 0,
    dg::LayoutElement::FREQUENCY frequency = dg::LayoutElement::FREQUENCY_PER_VERTEX) {

    if constexpr (std::is_same<T, float>::value) {
        return Layout{msh::Decl(name, "float"),  dg::LayoutElement(0, bufferSlot, 1, dg::VT_FLOAT32, false, frequency)};
    } else if constexpr (std::is_same<T, dg::float2>::value) {
        return Layout{msh::Decl(name, "float2"), dg::LayoutElement(0, bufferSlot, 2, dg::VT_FLOAT32, false, frequency)};
    } else if constexpr (std::is_same<T, dg::float3>::value) {
        return Layout{msh::Decl(name, "float3"), dg::LayoutElement(0, bufferSlot, 3, dg::VT_FLOAT32, false, frequency)};
    } else if constexpr (std::is_same<T, dg::float4>::value) {
        return Layout{msh::Decl(name, "float4"), dg::LayoutElement(0, bufferSlot, 4, dg::VT_FLOAT32, false, frequency)};
    } else if constexpr (std::is_same<T, math::Color3>::value) {
        return Layout{msh::Decl(name, "float3"), dg::LayoutElement(0, bufferSlot, 3, dg::VT_UINT8, true, frequency)};
    } else if constexpr (std::is_same<T, math::Color4>::value) {
        return Layout{msh::Decl(name, "float4"), dg::LayoutElement(0, bufferSlot, 4, dg::VT_UINT8, true, frequency)};
    } else {
        throw EngineError("unknown type for Layout");
    }
}

class VertexDecl : public Counter<VertexDecl> {
public:
    VertexDecl() = default;
    VertexDecl(std::initializer_list<Layout> layouts);
    VertexDecl(const VertexDecl& first, const VertexDecl& second);
    ~VertexDecl() = default;

    const msh::SemanticDecls& GetVertexInput() const { return m_vertexInputDecl; }
    dg::InputLayoutDesc GetInputLayoutDesc() const {
        return dg::InputLayoutDesc(m_inputLayoutDesc.data(), static_cast<uint32_t>(m_inputLayoutDesc.size()));
    }

private:
    msh::SemanticDecls m_vertexInputDecl;
    std::vector<dg::LayoutElement> m_inputLayoutDesc;
};

class VertexDeclCache : Fixed {
public:
    VertexDeclCache() = default;
    ~VertexDeclCache() = default;

    VertexDecl& GetFullVertexDecl(const VertexDecl& vertexDecl, const VertexDecl& additionalVertexDecl);

private:
    std::unordered_map<uint64_t, VertexDecl> m_cache;
};
