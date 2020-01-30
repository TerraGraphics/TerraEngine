#include "middleware/generator/sphere_shape.h"

#include "core/common/exception.h"


SphereShape::SphereShape(const UInt2& segments, const Axis& axisUp)
    : Shape((segments.x + 1) * (segments.y + 1), segments.x * segments.y * 6)
    , m_segments(segments)
    , m_axisUp(axisUp) {

    if (segments.x < 3) {
        throw EngineError("minimum value for segments.x in SphereShape is 3");
    }
    if (segments.y < 3) {
        throw EngineError("minimum value for segments.y in SphereShape is 3");
    }
}

void SphereShape::FillVertex(VertexBufferRange<VertexPNC>& vb) const {
    uint32_t axis0 = static_cast<uint32_t>(m_axisUp);
    uint32_t axis1 = (axis0 + 2) % 3;
    uint32_t axis2 = (axis0 + 1) % 3;
    FlatGenerator(vb, m_segments, [axis0, axis1, axis2](const dg::float2& c, VertexPNC& out) {
        float angleA = TwoPI<float>() * c.x;
        float angleB = PI<float>() * c.y - HalfPI<float>();

        float posUp = std::sin(angleB) * 0.5f;
        float radius = std::cos(angleB) * 0.5f;
        auto circleX = std::cos(angleA) * radius;
        auto circleY = std::sin(angleA) * radius;

        out.position[axis0] = posUp;
        out.position[axis1] = circleX;
        out.position[axis2] = circleY;
        out.normal = dg::normalize(out.position);
        out.uv = Shape::ToDXTexCoord(c);
    });
}

void SphereShape::FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const {
    FlatGenerator(ib, m_segments, vertexStartIndex);
}
