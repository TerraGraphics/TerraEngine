#include "middleware/generator/mesh_generator.h"

#include "core/scene/geometry_node.h"
#include "middleware/generator/cube_shape.h"
#include "middleware/generator/plane_shape.h"
#include "middleware/generator/sphere_shape.h"
#include "middleware/generator/cylinder_shape.h"


std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidCube(DevicePtr& device) {
    CubeShape shape;

    VertexBufferBuilder vbBuilder;
    auto vb = shape.FillVertex(vbBuilder);

    IndexBufferBuilder ibBuilder;
    auto ib = shape.FillIndex(ibBuilder);

    return std::make_shared<GeometryNode>(vbBuilder.Build(device, "cube vb"), vb.OffsetBytes(),
        ibBuilder.Build(device, "cube ib"), ib.OffsetBytes(), ib.Count(), ib.IsUint32());
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidSphere(DevicePtr& device, uint32_t cntCirclePoints) {
    SphereShape shape(cntCirclePoints);

    VertexBufferBuilder vbBuilder;
    auto vb = shape.FillVertex(vbBuilder);

    IndexBufferBuilder ibBuilder;
    auto ib = shape.FillIndex(ibBuilder);

    return std::make_shared<GeometryNode>(vbBuilder.Build(device, "sphere vb"), vb.OffsetBytes(),
        ibBuilder.Build(device, "sphere ib"), ib.OffsetBytes(), ib.Count(), ib.IsUint32());
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidCylinder(DevicePtr& device, uint32_t cntCirclePoints) {
    CylinderShape shape(cntCirclePoints);

    VertexBufferBuilder vbBuilder;
    auto vb = shape.FillVertex(vbBuilder);

    IndexBufferBuilder ibBuilder;
    auto ib = shape.FillIndex(ibBuilder);

    return std::make_shared<GeometryNode>(vbBuilder.Build(device, "cylinder vb"), vb.OffsetBytes(),
        ibBuilder.Build(device, "cylinder ib"), ib.OffsetBytes(), ib.Count(), ib.IsUint32());
}

static std::shared_ptr<GeometryNode> CreateSolidPlane(DevicePtr& device, uint32_t cntWidthPoints, uint32_t cntHeightPoints, float scaleTextureWidth, float scaleTextureHeight, Axis axisUp) {
    PlaneShape shape(cntWidthPoints, cntHeightPoints, scaleTextureWidth, scaleTextureHeight, axisUp);

    VertexBufferBuilder vbBuilder;
    auto vb = shape.FillVertex(vbBuilder);

    IndexBufferBuilder ibBuilder;
    auto ib = shape.FillIndex(ibBuilder);

    return std::make_shared<GeometryNode>(vbBuilder.Build(device, "plane vb"), vb.OffsetBytes(),
        ibBuilder.Build(device, "plane ib"), ib.OffsetBytes(), ib.Count(), ib.IsUint32());
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidPlaneXZ(DevicePtr& device, uint32_t cntXPoints, uint32_t cntZPoints, float scaleTextureX, float scaleTextureZ) {
    return CreateSolidPlane(device, cntXPoints, cntZPoints, scaleTextureX, scaleTextureZ, Axis::Y);
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidPlaneXY(DevicePtr& device, uint32_t cntXPoints, uint32_t cntYPoints, float scaleTextureX, float scaleTextureY) {
    return CreateSolidPlane(device, cntXPoints, cntYPoints, scaleTextureX, scaleTextureY, Axis::Z);
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidPlaneZY(DevicePtr& device, uint32_t cntZPoints, uint32_t cntYPoints, float scaleTextureZ, float scaleTextureY) {
    return CreateSolidPlane(device, cntZPoints, cntYPoints, scaleTextureZ, scaleTextureY, Axis::X);
}
