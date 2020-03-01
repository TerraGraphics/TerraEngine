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
    return m_parent.m_callback(dg::float2(m_deltas.x * m_ind.x, m_deltas.y * m_ind.y));
}

UVGridGenerator::Vertexes::Vertexes(math::UInt2 segments)
    : m_segments(std::move(segments)) {

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

UVGridGenerator::UVGridGenerator(math::UInt2 segments)
    : m_vertexes(segments) {

}

void UVGridGenerator::SetCallback(Callback&& value) {
    m_vertexes.SetCallback(std::move(value));
}