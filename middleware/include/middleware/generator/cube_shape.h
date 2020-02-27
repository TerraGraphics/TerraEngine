#pragma once

#include <cstdint>
#include "core/scene/index_buffer.h"
#include "middleware/generator/shape.h"


struct VertexPNC;
template <typename Vertex> class VertexBufferRange;
class CubeShape : public Shape {
public:
    /*!
       Creates a solid cube with a center at the beginning of the coordinates and a long edge equal to 1

                   21-------22
                   /       /
                  /       /
                20-------23
            9    6-------5    14
           /|    |       |    /|
          / |    | Y^  Z^|   / |
        10  | 1--|--|-2/ | 13  |
         |  8 |  7--|-|--4  | 15
         | /  |     |/|     | /
         |/   |     O-|-->X |/
        11    0-------3    12
             16-------19
             /       /
            /       /
          17------18
    */
    CubeShape();
    ~CubeShape() = default;

protected:
    void FillVertex(VertexBufferRange<VertexPNC>& vb) const override;
    void FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const override;
};
