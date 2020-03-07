#include "middleware/generator/plane_shape.h"

#include <cstdint>

#include "core/scene/vertexes.h"
#include "core/common/exception.h"


PlaneShape::PlaneShape(const math::UInt2 segments, const math::Axis2 axes, math::Direction normalDir, const dg::float2 sizes)
    : Shape("PlaneShape", {axes[0], axes[1], math::GetAxis(normalDir)})
    , m_generator(segments, [sizes, normalDir](VertexPNC* begin, VertexPNC* end) {
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
