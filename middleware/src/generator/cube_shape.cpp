#include "middleware/generator/cube_shape.h"

#include "core/dg/math.h"
#include "core/scene/vertexes.h"
#include "core/common/exception.h"
#include "core/scene/vertex_buffer.h"


static auto MakeGenerator(const dg::float3 sizes, const math::UInt3 segments) {
    float offset = sizes.z * 0.5f;
    dg::float2 size = {sizes.x, sizes.y};
    math::UInt2 sg = {segments.x, segments.y};
    math::Axis2 axes = {math::Axis::X, math::Axis::Y};
    auto p0 = PlaneShape(axes, math::Direction::POS_Z, size, sg);
    p0.SetCenter({0, 0, offset});
    auto p1 = PlaneShape(axes, math::Direction::NEG_Z, size, sg);
    p1.SetCenter({0, 0, -offset});

    offset = sizes.x * 0.5f;
    size = {sizes.y, sizes.z};
    sg = {segments.y, segments.z};
    axes = {math::Axis::Y, math::Axis::Z};
    auto p2 = PlaneShape(axes, math::Direction::POS_X, size, sg);
    p2.SetCenter({offset, 0, 0});
    auto p3 = PlaneShape(axes, math::Direction::NEG_X, size, sg);
    p3.SetCenter({-offset, 0, 0});

    offset = sizes.y * 0.5f;
    size = {sizes.z, sizes.x};
    sg = {segments.z, segments.x};
    axes = {math::Axis::Z, math::Axis::X};
    auto p4 = PlaneShape(axes, math::Direction::POS_Y, size, sg);
    p4.SetCenter({0, offset, 0});
    auto p5 = PlaneShape(axes, math::Direction::NEG_Y, size, sg);
    p5.SetCenter({0, -offset, 0});

    return MergeShape<PlaneShape, PlaneShape, PlaneShape, PlaneShape, PlaneShape, PlaneShape>(
        std::move(p0),
        std::move(p1),
        std::move(p2),
        std::move(p3),
        std::move(p4),
        std::move(p5));
}

CubeShape::CubeShape(const dg::float3 sizes, const math::UInt3 segments)
    : Shape("CubeShape", {math::Axis::X, math::Axis::Y, math::Axis::Z})
    , m_generator(MakeGenerator(sizes, segments)) {

    if (segments.x < 1) {
        throw EngineError("minimum value for segments.x in CubeShape is 1");
    }
    if (segments.y < 1) {
        throw EngineError("minimum value for segments.y in CubeShape is 1");
    }
    if (segments.z < 1) {
        throw EngineError("minimum value for segments.z in CubeShape is 1");
    }
    if (sizes.x < 0) {
        throw EngineError("minimum value for sizes.x in CubeShape is greater than 0");
    }
    if (sizes.y < 0) {
        throw EngineError("minimum value for sizes.y in CubeShape is greater than 0");
    }
    if (sizes.z < 0) {
        throw EngineError("minimum value for sizes.z in CubeShape is greater than 0");
    }

    SetGenerator(&m_generator);
}

CubeShape::CubeShape(CubeShape&& other) noexcept
    : Shape(std::move(other))
    , m_generator(std::move(other.m_generator)) {
    SetGenerator(&m_generator);
}

CubeShape& CubeShape::operator=(CubeShape&& other) noexcept {
    Shape::operator=(std::move(other));
    m_generator = std::move(other.m_generator);
    SetGenerator(&m_generator);

    return *this;
}
