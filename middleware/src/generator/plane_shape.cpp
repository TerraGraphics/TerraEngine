#include "middleware/generator/plane_shape.h"

#include <memory>
#include <utility>
#include <type_traits>

#include "core/scene/vertexes.h"
#include "core/common/exception.h"


PlaneShape::PlaneShape(const math::Axis2 axes, math::Direction normalDir, const dg::float2 sizes, const dg::uint2 segments)
    : Shape("PlaneShape", {axes[0], axes[1], math::GetAxis(normalDir)})
    , m_generator({segments, math::GetSign(normalDir) == -1}, [sizes, normalDir](VertexPNC* begin, VertexPNC* end) {
        auto normal = dg::float3(0, 0, static_cast<float>(math::GetSign(normalDir)));
        for (VertexPNC* it=begin; it != end; ++it) {
            it->position = dg::float3((it->uv.x - 0.5f) * sizes[0], (it->uv.y - 0.5f) * sizes[1], 0);
            it->normal = normal;
        }
    }) {

    if (segments.x < 1) {
        throw EngineError("minimum value for segments.x in PlaneShape is 1");
    }
    if (segments.y < 1) {
        throw EngineError("minimum value for segments.y in PlaneShape is 1");
    }
    if (sizes.x < 0) {
        throw EngineError("minimum value for sizes.x in PlaneShape is greater than 0");
    }
    if (sizes.y < 0) {
        throw EngineError("minimum value for sizes.y in PlaneShape is greater than 0");
    }

    SetGenerator(&m_generator);
}

PlaneShape::PlaneShape(PlaneShape&& other) noexcept
    : Shape(std::move(other))
    , m_generator(std::move(other.m_generator)) {
    SetGenerator(&m_generator);
}

PlaneShape& PlaneShape::operator=(PlaneShape&& other) noexcept {
    Shape::operator=(std::move(other));
    m_generator = std::move(other.m_generator);
    SetGenerator(&m_generator);

    return *this;
}
