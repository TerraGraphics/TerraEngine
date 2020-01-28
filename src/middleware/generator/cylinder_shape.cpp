#include "middleware/generator/cylinder_shape.h"

#include "core/common/exception.h"


CylinderShape::CylinderShape(const UInt2& segments, const Axis& axisUp)
    : Shape((segments.x + 1) * (segments.y + 1), segments.x * segments.y * 6)
    , m_segments(segments)
    , m_axisUp(axisUp) {


    if (segments.x < 3) {
        throw EngineError("minimum value for segments.x in CylinderShape is 3");
    }
    if (segments.y < 1) {
        throw EngineError("minimum value for segments.y in CylinderShape is 1");
    }
}

void CylinderShape::FillVertex(VertexBufferRange<VertexPNC>& vb) const {
    uint32_t axis0 = static_cast<uint32_t>(m_axisUp);
    uint32_t axis1, axis2;
    if (m_axisUp == Axis::X) {
        axis1 = static_cast<uint32_t>(Axis::Z);
        axis2 = static_cast<uint32_t>(Axis::Y);
    } else if(m_axisUp == Axis::Y) {
        axis1 = static_cast<uint32_t>(Axis::X);
        axis2 = static_cast<uint32_t>(Axis::Z);
    } else {
        axis1 = static_cast<uint32_t>(Axis::Y);
        axis2 = static_cast<uint32_t>(Axis::X);
    }
    FlatGenerator(vb, m_segments, [axis0, axis1, axis2](const dg::float2& c, VertexPNC& out) {
        auto angle = TwoPI<float>() * c.x;
        auto circleX = std::cos(angle) * 0.5f;
        auto circleY = std::sin(angle) * 0.5f;

        out.position[axis0] = c.y - 0.5f;
        out.position[axis1] = circleX;
        out.position[axis2] = circleY;
        out.normal[axis0] = 0.f;
        out.normal[axis1] = circleX;
        out.normal[axis2] = circleY;
        out.normal = dg::normalize(out.normal);
        out.uv = Shape::ToDXTexCoord(c);
    });
}

void CylinderShape::FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const {
    FlatGenerator(ib, m_segments, vertexStartIndex);
}
