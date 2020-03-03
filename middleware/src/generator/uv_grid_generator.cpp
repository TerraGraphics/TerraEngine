#include "middleware/generator/uv_grid_generator.h"


UVGridGenerator::Vertexes::Iterator::Iterator(const Vertexes& parent)
    : m_deltas(1.f / parent.m_segments.x, 1.f / parent.m_segments.y)
    , m_parent(parent) {

}

UVGridGenerator::Vertexes::Iterator::Iterator(math::UInt2&& ind, const Vertexes& parent)
    : m_ind(std::move(ind))
    , m_parent(parent) {

}

UVGridGenerator::Vertexes::Iterator& UVGridGenerator::Vertexes::Iterator::operator++() {
    ++m_ind.x;
    if (m_ind.x > m_parent.m_segments.x) {
        m_ind.x = 0;
        ++m_ind.y;
    }
    return *this;
}

VertexPNC UVGridGenerator::Vertexes::Iterator::operator*() const {
    VertexPNC result = m_parent.m_callback(dg::float2(m_deltas.x * m_ind.x, m_deltas.y * m_ind.y));
    result.position = dg::float3(
        result.position[m_parent.m_permutations[0]],
        result.position[m_parent.m_permutations[1]],
        result.position[m_parent.m_permutations[2]]
    );
    result.normal = dg::float3(
        result.normal[m_parent.m_permutations[0]],
        result.normal[m_parent.m_permutations[1]],
        result.normal[m_parent.m_permutations[2]]
    );

    return result;
}

UVGridGenerator::Vertexes::Vertexes(const math::UInt2& segments, const math::Axis3& orientation)
    : m_segments(segments) {

    m_permutations[static_cast<uint32_t>(orientation[0])] = 0;
    m_permutations[static_cast<uint32_t>(orientation[1])] = 1;
    m_permutations[static_cast<uint32_t>(orientation[2])] = 2;
}

void UVGridGenerator::Vertexes::SetCallback(Callback&& value) {
    m_callback = std::move(value);
}

UVGridGenerator::Vertexes::Iterator UVGridGenerator::Vertexes::begin() const {
    return Iterator(*this);
}

UVGridGenerator::Vertexes::Iterator UVGridGenerator::Vertexes::end() const {
    return Iterator(math::UInt2(0, m_segments.y + 1), *this);
}

UVGridGenerator::Indexes::Iterator::Iterator(const math::UInt2& segments, uint32_t offset)
    : m_offset(offset)
    , m_segments(segments)
    , m_triangleTable{0, 1, segments.x + 1, segments.x + 1, 1, segments.x + 2} {

}

UVGridGenerator::Indexes::Iterator::Iterator(math::UInt2&& ind)
    : m_ind(std::move(ind)) {

}

UVGridGenerator::Indexes::Iterator& UVGridGenerator::Indexes::Iterator::operator++() {
    ++m_triangleIndex;
    if (m_triangleIndex == 6) {
        m_triangleIndex = 0;
        ++m_offset;
        ++m_ind.x;
        if (m_ind.x == m_segments.x) {
            m_ind.x = 0;
            ++m_ind.y;
            ++m_offset;
        }
    }

    return *this;
}

uint32_t UVGridGenerator::Indexes::Iterator::operator*() const {
    return m_offset + m_triangleTable[m_triangleIndex];
}

UVGridGenerator::Indexes::Indexes(const math::UInt2& segments, uint32_t offset)
    : m_segments(segments)
    , m_offset(offset) {

}

UVGridGenerator::Indexes::Iterator UVGridGenerator::Indexes::begin() const {
    return Iterator(m_segments, m_offset);
}

UVGridGenerator::Indexes::Iterator UVGridGenerator::Indexes::end() const {
    return Iterator(math::UInt2(0, m_segments.y));
}

UVGridGenerator::UVGridGenerator(const math::UInt2& segments, const math::Axis3& orientation)
    : m_vertexes(segments, orientation)
    , m_segments(segments) {

}

void UVGridGenerator::SetCallback(Callback&& value) {
    m_vertexes.SetCallback(std::move(value));
}
