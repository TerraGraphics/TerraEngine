#pragma once

#include <cstddef>
#include <cstdint>

#include "core/math/basic.h"
#include "core/common/ctor.h"
#include "middleware/generator/shape_generator.h"


struct VertexPNC;
class UVGridGenerator : public IShapeGenerator, Noncopyable {
public:
    UVGridGenerator() = delete;
    UVGridGenerator(math::UInt2 segments, bool counterClockwise = false);
    UVGridGenerator(UVGridGenerator&& other) noexcept;
    UVGridGenerator& operator=(UVGridGenerator&& other) noexcept;

    size_t LenghtVertex() const final;
    size_t LenghtIndex() const final;
    void FillVertex(VertexPNC* vertexes) const final;
    void FillIndex(uint32_t* indexes, uint32_t vertexStartIndex) const final;

private:
    math::UInt2 m_segments;
    bool m_counterClockwise;
};
