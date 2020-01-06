#pragma once

#include "middleware/generator/shape.h"


class PlaneShape {
public:
    PlaneShape() = delete;
    PlaneShape(uint32_t cntWidthPoints, uint32_t cntHeightPoints, float scaleTextureWidth, float scaleTextureHeight, Axis axisUp);
    ~PlaneShape() = default;

    VertexBufferRange<VertexPNC> FillVertex(VertexBufferBuilder& vbBuilder);
    IndexBufferRange<uint32_t> FillIndex(IndexBufferBuilder& ibBuilder);

private:
    uint32_t m_cntWidthPoints;
    uint32_t m_cntHeightPoints;
    float m_scaleTextureWidth;
    float m_scaleTextureHeight;
    Axis m_axisUp;
};
