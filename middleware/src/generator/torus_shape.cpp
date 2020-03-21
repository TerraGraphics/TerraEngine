#include "middleware/generator/torus_shape.h"

#include <memory>
#include <utility>
#include <type_traits>

#include "core/math/constants.h"
#include "core/scene/vertexes.h"
#include "core/common/exception.h"


TorusShape::TorusShape(float minorRadius, float majorRadius, uint32_t minorSegments, uint32_t majorSegments, const math::Axis axisRotation)
    : Shape("TorusShape", {axisRotation, math::Prev(axisRotation), math::Next(axisRotation)})
    , m_generator(dg::uint2(minorSegments, majorSegments), [minorRadius, majorRadius](VertexPNC* begin, VertexPNC* end) {
        for (VertexPNC* it=begin; it != end; ++it) {
            auto majorAngle = TwoPI<float>() * it->uv.y;
            auto minorAngle = TwoPI<float>() * it->uv.x;
            auto minorCircleX = std::cos(minorAngle);
            auto minorCircleY = std::sin(minorAngle);

            auto minorPos = dg::float4(0, minorCircleX * minorRadius, minorCircleY * minorRadius + majorRadius, 1.f);
            auto minorNormal = dg::float4(0, minorCircleX, minorCircleY, 0);

            auto transformMatrix = dg::float4x4::RotationY(majorAngle);
            it->position = static_cast<dg::float3>(minorPos * transformMatrix);
            it->normal = dg::normalize(static_cast<dg::float3>(minorNormal * transformMatrix));
        }
    }) {

    if (minorSegments < 3) {
        throw EngineError("minimum value for minorSegments in TorusShape is 3");
    }
    if (majorSegments < 3) {
        throw EngineError("minimum value for majorSegments in TorusShape is 3");
    }
    if (minorRadius <= 0.f) {
        throw EngineError("minorRadius value in TorusShape must be greater than zero");
    }
    if (majorRadius <= 0.f) {
        throw EngineError("majorRadius value in TorusShape must be greater than zero");
    }

    SetGenerator(&m_generator);
}

TorusShape::TorusShape(TorusShape&& other) noexcept
    : Shape(std::move(other))
    , m_generator(std::move(other.m_generator)) {
    SetGenerator(&m_generator);
}

TorusShape& TorusShape::operator=(TorusShape&& other) noexcept {
    Shape::operator=(std::move(other));
    m_generator = std::move(other.m_generator);
    SetGenerator(&m_generator);

    return *this;
}
