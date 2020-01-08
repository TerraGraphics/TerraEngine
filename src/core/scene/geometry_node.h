#pragma once

#include <DiligentCore/Common/interface/BasicMath.h>
#include <DiligentCore/Primitives/interface/BasicTypes.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/GraphicsTypes.h>

#include "core/scene/index_buffer.h"
#include "core/scene/vertex_buffer.h"


class GeometryNode : public Counter<GeometryNode>, Fixed {
public:
    GeometryNode() = delete;
    GeometryNode(const std::shared_ptr<VertexBuffer>& vb, uint32_t vbOffsetBytes,
        const std::shared_ptr<IndexBuffer>& ib, uint32_t ibOffsetBytes, uint32_t ibCount, bool ibUint32);
    ~GeometryNode() = default;

public:
    void Bind(ContextPtr& context);
    uint32_t Draw(ContextPtr& context, uint32_t firstInstanceIndex = 0);

private:
    std::shared_ptr<VertexBuffer> m_vertexBuffer = nullptr;
    std::shared_ptr<IndexBuffer> m_indexBuffer = nullptr;
    uint32_t m_vertexBufferOffsetBytes = 0;
    uint32_t m_indexBufferOffsetBytes = 0;
    uint32_t m_indexBufferCount = 0;
    bool m_indexBufferUint32 = true;
};
