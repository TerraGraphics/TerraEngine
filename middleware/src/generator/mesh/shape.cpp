#include "middleware/generator/mesh/shape.h"

#include <utility>

 #include "core/render/vertexes.h"
 #include "core/common/exception.h"
 #include "core/math/normal_matrix.h"


Shape::Shape(const std::string& name, const math::Axis3& orientation) {
    for (uint32_t i=0; i!=3; ++i) {
        auto axis = orientation[i];
        m_axisPermutations[static_cast<uint32_t>(axis)] = i;
        if ((axis != math::Axis::X) && (axis != math::Axis::Y) && (axis != math::Axis::Z)) {
            throw EngineError("wrong orientation[{}]={} value in {}", i, static_cast<uint32_t>(axis), name);
        }
        if (orientation[0] == orientation[1]) {
            throw EngineError("error in the orientation array in {}, elements number 0 and number 1 are equal", name);
        }
        if (orientation[0] == orientation[2]) {
            throw EngineError("error in the orientation array in {}, elements number 0 and number 2 are equal", name);
        }
        if (orientation[1] == orientation[2]) {
            throw EngineError("error in the orientation array in {}, elements number 1 and number 2 are equal", name);
        }
    }
}

Shape::Shape(Shape&& other) noexcept
    : m_axisPermutations{other.m_axisPermutations[0], other.m_axisPermutations[1], other.m_axisPermutations[2]}
    , m_uvScale(std::move(other.m_uvScale))
    , m_shapeCenter(std::move(other.m_shapeCenter))
    , m_normalMatrix(std::move(other.m_normalMatrix))
    , m_vertexMatrix(std::move(other.m_vertexMatrix))
    , m_matrixChanged(other.m_matrixChanged)
    , m_baseGenerator(nullptr) {

    other.m_baseGenerator = nullptr;
}

Shape& Shape::operator=(Shape&& other) noexcept {
    m_axisPermutations[0] = other.m_axisPermutations[0];
    m_axisPermutations[1] = other.m_axisPermutations[1];
    m_axisPermutations[2] = other.m_axisPermutations[2];
    m_uvScale = std::move(other.m_uvScale);
    m_shapeCenter = std::move(other.m_shapeCenter);
    m_normalMatrix = std::move(other.m_normalMatrix);
    m_vertexMatrix = std::move(other.m_vertexMatrix);
    m_matrixChanged = other.m_matrixChanged;
    m_baseGenerator = nullptr;
    other.m_baseGenerator = nullptr;

    return *this;
}

void Shape::SetUVScale(const dg::float2& value) {
    m_uvScale = value;
}

void Shape::SetCenter(const dg::float3& value) {
    m_shapeCenter = value;
}

void Shape::SetTransform(const dg::float4x4& value) {
    m_normalMatrix = MakeNormalMatrix3x3(value);
    m_vertexMatrix = value;
    m_matrixChanged = true;
}

size_t Shape::LenghtVertex() const {
    return m_baseGenerator ? m_baseGenerator->LenghtVertex() : 0;
}

size_t Shape::LenghtIndex() const {
    return m_baseGenerator ? m_baseGenerator->LenghtIndex() : 0;
}

void Shape::FillVertex(VertexPNC* vertexes) const {
    if (m_baseGenerator != nullptr) {
        m_baseGenerator->FillVertex(vertexes);
    }
    for (uint32_t i=0; i!=LenghtVertex(); ++i) {
        auto& v = vertexes[i];
        v.position = dg::float3(
            m_shapeCenter.x + v.position[m_axisPermutations[0]],
            m_shapeCenter.y + v.position[m_axisPermutations[1]],
            m_shapeCenter.z + v.position[m_axisPermutations[2]]
        );
        v.normal = dg::float3(
            v.normal[m_axisPermutations[0]],
            v.normal[m_axisPermutations[1]],
            v.normal[m_axisPermutations[2]]
        );
        // convert to directX texture coord system
        v.uv = dg::float2(v.uv.u * m_uvScale.x, (1.f - v.uv.v) * m_uvScale.y);
    }

    if (m_matrixChanged) {
        for (uint32_t i=0; i!=LenghtVertex(); ++i) {
            vertexes[i].position = vertexes[i].position * m_vertexMatrix;
            vertexes[i].normal = vertexes[i].normal * m_normalMatrix;
            vertexes[i].uv *= m_uvScale;
        }
    }
}

void Shape::FillIndex(uint32_t* indexes, uint32_t vertexStartIndex) const {
    if (m_baseGenerator != nullptr) {
        m_baseGenerator->FillIndex(indexes, vertexStartIndex);
    }
}

void Shape::SetGenerator(const IShapeGenerator* baseGenerator) {
    m_baseGenerator = baseGenerator;
}
