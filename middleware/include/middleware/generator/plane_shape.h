#pragma once

#include "core/math/basic.h"
#include "middleware/generator/shape.h"


struct VertexPNC;
template <typename Vertex> class VertexBufferRange;
class PlaneShape : public FlatPlaneGenerator {
public:
    PlaneShape() = delete;
    /*!
        Creates a square plane with a center at the beginning of coordinates with the edge side equal to 1.
        The plane is located in the {axes[0], axes[1]} plane, the normals are directed along the third axis.
        The number of segments along the axes[N]-axis equals segments[N] (segments[N] >= 1).
    */
    PlaneShape(const UInt2& segments, const dg::Vector2<math::Axis>& axes);
    ~PlaneShape() = default;

protected:
    void FillVertex(VertexBufferRange<VertexPNC>& vb) const override;

private:
    dg::Vector2<math::Axis> m_axes;
};
