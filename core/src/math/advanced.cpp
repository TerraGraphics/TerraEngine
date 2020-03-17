#include "core/math/advanced.h"

#include <utility>
#include "core/math/equation.h"


namespace math {

/*
    Cylinder 0Z:
        x*x + y*y = r*r
        0 <= z <= height

    Ray from (x0, y0, z0) with direction (l, m, n):
        x = x0 + l*t
        y = y0 + m*t
        z = z0 + n*t
        0 <= t <= +inf

    Intersection:
        (x0 + l*t)^2 + (y0 + m*t)^2 = r*r
*/
bool IntersectionRayAndCylinder(dg::float3 rayStart, dg::float3 rayDir, Axis axisUp, float radius, float height) {
    // reduce the task to a cylinder OZ
    switch (axisUp) {
    case Axis::X:
        std::swap(rayStart.x, rayStart.z);
        std::swap(rayDir.x, rayDir.z);
        break;
    case Axis::Y:
        std::swap(rayStart.y, rayStart.z);
        std::swap(rayDir.y, rayDir.z);
        break;
    default:
        break;
    }

    auto p = dg::float3(
        rayDir.x * rayDir.x + rayDir.y * rayDir.y,
        2.f * (rayStart.x * rayDir.x + rayStart.y * rayDir.y),
        rayStart.x * rayStart.x + rayStart.y * rayStart.y - radius * radius);

    dg::float2 t;
    if (SolveQuad(p.x, p.y, p.z, &t[0]) != 2) {
        return false;
    }

    // 0 <= t <= +inf
    if ((t.x < 0) && (t.y < 0)) {
        return false;
    }

    // 0 <= z <= height
    float z = rayStart.z + rayDir.z * t.x;
    if ((z >= 0) && (z <= height)) {
        return true;
    }

    z = rayStart.z + rayDir.z * t.y;
    if ((z >= 0) && (z <= height)) {
        return true;
    }

    return false;
}

/*
    plane: plane.a * x + plane.b * y + plane.c * z + plane.d = 0
    ray:
        x = x0 + l*t
        y = y0 + m*t
        z = z0 + n*t
        0 <= t <= +inf

    intersection:
        plane.a * (x0 + l*t) + plane.b * (y0 + m*t) + plane.c * (z0 + n*t) + plane.d = 0
*/
bool IntersectionRayAndPlane(dg::float3 rayStart, dg::float3 rayDir, PlaneT<float> plane, dg::float3& result) {
    float t = -
        dg::dot(static_cast<dg::float4>(plane), dg::float4(rayStart, 1.f)) /
        dg::dot(static_cast<dg::float3>(plane), rayDir);

    // 0 <= t <= +inf
    if (t < 0) {
        return false;
    }

    result = rayStart + rayDir * t;

    return true;
}

}
