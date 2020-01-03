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
    static std::shared_ptr<GeometryNode> CreateSolidCube(DevicePtr device);
    /*!
        Creates a sphere with a center at the beginning of coordinates and a diameter equal to 1
        cntVertexCircle - Number of vertices in the circle
    */
    static std::shared_ptr<GeometryNode> CreateSolidSphere(DevicePtr device, uint16_t cntVertexCircle);
    /*!
        Creates a cylinder with a center at the beginning of coordinates, with a diameter and height equal to 1
        cntVertexCircle - Number of vertices in the base circle
    */
    static std::shared_ptr<GeometryNode> CreateSolidCylinder(DevicePtr device, uint16_t cntVertexCircle);
    /*!
        Creates a square plane at the beginning of the coordinates with the edge side equal to 1.
        The plane is located in the X0Z plane, the normals are directed along the Y axis.

        cntXSides - Number of vertices on 0X axis (cntXSides >= 2)
        cntZSides - Number of vertices on 0Z axis (cntZSides >= 2)
    */
    static std::shared_ptr<GeometryNode> CreateSolidPlane(DevicePtr device, uint32_t cntXSides = 2, uint32_t cntZSides = 2, float scaleTextureX = 1.0f, float scaleTextureZ = 1.0f);
};
