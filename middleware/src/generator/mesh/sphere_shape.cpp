#include "middleware/generator/mesh/sphere_shape.h"

#include <memory>
#include <utility>
#include <type_traits>

#include "core/math/constants.h"
#include "core/scene/vertexes.h"
#include "core/common/exception.h"


SphereShape::SphereShape(const dg::uint2 segments, const math::Axis axisUp, float radius)
    : Shape("SphereShape", {axisUp, math::Prev(axisUp), math::Next(axisUp)})
    , m_generator(segments, [radius](VertexPNC* begin, VertexPNC* end) {
        for (VertexPNC* it=begin; it != end; ++it) {
            float angleA = TwoPI<float>() * it->uv.x;
            float angleB = PI<float>() * it->uv.y - HalfPI<float>();

            float posUp = std::sin(angleB) * radius;
            float circleRadius = std::cos(angleB) * radius;
            auto circleX = std::cos(angleA) * circleRadius;
            auto circleY = std::sin(angleA) * circleRadius;

            it->position = dg::float3(posUp, circleX, circleY);
            it->normal = dg::normalize(it->position);
        }
    }) {

    if (segments.x < 3) {
        throw EngineError("minimum value for segments.x in SphereShape is 3");
    }
    if (segments.y < 3) {
        throw EngineError("minimum value for segments.y in SphereShape is 3");
    }
    if (radius <= 0.f) {
        throw EngineError("radius value in SphereShape must be greater than zero");
    }

    SetGenerator(&m_generator);
}

SphereShape::SphereShape(SphereShape&& other) noexcept
    : Shape(std::move(other))
    , m_generator(std::move(other.m_generator)) {
    SetGenerator(&m_generator);
}

SphereShape& SphereShape::operator=(SphereShape&& other) noexcept {
    Shape::operator=(std::move(other));
    m_generator = std::move(other.m_generator);
    SetGenerator(&m_generator);

    return *this;
}
