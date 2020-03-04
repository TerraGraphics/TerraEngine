#include "middleware/generator/shape.h"

 #include "core/scene/vertexes.h"
 #include "core/scene/vertex_buffer.h"


void Shape::SetTranform(const dg::float4x4& matrix) {
    m_matrix = matrix;
    m_matrixChanged = true;
}

void Shape::SetTexScale(const dg::float2& value) {
    m_texScale = value;
}
