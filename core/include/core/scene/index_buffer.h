#pragma once

#include <memory>
#include <vector>
#include <stdexcept>

#include "core/dg/dg.h"
#include "core/dg/buffer.h"
#include "core/common/ctor.h"
#include "core/common/counter.h"


class IndexBuffer : public Counter<IndexBuffer>, Fixed {
public:
    IndexBuffer() = delete;
    IndexBuffer(DevicePtr& device, const void* data, uint32_t size, const dg::Char* name = nullptr);
    ~IndexBuffer() = default;

    void Bind(ContextPtr& context, uint32_t offset);

private:
	BufferPtr m_indexBuffer;
};

template <typename T>
struct IndexTypesSupported {
    using type = typename std::enable_if<
    std::is_same<uint16_t, T>::value ||
    std::is_same<uint32_t, T>::value
    >::type;
};

template<typename Index, typename = typename IndexTypesSupported<Index>::type>
    class IndexBufferRange : Noncopyable {
public:
    IndexBufferRange() = delete;
    IndexBufferRange(Index* begin, uint32_t offsetBytes, uint32_t count) : m_begin(begin), m_offsetBytes(offsetBytes), m_count(count) {}
    IndexBufferRange(IndexBufferRange&& other) noexcept : m_begin(other.m_begin), m_offsetBytes(other.m_offsetBytes), m_count(other.m_count) {
        other.m_begin = nullptr;
        other.m_offsetBytes = 0;
        other.m_count = 0;
    }

    IndexBufferRange& operator=(IndexBufferRange&& other) noexcept {
        std::swap(m_begin, other.m_begin);
        std::swap(m_offsetBytes, other.m_offsetBytes);
        std::swap(m_count, other.m_count);

        return *this;
    }

    ~IndexBufferRange() = default;

public:
    Index* Begin() noexcept { return m_begin; }
    Index* End() noexcept { return m_begin + m_count; }

    uint32_t OffsetBytes() const noexcept { return m_offsetBytes; }
    uint32_t Count() const noexcept { return m_count; }
    bool IsUint32() const noexcept { return sizeof(Index) == sizeof(uint32_t); }

    inline Index& operator[](uint32_t idx) {
        if (idx < m_count) {
            return m_begin[idx];
        }
        throw std::out_of_range("IndexBufferRange operator[]");
    }

    inline const Index& operator[](uint32_t idx) const {
        if (idx < m_count) {
            return m_begin[idx];
        }
        throw std::out_of_range("IndexBufferRange operator[]");
    }

private:
    Index* m_begin = nullptr;
    uint32_t m_offsetBytes = 0;
    uint32_t m_count = 0;
};

class IndexBufferBuilder : Fixed {
public:
    IndexBufferBuilder() = default;
    ~IndexBufferBuilder() = default;

public:
    template<typename Index, typename = typename IndexTypesSupported<Index>::type>
    void Append(const std::vector<Index>& data) {
        const auto* begin = reinterpret_cast<uint8_t*>(data.data());
        m_data.insert(m_data.end(), begin, begin + data.size() * sizeof(Index));
    }

    template<typename Index, typename = typename IndexTypesSupported<Index>::type>
    IndexBufferRange<Index> AddRange(size_t indexCount) {
        auto offsetCount = static_cast<uint32_t>(m_data.size());
        m_data.resize(offsetCount + indexCount * sizeof(Index));
        return IndexBufferRange<Index>(reinterpret_cast<Index*>(&m_data[offsetCount]), offsetCount, indexCount);
    }

    std::shared_ptr<IndexBuffer> Build(DevicePtr& device, const dg::Char* name = nullptr) {
        return std::make_shared<IndexBuffer>(device, m_data.data(), m_data.size(), name);
    }

private:
    std::vector<uint8_t> m_data;
};
