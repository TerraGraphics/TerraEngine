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
        Vertexes(math::UInt2 segments);

        void SetCallback(Callback&& value);

        Iterator begin() const;
        Iterator end() const;

    private:
        math::UInt2 m_segments;
        Callback m_callback;
    };

public:
    UVGridGenerator() = delete;
    UVGridGenerator(math::UInt2 segments);

    void SetCallback(Callback&& value);

    const Vertexes& GetVertexes() const { return m_vertexes; }

private:
    Vertexes m_vertexes;
};
