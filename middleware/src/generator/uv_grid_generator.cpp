#include "middleware/generator/uv_grid_generator.h"

#include "core/common/exception.h"


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
    auto uv = dg::float2(m_deltas.x * m_ind.x, m_deltas.y * m_ind.y);

    VertexPNC result = m_parent.m_callback(uv);
    result.position = dg::float3(
        m_parent.m_center.x + result.position[m_parent.m_permutations[0]],
        m_parent.m_center.y + result.position[m_parent.m_permutations[1]],
        m_parent.m_center.z + result.position[m_parent.m_permutations[2]]
    );

    result.normal = dg::float3(
        result.normal[m_parent.m_permutations[0]],
        result.normal[m_parent.m_permutations[1]],
        result.normal[m_parent.m_permutations[2]]
    );

    // convert to directX texture coord system
    result.uv = dg::float2(uv.u, 1.f - uv.v);

    return result;
}

UVGridGenerator::Vertexes::Vertexes(
    const math::UInt2& segments, const math::Axis3& orientation, const dg::float3& center, const Callback& callback)
    : m_segments(segments)
    , m_center(center)
    , m_callback(callback) {

    m_permutations[static_cast<uint32_t>(orientation[0])] = 0;
    m_permutations[static_cast<uint32_t>(orientation[1])] = 1;
    m_permutations[static_cast<uint32_t>(orientation[2])] = 2;
}

UVGridGenerator::Vertexes::Iterator UVGridGenerator::Vertexes::begin() const {
    return Iterator(*this);
}

UVGridGenerator::Vertexes::Iterator UVGridGenerator::Vertexes::end() const {
    return Iterator(math::UInt2(0, m_segments.y + 1), *this);
}

size_t UVGridGenerator::Vertexes::Lenght() const {
    return (m_segments.x + 1) * (m_segments.y + 1);
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

size_t UVGridGenerator::Indexes::Lenght() const {
    return m_segments.x * m_segments.y * 6;
}

UVGridGenerator::UVGridGenerator(
    const std::string& name, const math::UInt2& segments, const math::Axis3& orientation, const dg::float3& center)
    : m_segments(segments)
    , m_orientation(orientation)
    , m_center(center) {

    for (uint32_t i=0; i!=3; ++i) {
        auto axis = orientation[i];
        if ((axis != math::Axis::X) && (axis != math::Axis::Y) && (axis != math::Axis::Z)) {
            throw EngineError("wrong orientation[{}]={} value in {}", i, static_cast<uint32_t>(axis), name);
        }
        if (orientation[0] == orientation[1]) {
            throw EngineError("error in the orientation array in {}, elements number 0 and number 1 are equal", name);
        }
        if (orientation[0] == orientation[2]) {
            throw EngineError("error in the orientation array in {}, elements number 0 and number 2 are equal", name);
        }
        if (orientation[1] == orientation[2]) {
            throw EngineError("error in the orientation array in {}, elements number 1 and number 2 are equal", name);
        }
    }
}

void UVGridGenerator::SetCallback(Callback&& value) {
    m_callback = std::move(value);
}

const UVGridGenerator::Vertexes UVGridGenerator::GetVertexes() const {
    return Vertexes(m_segments, m_orientation, m_center, m_callback);
}

const UVGridGenerator::Indexes UVGridGenerator::GetIndexes(uint32_t vertexStartIndex) const {
    return Indexes(m_segments, vertexStartIndex);
}
