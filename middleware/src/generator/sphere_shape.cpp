#include "middleware/generator/sphere_shape.h"

#include <cstdint>

#include "core/math/constants.h"
#include "core/scene/vertexes.h"
#include "core/common/exception.h"


SphereShape::SphereShape(const math::UInt2& segments, const math::Axis& axisUp, float radius, const dg::float3& center)
    : FlatPlaneGenerator("SphereShape", segments, {axisUp, math::Prev(axisUp), math::Next(axisUp)}, center) {

    if (segments.x < 3) {
        throw EngineError("minimum value for segments.x in SphereShape is 3");
    }
    if (segments.y < 3) {
        throw EngineError("minimum value for segments.y in SphereShape is 3");
    }
    if (radius <= 0.f) {
        throw EngineError("radius value in SphereShape must be greater than zero");
    }

    m_generator.SetCallback([radius](const dg::float2& c) {
        VertexPNC out;

        float angleA = TwoPI<float>() * c.x;
        float angleB = PI<float>() * c.y - HalfPI<float>();

        float posUp = std::sin(angleB) * radius;
        float circleRadius = std::cos(angleB) * radius;
        auto circleX = std::cos(angleA) * circleRadius;
        auto circleY = std::sin(angleA) * circleRadius;

        out.position = dg::float3(posUp, circleX, circleY);
        out.normal = dg::normalize(out.position);

        return out;
    });
}
