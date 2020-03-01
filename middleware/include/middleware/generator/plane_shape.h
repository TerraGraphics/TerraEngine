#pragma once

#include "core/math/basic.h"
#include "middleware/generator/shape.h"


struct VertexPNC;
template <typename Vertex> class VertexBufferRange;
class PlaneShape : public FlatPlaneGenerator {
public:
    PlaneShape() = delete;
    /*!
        Creates a square plane with a center at the beginning of coordinates.
        The plane is located in the {axes[0], axes[1]} plane, the normals are directed along the third axis.
        The number of segments along the axes[N]-axis equals segments[N] (segments[N] >= 1).
        Edge side[N] of the plane is equal to the sizes[N] (sizes[N] > 0)
    */
    PlaneShape(const math::UInt2& segments, const math::Axis2& axes, const dg::float2& sizes = dg::float2(1.f, 1.f));
    ~PlaneShape() = default;

protected:
    void FillVertex(VertexBufferRange<VertexPNC>& vb) const override;

private:
    math::Axis2 m_axes;
    dg::float2 m_sizes;
};
