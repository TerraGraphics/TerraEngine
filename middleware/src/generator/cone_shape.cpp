#include "middleware/generator/cone_shape.h"

#include <cstdint>

#include "core/math/constants.h"
#include "core/scene/vertexes.h"
#include "core/common/exception.h"


ConeShape::ConeShape(const math::UInt2& segments, const math::Axis& axisUp, float radius, float height, const dg::float3& center)
    : FlatPlaneGenerator("ConeShape", segments, {axisUp, math::Prev(axisUp), math::Next(axisUp)}, center) {

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

    // tan(coneAngle) == radius / height
    // sizeCone = sqrt(radius * radius + height * height)
    auto oneOverSizeCone = 1.f / std::hypot(radius, height);
    auto radiusNorm = height * oneOverSizeCone; // cos(coneAngle) = height / sizeCone
    auto normUp = radius * oneOverSizeCone; // sin(coneAngle) = radius / sizeCone
    m_generator.SetCallback([radiusNorm, normUp, radius, height](const dg::float2& c) {
        VertexPNC out;
        float circleAngle = TwoPI<float>() * c.x;

        float posUp = (c.y - 0.5f) * height;
        float radiusPos = (1.f - c.y) * radius;
        auto cosA = std::cos(circleAngle);
        auto sinA = std::sin(circleAngle);

        out.position = dg::float3(posUp, cosA * radiusPos, sinA * radiusPos);
        out.normal = dg::normalize(dg::float3(normUp, cosA * radiusNorm, sinA * radiusNorm));

        return out;
    });
}
