#pragma once

#include <cstdint>

#include "core/dg/math.h"
#include "core/math/basic.h"
#include "core/common/ctor.h"
#include "core/scene/vertexes.h"
#include "middleware/generator/shape_generator.h"


class UVGridGenerator : public IShapeGenerator {
public:
    UVGridGenerator() = delete;
    UVGridGenerator(math::UInt2 segments);

    size_t LenghtVertex() const final;
    size_t LenghtIndex() const final;
    void FillVertex(VertexPNC* vertexes) const final;
    void FillIndex(uint32_t* indexes, uint32_t vertexStartIndex) const final;

private:
    math::UInt2 m_segments;
};
