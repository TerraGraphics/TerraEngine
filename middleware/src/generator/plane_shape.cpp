#include "middleware/generator/plane_shape.h"

#include <cstdint>

#include "core/scene/vertexes.h"
#include "core/common/exception.h"


PlaneShape::PlaneShape(const math::UInt2& segments, const math::Axis2& axes, const dg::float2& sizes)
    : FlatPlaneGenerator(segments)
    , m_axes(axes)
    , m_sizes(sizes) {

    if (segments.x < 1) {
        throw EngineError("minimum value for segments.x in PlaneShape is 1");
    }
    if (segments.y < 1) {
        throw EngineError("minimum value for segments.y in PlaneShape is 1");
    }
    if (axes[0] == axes[1]) {
        throw EngineError("axles must not match in PlaneShape");
    }
    if (sizes.x < 0) {
        throw EngineError("minimum value for sizes.x in PlaneShape is greater than 0");
    }
    if (sizes.y < 0) {
        throw EngineError("minimum value for sizes.y in PlaneShape is greater than 0");
    }
}

void PlaneShape::FillVertex(VertexBufferRange<VertexPNC>& vb) const {
    uint32_t axis0 = static_cast<uint32_t>(m_axes[0]);
    uint32_t axis1 = static_cast<uint32_t>(m_axes[1]);
    uint32_t axis2 = static_cast<uint32_t>(math::Axis::X) + static_cast<uint32_t>(math::Axis::Y) + static_cast<uint32_t>(math::Axis::Z) - axis0 - axis1;
    Generate(vb, [axis0, axis1, axis2, sizes = m_sizes](const dg::float2& c) {
        VertexPNC out;

        out.position[axis0] = (c.x - 0.5f) * sizes[0];
        out.position[axis1] = (c.y - 0.5f) * sizes[1];
        out.position[axis2] = 0.f;
        out.normal[axis0] = 0.f;
        out.normal[axis1] = 0.f;
        out.normal[axis2] = 1.f;
        out.uv = Shape::ToDXTexCoord(c);

        return out;
    });
}
