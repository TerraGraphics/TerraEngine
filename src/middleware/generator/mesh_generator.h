#pragma once

#include <memory>
#include "core/common/dg.h"


class GeometryNode;
struct MeshGenerator {
    /*!
       Creates a solid cube with a center at the beginning of the coordinates and a long edge equal to 1

                   21-------22
                   /       /
                  /       /
                20-------23
            9    6-------5    14
           /|    |       |    /|
          / |    | Y^  Z^|   / |
        10  | 1--|--|-2/ | 13  |
         |  8 |  7--|-|--4  | 15
         | /  |     |/|     | /
         |/   |     O-|-->X |/
        11    0-------3    12
             16-------19
             /       /
            /       /
          17------18
    */
    static std::shared_ptr<GeometryNode> CreateSolidCube(DevicePtr& device);
    /*!
        Creates a sphere with a center at the beginning of coordinates and a diameter equal to 1
        cntCirclePoints - Number of vertices in one circle of sphere (cntCirclePoints >= 3)
    */
    static std::shared_ptr<GeometryNode> CreateSolidSphere(DevicePtr& device, uint32_t cntCirclePoints);
    /*!
        Creates a cylinder with a center at the beginning of coordinates, with a diameter and height equal to 1
        cntCirclePoints - Number of vertices in the base circle (cntCirclePoints >= 3)
    */
    static std::shared_ptr<GeometryNode> CreateSolidCylinder(DevicePtr& device, uint32_t cntCirclePoints);
    /*!
        Creates a square plane with a center at the beginning of coordinates with the edge side equal to 1.
        The plane is located in the X0Z plane, the normals are directed along the Y axis.

        cntXPoints - Number of vertices on 0X axis (cntXPoints >= 2)
        cntZPoints - Number of vertices on 0Z axis (cntZPoints >= 2)
    */
    static std::shared_ptr<GeometryNode> CreateSolidPlaneXZ(DevicePtr& device, uint32_t cntXPoints = 2, uint32_t cntZPoints = 2, float scaleTextureX = 1.f, float scaleTextureZ = 1.f);
    /*!
        Creates a square plane with a center at the beginning of coordinates with the edge side equal to 1.
        The plane is located in the X0Y plane, the normals are directed along the Z axis.

        cntXPoints - Number of vertices on 0X axis (cntXPoints >= 2)
        cntYPoints - Number of vertices on 0Y axis (cntYPoints >= 2)
    */
    static std::shared_ptr<GeometryNode> CreateSolidPlaneXY(DevicePtr& device, uint32_t cntXPoints = 2, uint32_t cntYPoints = 2, float scaleTextureX = 1.f, float scaleTextureY = 1.f);
    /*!
        Creates a square plane with a center at the beginning of coordinates with the edge side equal to 1.
        The plane is located in the Z0Y plane, the normals are directed along the X axis.

        cntZPoints - Number of vertices on 0Z axis (cntZPoints >= 2)
        cntYPoints - Number of vertices on 0Y axis (cntYPoints >= 2)
    */
    static std::shared_ptr<GeometryNode> CreateSolidPlaneZY(DevicePtr& device, uint32_t cntZPoints = 2, uint32_t cntYPoints = 2, float scaleTextureZ = 1.f, float scaleTextureY = 1.f);
};
