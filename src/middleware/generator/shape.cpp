#include "middleware/generator/shape.h"


Shape::Shape(uint32_t vertexCount, uint32_t indexCount)
    : m_vertexCount(vertexCount)
    , m_indexCount(indexCount) {

}

void Shape::SetTranform(const dg::float4x4& matrix) {
    m_matrix = matrix;
    m_matrixChanged = true;
}
