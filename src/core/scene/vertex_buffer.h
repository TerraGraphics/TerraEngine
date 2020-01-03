#pragma once

#include <memory>
#include <vector>
#include <stdexcept>

#include <DiligentCore/Graphics/GraphicsEngine/interface/Buffer.h>

#include "core/common/dg.h"
#include "core/common/ctor.h"
#include "core/common/counter.h"


class VertexBuffer : public Counter<VertexBuffer>, Fixed {
public:
    VertexBuffer() = delete;
    VertexBuffer(DevicePtr& device, const void* data, uint32_t size, const dg::Char* name = nullptr);
    ~VertexBuffer() = default;

    void Bind(ContextPtr& context, uint32_t offset);

private:
	dg::RefCntAutoPtr<dg::IBuffer> m_vertexBuffer;
};

template<typename Vertex> class VertexBufferRange : Noncopyable {
public:
    VertexBufferRange() = delete;
    VertexBufferRange(Vertex* begin, uint32_t offsetBytes, uint32_t count) : m_begin(begin), m_offsetBytes(offsetBytes), m_count(count) {}
    VertexBufferRange(VertexBufferRange&& other) noexcept : m_begin(other.m_begin), m_offsetBytes(other.m_offsetBytes), m_count(other.m_count) {
        other.m_begin = nullptr;
        other.m_offsetBytes = 0;
        other.m_count = 0;
    }

    VertexBufferRange& operator=(VertexBufferRange&& other) noexcept {
        std::swap(m_begin, other.m_begin);
        std::swap(m_offsetBytes, other.m_offsetBytes);
        std::swap(m_count, other.m_count);

        return *this;
    }

    ~VertexBufferRange() = default;

public:
    Vertex* Begin() noexcept { return m_begin; }
    Vertex* End() noexcept { return m_begin + m_count; }

    uint32_t OffsetBytes() const noexcept { return m_offsetBytes; }
    uint32_t Count() const noexcept { return m_count; }

    inline Vertex& operator[](uint32_t idx) {
        if (idx < m_count) {
            return m_begin[idx];
        }
        throw std::out_of_range("VertexBufferRange operator[]");
    }

    inline const Vertex& operator[](uint32_t idx) const {
        if (idx < m_count) {
            return m_begin[idx];
        }
        throw std::out_of_range("VertexBufferRange operator[]");
    }

private:
    Vertex* m_begin = nullptr;
    uint32_t m_offsetBytes = 0;
    uint32_t m_count = 0;
};

class VertexBufferBuilder : Fixed {
public:
    VertexBufferBuilder() = default;
    ~VertexBufferBuilder() = default;

public:
    template<typename Vertex>
    void Append(const std::vector<Vertex>& data) {
        const auto* begin = reinterpret_cast<uint8_t*>(data.data());
        m_data.insert(m_data.end(), begin, begin + data.size() * sizeof(Vertex));
    }

    template<typename Vertex>
    VertexBufferRange<Vertex> AddRange(size_t vertexCount) {
        auto offsetCount = static_cast<uint32_t>(m_data.size());
        m_data.resize(offsetCount + vertexCount * sizeof(Vertex));
        return VertexBufferRange<Vertex>(reinterpret_cast<Vertex*>(&m_data[offsetCount]), offsetCount, vertexCount);
    }

    std::shared_ptr<VertexBuffer> Build(DevicePtr& device, const dg::Char* name = nullptr) {
        return std::make_shared<VertexBuffer>(device, m_data.data(), m_data.size(), name);
    }

private:
    std::vector<uint8_t> m_data;
};
