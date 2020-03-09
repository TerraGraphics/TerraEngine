#include "middleware/generator/cylinder_shape.h"

#include <cstdint>

#include "core/math/constants.h"
#include "core/scene/vertexes.h"
#include "core/common/exception.h"


CylinderShape::CylinderShape(const math::UInt2 segments, const math::Axis axisUp, float radius, float height)
    : Shape("CylinderShape", {axisUp, math::Prev(axisUp), math::Next(axisUp)})
    , m_generator(segments, [radius, height](VertexPNC* begin, VertexPNC* end) {
        for (VertexPNC* it=begin; it != end; ++it) {
            auto angle = TwoPI<float>() * it->uv.x;
            auto circleX = std::cos(angle);
            auto circleY = std::sin(angle);

            it->position = dg::float3((it->uv.y - 0.5f) * height, circleX * radius, circleY * radius);
            it->normal = dg::normalize(dg::float3(0.f, circleX, circleY));
        }
    }) {

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

    SetGenerator(&m_generator);
}

CylinderShape::CylinderShape(CylinderShape&& other) noexcept
    : Shape(std::move(other))
    , m_generator(std::move(other.m_generator)) {
    SetGenerator(&m_generator);
}

CylinderShape& CylinderShape::operator=(CylinderShape&& other) noexcept {
    Shape::operator=(std::move(other));
    m_generator = std::move(other.m_generator);
    SetGenerator(&m_generator);

    return *this;
}
