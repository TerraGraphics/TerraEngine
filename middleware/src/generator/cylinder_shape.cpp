#include "middleware/generator/cylinder_shape.h"

#include <cstdint>

#include "core/math/constants.h"
#include "core/scene/vertexes.h"
#include "core/common/exception.h"


CylinderShape::CylinderShape(const math::UInt2& segments, const math::Axis& axisUp, float radius, float height)
    : FlatPlaneGenerator("CylinderShape", segments, {axisUp, math::Prev(axisUp), math::Next(axisUp)})
    , m_radius(radius)
    , m_height(height) {

    if (segments.x < 3) {
        throw EngineError("minimum value for segments.x in CylinderShape is 3");
    }
    if (segments.y < 1) {
        throw EngineError("minimum value for segments.y in CylinderShape is 1");
    }
    if (radius <= 0.f) {
        throw EngineError("radius value in CylinderShape must be greater than zero");
    }
    if (height <= 0.f) {
        throw EngineError("height value in CylinderShape must be greater than zero");
    }
}

void CylinderShape::FillVertex(VertexBufferRange<VertexPNC>& vb) const {
    Generate(vb, [radius = m_radius, height = m_height](const dg::float2& c) {
        VertexPNC out;
        auto angle = TwoPI<float>() * c.x;
        auto circleX = std::cos(angle);
        auto circleY = std::sin(angle);

        out.position = dg::float3((c.y - 0.5f) * height, circleX * radius, circleY * radius);
        out.normal = dg::normalize(dg::float3(0.f, circleX, circleY));

        return out;
    });
}
