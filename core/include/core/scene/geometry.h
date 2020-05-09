#pragma once

#include <memory>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/common/counter.h"


class Geometry : public Counter<Geometry>, Fixed {
protected:
    Geometry() = delete;
    Geometry(uint16_t vDeclId);
    virtual ~Geometry();

public:
    uint16_t GetVDeclId() const noexcept { return m_vDeclId; }

    virtual void Bind(ContextPtr& context) = 0;
    virtual uint32_t Draw(ContextPtr& context, uint32_t firstInstanceIndex = 0) = 0;

protected:
    uint16_t m_vDeclId;
};

class VertexBuffer;
class GeometryUnindexed final : public Geometry {
public:
    GeometryUnindexed() = delete;
    GeometryUnindexed(const std::shared_ptr<VertexBuffer>& vb, uint32_t vbOffsetBytes, uint32_t vbCount, uint16_t vDeclId);
    ~GeometryUnindexed() final;

public:
    void Bind(ContextPtr& context) final;
    uint32_t Draw(ContextPtr& context, uint32_t firstInstanceIndex = 0) final;

private:
    std::shared_ptr<VertexBuffer> m_vertexBuffer = nullptr;
    uint32_t m_vertexBufferOffsetBytes = 0;
    uint32_t m_vertexBufferCount = 0;
};

class IndexBuffer;
class GeometryIndexed final : public Geometry {
public:
    GeometryIndexed() = delete;
    GeometryIndexed(const std::shared_ptr<VertexBuffer>& vb, uint32_t vbOffsetBytes,
        const std::shared_ptr<IndexBuffer>& ib, uint32_t ibOffsetBytes, uint32_t ibCount, bool ibUint32,
        uint16_t vDeclId);
    ~GeometryIndexed() final;

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
