#include "middleware/generator/sphere_shape.h"

#include <cstdint>

#include "core/math/constants.h"
#include "core/scene/vertexes.h"
#include "core/common/exception.h"


SphereShape::SphereShape(const math::UInt2& segments, const math::Axis& axisUp, float radius)
    : FlatPlaneGenerator(segments)
    , m_axisUp(axisUp)
    , m_radius(radius) {

    if (segments.x < 3) {
        throw EngineError("minimum value for segments.x in SphereShape is 3");
    }
    if (segments.y < 3) {
        throw EngineError("minimum value for segments.y in SphereShape is 3");
    }
    if (radius <= 0.f) {
        throw EngineError("radius value in SphereShape must be greater than zero");
    }
}

void SphereShape::FillVertex(VertexBufferRange<VertexPNC>& vb) const {
    uint32_t axis0 = static_cast<uint32_t>(m_axisUp);
    uint32_t axis1 = (axis0 + 2) % 3;
    uint32_t axis2 = (axis0 + 1) % 3;
    Generate(vb, [axis0, axis1, axis2, radius = m_radius](const dg::float2& c) {
        VertexPNC out;

        float angleA = TwoPI<float>() * c.x;
        float angleB = PI<float>() * c.y - HalfPI<float>();

        float posUp = std::sin(angleB) * radius;
        float circleRadius = std::cos(angleB) * radius;
        auto circleX = std::cos(angleA) * circleRadius;
        auto circleY = std::sin(angleA) * circleRadius;

        out.position[axis0] = posUp;
        out.position[axis1] = circleX;
        out.position[axis2] = circleY;
        out.normal = dg::normalize(out.position);
        out.uv = Shape::ToDXTexCoord(c);

        return out;
    });
}
