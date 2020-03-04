#include "middleware/generator/plane_shape.h"

#include <cstdint>

#include "core/scene/vertexes.h"
#include "core/common/exception.h"


PlaneShape::PlaneShape(const math::UInt2& segments, const math::Axis2& axes, math::Direction normalDir,
    const dg::float2& sizes, const dg::float3& center)
    : m_generator("PlaneShape", segments, {axes[0], axes[1], math::GetAxis(normalDir)}, center) {

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

    m_generator.SetCallback([sizes, normalValue = static_cast<float>(math::GetSign(normalDir))](const dg::float2& c) {
        VertexPNC out;
        out.position = dg::float3((c.x - 0.5f) * sizes[0], (c.y - 0.5f) * sizes[1], 0);
        out.normal = dg::float3(0, 0, normalValue);

        return out;
    });
}
