#pragma once

#include <cstdint>

#include "core/math/types.h"


namespace math {
    /*
        Intersection between ray and cylinder
    */
    bool IsIntersection(const RayT<double>& ray, const CylinderT<double>& cylinder);
    bool IsIntersection(const RayT<float>& ray, const CylinderT<float>& cylinder);

    /*
        Intersection between ray and torus
        Param: result - array of size 4
        Return count of result [0; 4]
    */
    uint8_t Intersection(const RayT<double>& ray, const TorusT<double>& torus, dg::double3* result);
    uint8_t Intersection(const RayT<float>& ray, const TorusT<float>& torus, dg::float3* result);

    bool IsIntersection(const RayT<double>& ray, const TorusT<double>& torus);
    bool IsIntersection(const RayT<float>& ray, const TorusT<float>& torus);

    /*
        Intersection between ray and plane
        Param: result - one vector3
        Return count of result [0; 1]
    */
    uint8_t Intersection(const RayT<double>& ray, const PlaneT<double>& plane, dg::double3& result);
    uint8_t Intersection(const RayT<float>& ray, const PlaneT<float>& plane, dg::float3& result);

}
