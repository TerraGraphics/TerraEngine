#pragma once

#include "middleware/generator/mesh/shape_generator.h"


class EmptyShape : public IShapeGenerator {
public:
    size_t LenghtVertex() const final {
        return 0;
    }

    size_t LenghtIndex() const final {
        return 0;
    }

    void FillVertex(VertexPNC*) const final {
    }

    void FillIndex(uint32_t*, uint32_t) const final {
    }
};
