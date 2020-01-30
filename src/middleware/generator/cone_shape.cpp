#include "middleware/generator/cone_shape.h"

#include "core/common/exception.h"


ConeShape::ConeShape(const UInt2& segments, const Axis& axisUp)
    : Shape((segments.x + 1) * (segments.y + 1), segments.x * segments.y * 6)
    , m_segments(segments)
    , m_axisUp(axisUp) {

    if (segments.x < 3) {
        throw EngineError("minimum value for segments.x in ConeShape is 3");
    }
    if (segments.y < 1) {
        throw EngineError("minimum value for segments.y in ConeShape is 1");
    }
}

void ConeShape::FillVertex(VertexBufferRange<VertexPNC>& vb) const {
    uint32_t axis0 = static_cast<uint32_t>(m_axisUp);
    uint32_t axis1 = (axis0 + 2) % 3;
    uint32_t axis2 = (axis0 + 1) % 3;
    // tan(coneAngle) == radius (0.5) / height (1.0) = sin(coneAngle) / cos(coneAngle)
    auto coneAngle = std::atan(0.5f);
    auto radiusNorm = std::cos(coneAngle);
    auto normUp = radiusNorm * 0.5f; // sin(coneAngle)
    FlatGenerator(vb, m_segments, [axis0, axis1, axis2, radiusNorm, normUp](const dg::float2& c, VertexPNC& out) {
        float angle = TwoPI<float>() * c.x;

        float posUp = c.y - 0.5f;
        float radiusPos = (1.f - c.y) * 0.5;
        auto cosA = std::cos(angle);
        auto sinA = std::sin(angle);

        out.position[axis0] = posUp;
        out.position[axis1] = cosA * radiusPos;
        out.position[axis2] = sinA * radiusPos;

        out.normal[axis0] = normUp;
        out.normal[axis1] = cosA * radiusNorm;
        out.normal[axis2] = sinA * radiusNorm;
        out.normal = dg::normalize(out.normal);

        out.uv = Shape::ToDXTexCoord(c);
    });
}

void ConeShape::FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const {
    FlatGenerator(ib, m_segments, vertexStartIndex);
}
