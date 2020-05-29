#include "middleware/generator/mesh/cone_shape.h"

#include <memory>
#include <utility>
#include <type_traits>

#include "core/math/constants.h"
#include "core/render/vertexes.h"
#include "core/common/exception.h"


ConeShape::ConeShape(const dg::uint2 segments, const math::Axis axisUp, float radius, float height)
    : Shape("ConeShape", {axisUp, math::Prev(axisUp), math::Next(axisUp)})
    , m_generator(segments, [radius, height](VertexPNC* begin, VertexPNC* end) {
        // tan(coneAngle) == radius / height
        // sizeCone = sqrt(radius * radius + height * height)
        auto oneOverSizeCone = 1.f / std::hypot(radius, height);
        auto radiusNorm = height * oneOverSizeCone; // cos(coneAngle) = height / sizeCone
        auto normUp = radius * oneOverSizeCone; // sin(coneAngle) = radius / sizeCone
        for (VertexPNC* it=begin; it != end; ++it) {
            float circleAngle = TwoPI<float>() * it->uv.x;

            float posUp = (it->uv.y - 0.5f) * height;
            float radiusPos = (1.f - it->uv.y) * radius;
            auto cosA = std::cos(circleAngle);
            auto sinA = std::sin(circleAngle);

            it->position = dg::float3(posUp, cosA * radiusPos, sinA * radiusPos);
            it->normal = dg::normalize(dg::float3(normUp, cosA * radiusNorm, sinA * radiusNorm));
        }
    }) {

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

    SetGenerator(&m_generator);
}

ConeShape::ConeShape(ConeShape&& other) noexcept
    : Shape(std::move(other))
    , m_generator(std::move(other.m_generator)) {
    SetGenerator(&m_generator);
}

ConeShape& ConeShape::operator=(ConeShape&& other) noexcept {
    Shape::operator=(std::move(other));
    m_generator = std::move(other.m_generator);
    SetGenerator(&m_generator);

    return *this;
}
