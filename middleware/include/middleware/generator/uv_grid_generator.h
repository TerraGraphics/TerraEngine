#pragma once

#include <cstdint>

#include "core/dg/math.h"
#include "core/math/basic.h"
#include "core/common/ctor.h"
#include "core/scene/vertexes.h"


class UVGridGenerator : Fixed {
public:
    using Callback = std::function<VertexPNC (const dg::float2&)>;
    struct Vertexes : private Fixed {
        struct Iterator : private Fixed {
            Iterator() = delete;
            Iterator(const Vertexes& parent);
            Iterator(math::UInt2&& ind, const Vertexes& parent);

            bool operator==(const Iterator& other) const { return m_ind == other.m_ind; }
            bool operator!=(const Iterator& other) const { return m_ind != other.m_ind; }
            Iterator& operator++();
            VertexPNC operator*() const;

        private:
            math::UInt2 m_ind = {0, 0};
            const dg::float2 m_deltas;
            const Vertexes& m_parent;
        };

        Vertexes() = delete;
        Vertexes(const math::UInt2& segments, const math::Axis3& orientation, const dg::float3& center, const Callback& callback);

        Iterator begin() const;
        Iterator end() const;

    private:
        math::UInt2 m_segments;
        dg::float3 m_center;
        uint32_t m_permutations[3];
        const Callback& m_callback;
    };

    struct Indexes : private Fixed {
        struct Iterator : private Fixed {
            Iterator() = delete;
            Iterator(const math::UInt2& segments, uint32_t offset);
            Iterator(math::UInt2&& ind);

            bool operator==(const Iterator& other) const { return m_ind == other.m_ind; }
            bool operator!=(const Iterator& other) const { return m_ind != other.m_ind; }
            Iterator& operator++();
            uint32_t operator*() const;

        private:
            math::UInt2 m_ind = {0, 0};
            uint32_t m_offset;
            uint32_t m_triangleIndex = 0;
            math::UInt2 m_segments;
            uint32_t m_triangleTable[6];
        };

        Indexes() = delete;
        Indexes(const math::UInt2& segments, uint32_t offset);

        Iterator begin() const;
        Iterator end() const;

    private:
        math::UInt2 m_segments;
        uint32_t m_offset;
    };

public:
    UVGridGenerator() = delete;
    UVGridGenerator(const std::string& name, const math::UInt2& segments, const math::Axis3& orientation);

    void SetCallback(Callback&& value);

    const Vertexes GetVertexes(const dg::float3& center = dg::float3(0, 0, 0)) const;
    const Indexes GetIndexes(uint32_t vertexStartIndex = 0) const;

private:
    math::UInt2 m_segments;
    math::Axis3 m_orientation;
    Callback m_callback;
};
