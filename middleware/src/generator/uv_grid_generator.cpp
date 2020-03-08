#include "middleware/generator/uv_grid_generator.h"

#include "core/common/exception.h"


UVGridGenerator::UVGridGenerator(math::UInt2 segments, bool counterClockwise)
    : m_segments(segments)
    , m_counterClockwise(counterClockwise) {

}

size_t UVGridGenerator::LenghtVertex() const {
    return (m_segments.x + 1) * (m_segments.y + 1);
}

size_t UVGridGenerator::LenghtIndex() const {
    return m_segments.x * m_segments.y * 6;
}

void UVGridGenerator::FillVertex(VertexPNC* vertexes) const {
    uint32_t ind = 0;
    dg::float2 coord = {0, 0};
    auto dtx = 1.f / static_cast<float>(m_segments.x);
    auto dty = 1.f / static_cast<float>(m_segments.y);

    for(uint32_t y=0; y!=(m_segments.y + 1); ++y) {
        for(uint32_t x=0; x!=(m_segments.x + 1); ++x) {
            vertexes[ind++].uv = coord;
            coord.x += dtx;
        }
        coord.y += dty;
        coord.x = 0;
    }
}

void UVGridGenerator::FillIndex(uint32_t* indexes, uint32_t vertexStartIndex) const {
    uint32_t ind = 0;
    uint32_t offset = vertexStartIndex;

    if (m_counterClockwise) {
        for(uint32_t y=0; y!=m_segments.y; ++y) {
            for(uint32_t x=0; x!=m_segments.x; ++x) {
                uint32_t bottomLeftVertex = offset + x;
                uint32_t topLeftVertex = bottomLeftVertex + m_segments.x + 1;

                indexes[ind++] = bottomLeftVertex + 1;
                indexes[ind++] = bottomLeftVertex;
                indexes[ind++] = topLeftVertex;

                indexes[ind++] = bottomLeftVertex + 1;
                indexes[ind++] = topLeftVertex;
                indexes[ind++] = topLeftVertex + 1;
            }
            offset += m_segments.x + 1;
        }
    } else {
        for(uint32_t y=0; y!=m_segments.y; ++y) {
            for(uint32_t x=0; x!=m_segments.x; ++x) {
                uint32_t bottomLeftVertex = offset + x;
                uint32_t topLeftVertex = bottomLeftVertex + m_segments.x + 1;

                indexes[ind++] = bottomLeftVertex;
                indexes[ind++] = bottomLeftVertex + 1;
                indexes[ind++] = topLeftVertex;

                indexes[ind++] = topLeftVertex;
                indexes[ind++] = bottomLeftVertex + 1;
                indexes[ind++] = topLeftVertex + 1;
            }
            offset += m_segments.x + 1;
        }
    }
}
