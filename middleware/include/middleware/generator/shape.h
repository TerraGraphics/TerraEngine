#pragma once

#include <string>
#include <cstdint>
#include <cstddef>

#include "core/math/basic.h"
#include "core/common/ctor.h"
#include "core/math/constants.h"
#include "middleware/generator/shape_generator.h"


struct VertexPNC;
class Shape : public IShapeGenerator, Noncopyable {
protected:
    Shape() = delete;
    Shape(const std::string& name, const math::Axis3& orientation);

public:
    Shape(Shape&& other) noexcept;
    Shape& operator=(Shape&& other) noexcept;

    void SetUVScale(const dg::float2& value);
    void SetCenter(const dg::float3& value);
    void SetTransform(const dg::float4x4& value);

    size_t LenghtVertex() const final;
    size_t LenghtIndex() const final;
    void FillVertex(VertexPNC* vertexes) const final;
    void FillIndex(uint32_t* indexes, uint32_t vertexStartIndex) const final;

protected:
    void SetGenerator(const IShapeGenerator* baseGenerator);

private:
    uint32_t m_axisPermutations[3];
    dg::float2 m_uvScale = {1.f, 1.f};
    dg::float3 m_shapeCenter = {0, 0, 0};
    dg::float3x3 m_normalMatrix = dg::One3x3;
    dg::float4x4 m_vertexMatrix = dg::One4x4;
    bool m_matrixChanged = false;
    const IShapeGenerator* m_baseGenerator = nullptr;
};
