#pragma once

#include "middleware/generator/shape.h"


class PlaneShape : public Shape {
public:
    PlaneShape() = delete;
    PlaneShape(uint32_t cntWidthPoints, uint32_t cntHeightPoints, float scaleTextureWidth, float scaleTextureHeight, Axis axisUp);
    ~PlaneShape() = default;

protected:
    void FillVertex(VertexBufferRange<VertexPNC>& vb) const override;
    void FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const override;

private:
    uint32_t m_cntWidthPoints;
    uint32_t m_cntHeightPoints;
    float m_scaleTextureWidth;
    float m_scaleTextureHeight;
    Axis m_axisUp;
};
