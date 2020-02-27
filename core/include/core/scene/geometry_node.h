#pragma once

#include <memory>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/common/counter.h"


class GeometryNode : public Counter<GeometryNode>, Fixed {
protected:
    GeometryNode();
    virtual ~GeometryNode();

public:
    virtual void Bind(ContextPtr& context) = 0;
    virtual uint32_t Draw(ContextPtr& context, uint32_t firstInstanceIndex = 0) = 0;
};

class VertexBuffer;
class GeometryNodeUnindexed final : public GeometryNode {
public:
    GeometryNodeUnindexed() = delete;
    GeometryNodeUnindexed(const std::shared_ptr<VertexBuffer>& vb, uint32_t vbOffsetBytes, uint32_t vbCount);
    ~GeometryNodeUnindexed() final;

public:
    void Bind(ContextPtr& context) final;
    uint32_t Draw(ContextPtr& context, uint32_t firstInstanceIndex = 0) final;

private:
    std::shared_ptr<VertexBuffer> m_vertexBuffer = nullptr;
    uint32_t m_vertexBufferOffsetBytes = 0;
    uint32_t m_vertexBufferCount = 0;
};

class IndexBuffer;
class GeometryNodeIndexed final : public GeometryNode {
public:
    GeometryNodeIndexed() = delete;
    GeometryNodeIndexed(const std::shared_ptr<VertexBuffer>& vb, uint32_t vbOffsetBytes,
        const std::shared_ptr<IndexBuffer>& ib, uint32_t ibOffsetBytes, uint32_t ibCount, bool ibUint32);
    ~GeometryNodeIndexed() final;

public:
    void Bind(ContextPtr& context) final;
    uint32_t Draw(ContextPtr& context, uint32_t firstInstanceIndex = 0) final;

private:
    std::shared_ptr<VertexBuffer> m_vertexBuffer = nullptr;
    std::shared_ptr<IndexBuffer> m_indexBuffer = nullptr;
    uint32_t m_vertexBufferOffsetBytes = 0;
    uint32_t m_indexBufferOffsetBytes = 0;
    uint32_t m_indexBufferCount = 0;
    bool m_indexBufferUint32 = true;
};
