#pragma once

#include <DiligentCore/Common/interface/BasicMath.h>
#include <DiligentCore/Primitives/interface/BasicTypes.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/Buffer.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/GraphicsTypes.h>

#include "core/common/dg.h"
#include "core/common/ctor.h"


class VertexBuffer : Noncopyable {
public:
    VertexBuffer() = delete;
    VertexBuffer(DevicePtr& device, const void* data, size_t size, const dg::Char* name = nullptr);
    VertexBuffer(VertexBuffer&& other) noexcept;
    VertexBuffer& operator=(VertexBuffer&&) noexcept;

public:
    void Bind(ContextPtr& context);

private:
	dg::RefCntAutoPtr<dg::IBuffer> m_vertexBuffer;
};

class IndexBuffer : Noncopyable {
public:
    explicit IndexBuffer(DevicePtr& device, const uint16_t* data, size_t size, const dg::Char* name = nullptr);
    explicit IndexBuffer(DevicePtr& device, const uint32_t* data, size_t size, const dg::Char* name = nullptr);
    IndexBuffer(IndexBuffer&& other) noexcept;
    IndexBuffer& operator=(IndexBuffer&&) noexcept;

public:
	uint32_t GetNumber() const noexcept { return m_number; }
	dg::VALUE_TYPE GetType() const noexcept { return m_type; }

    void Bind(ContextPtr& context);

private:
	uint32_t m_number = 0;
    dg::VALUE_TYPE m_type = dg::VALUE_TYPE::VT_INT32;
	dg::RefCntAutoPtr<dg::IBuffer> m_indexBuffer;
};

class Counter {
public:
    Counter() : m_id(++Counter::m_lastId) {}

    uint32_t GetId() const noexcept { return m_id; }

protected:
    const uint32_t m_id = 0;

private:
    static uint32_t m_lastId;
};

class GeometryNode : public Counter, Fixed {
public:
    GeometryNode() = delete;
    GeometryNode(VertexBuffer&& vertexBuffer, IndexBuffer&& indexBuffer);

public:
    void Bind(ContextPtr& context);
    uint32_t Draw(ContextPtr& context);

private:
    VertexBuffer m_vertexBuffer;
    IndexBuffer m_indexBuffer;
};
