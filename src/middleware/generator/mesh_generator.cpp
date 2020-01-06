#include "middleware/generator/mesh_generator.h"

#include "core/scene/geometry_node.h"
#include "middleware/generator/cube_shape.h"
#include "middleware/generator/plane_shape.h"
#include "middleware/generator/sphere_shape.h"
#include "middleware/generator/shape_builder.h"
#include "middleware/generator/cylinder_shape.h"


std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidCube(DevicePtr& device) {
    CubeShape shape;
    return ShapeBuilder(device).Join({&shape}, "Cube");
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidSphere(DevicePtr& device, uint32_t cntCirclePoints) {
    SphereShape shape(cntCirclePoints);
    return ShapeBuilder(device).Join({&shape}, "Sphere");
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidCylinder(DevicePtr& device, uint32_t cntCirclePoints) {
    CylinderShape shape(cntCirclePoints);
    return ShapeBuilder(device).Join({&shape}, "Cylinder");
}

static std::shared_ptr<GeometryNode> CreateSolidPlane(DevicePtr& device, uint32_t cntWidthPoints, uint32_t cntHeightPoints, float scaleTextureWidth, float scaleTextureHeight, Axis axisUp) {
    PlaneShape shape(cntWidthPoints, cntHeightPoints, scaleTextureWidth, scaleTextureHeight, axisUp);
    return ShapeBuilder(device).Join({&shape}, "Plane");
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
