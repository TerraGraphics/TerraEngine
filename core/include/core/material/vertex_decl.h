#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>
#include <unordered_map>
#include <initializer_list>
#include <DiligentCore/Primitives/interface/BasicTypes.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/InputLayout.h>

#include "core/common/ctor.h"
#include "core/common/counter.h"
#include "core/material/microshader_types.h"


namespace dg = Diligent;

enum class VDeclType : uint8_t {
    Float = 0,
    Float2 = 1,
    Float3 = 2,
    Float4 = 3,
    Color3 = 4,
    Color4 = 5,
};

class VDeclItem {
public:
    VDeclItem() = delete;
    VDeclItem(const std::string& varName, VDeclType varType, uint32_t bufferSlot = 0, bool perVertex = true);
    ~VDeclItem() = default;

    bool operator==(const VDeclItem& other) const;

    size_t Hash() const;
    msh::SemanticDecl GetSemanticDecl(const std::string& semantic) const;
    dg::LayoutElement GetLayoutElement(uint32_t inputIndex) const;

private:
    std::string m_varName;
    uint32_t m_bufferSlot = 0;
    VDeclType m_varType = VDeclType::Float;
    bool m_perVertex = true;
};

class VertexDecl : public Counter<VertexDecl> {
public:
    VertexDecl() = default;
    VertexDecl(std::initializer_list<VDeclItem> items);
    VertexDecl(const VertexDecl& first, const VertexDecl& second);
    ~VertexDecl() = default;

    const msh::SemanticDecls& GetVertexInput() const { return m_vertexInputDecl; }
    dg::InputLayoutDesc GetInputLayoutDesc() const;

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
