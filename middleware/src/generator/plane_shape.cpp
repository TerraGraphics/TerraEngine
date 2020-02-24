#include "middleware/generator/plane_shape.h"

#include "core/common/exception.h"


PlaneShape::PlaneShape(const UInt2& segments, const dg::Vector2<math::Axis>& axes)
    : FlatPlaneGenerator(segments)
    , m_axes(axes) {

    if (segments.x < 1) {
        throw EngineError("minimum value for segments.x in PlaneShape is 1");
    }
    if (segments.y < 1) {
        throw EngineError("minimum value for segments.y in PlaneShape is 1");
    }
    if (m_axes[0] == m_axes[1]) {
        throw EngineError("axles must not match in PlaneShape");
    }
}

void PlaneShape::FillVertex(VertexBufferRange<VertexPNC>& vb) const {
    uint32_t axis0 = static_cast<uint32_t>(m_axes[0]);
    uint32_t axis1 = static_cast<uint32_t>(m_axes[1]);
    uint32_t axis2 = static_cast<uint32_t>(math::Axis::X) + static_cast<uint32_t>(math::Axis::Y) + static_cast<uint32_t>(math::Axis::Z) - axis0 - axis1;
    Generate(vb, [axis0, axis1, axis2](const dg::float2& c, VertexPNC& out) {
        out.position[axis0] = c.x - 0.5f;
        out.position[axis1] = c.y - 0.5f;
        out.position[axis2] = 0.f;
        out.normal[axis0] = 0.f;
        out.normal[axis1] = 0.f;
        out.normal[axis2] = 1.f;
        out.uv = Shape::ToDXTexCoord(c);
    });
}
