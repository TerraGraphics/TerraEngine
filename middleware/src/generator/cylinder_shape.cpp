#include "middleware/generator/cylinder_shape.h"

#include <cstdint>

#include "core/math/constants.h"
#include "core/scene/vertexes.h"
#include "core/common/exception.h"


CylinderShape::CylinderShape(const math::UInt2& segments, const math::Axis& axisUp, float radius, float height)
    : FlatPlaneGenerator(segments)
    , m_axisUp(axisUp)
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
    uint32_t axis0 = static_cast<uint32_t>(m_axisUp);
    uint32_t axis1 = (axis0 + 2) % 3;
    uint32_t axis2 = (axis0 + 1) % 3;
    Generate(vb, [axis0, axis1, axis2, radius = m_radius, height = m_height](const dg::float2& c, VertexPNC& out) {
        auto angle = TwoPI<float>() * c.x;
        auto circleX = std::cos(angle);
        auto circleY = std::sin(angle);

        out.position[axis0] = (c.y - 0.5f) * height;
        out.position[axis1] = circleX * radius;
        out.position[axis2] = circleY * radius;
        out.normal[axis0] = 0.f;
        out.normal[axis1] = circleX;
        out.normal[axis2] = circleY;
        out.normal = dg::normalize(out.normal);
        out.uv = Shape::ToDXTexCoord(c);
    });
}