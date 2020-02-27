#include "middleware/generator/cone_shape.h"

#include <cstdint>

#include "core/math/constants.h"
#include "core/scene/vertexes.h"
#include "core/common/exception.h"


ConeShape::ConeShape(const UInt2& segments, const math::Axis& axisUp, float radius, float height)
    : FlatPlaneGenerator(segments)
    , m_axisUp(axisUp)
    , m_radius(radius)
    , m_height(height) {

    if (segments.x < 3) {
        throw EngineError("minimum value for segments.x in ConeShape is 3");
    }
    if (segments.y < 1) {
        throw EngineError("minimum value for segments.y in ConeShape is 1");
    }
    if (radius <= 0.f) {
        throw EngineError("radius value in ConeShape must be greater than zero");
    }
    if (height <= 0.f) {
        throw EngineError("height value in ConeShape must be greater than zero");
    }
}

void ConeShape::FillVertex(VertexBufferRange<VertexPNC>& vb) const {
    uint32_t axis0 = static_cast<uint32_t>(m_axisUp);
    uint32_t axis1 = (axis0 + 2) % 3;
    uint32_t axis2 = (axis0 + 1) % 3;
    // tan(coneAngle) == radius / height
    // sizeCone = sqrt(radius * radius + height * height)
    auto oneOverSizeCone = 1.f / std::hypot(m_radius, m_height);
    auto radiusNorm = m_height * oneOverSizeCone; // cos(coneAngle) = height / sizeCone
    auto normUp = m_radius * oneOverSizeCone; // sin(coneAngle) = radius / sizeCone
    Generate(vb, [axis0, axis1, axis2, radiusNorm, normUp, radius = m_radius, height = m_height](const dg::float2& c, VertexPNC& out) {
        float circleAngle = TwoPI<float>() * c.x;

        float posUp = (c.y - 0.5f) * height;
        float radiusPos = (1.f - c.y) * radius;
        auto cosA = std::cos(circleAngle);
        auto sinA = std::sin(circleAngle);

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
