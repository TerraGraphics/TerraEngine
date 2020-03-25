#pragma once

#include <cstdint>


struct VertexPNC;
class IShapeGenerator {
public:
    virtual size_t LenghtVertex() const = 0;
    virtual size_t LenghtIndex() const = 0;
    virtual void FillVertex(VertexPNC* vertexes) const = 0;
    virtual void FillIndex(uint32_t* indexes, uint32_t vertexStartIndex) const = 0;
};
