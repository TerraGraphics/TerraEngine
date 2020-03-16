#include "core/math/advanced.h"

#include <utility>
#include "core/math/equation.h"


namespace math {

/*
    point(x0, y0, z0), n(l, m, n)
    l*(x-x0) + m(y-y0) + n*(z-z0) = 0
*/
dg::float4 GetPlaneByPointAndNormal(dg::float3 point, dg::float3 n) {
    return dg::float4(n.x, n.y, n.z, -n.x*point.x - n.y*point.y - n.z*point.z);
}

/*
    point(x0, y0, z0), v0(a0, b0, c0), v1(a1, b1, c1)
    | x-x0 y-y0 z-z0 |
    |   a0   b0   c0 | = (x-x0) * | b0 c0 | - (y-y0) * | a0 c0 | + (z-z0) * | a0 b0 |
    |   a1   b1   c1 |            | b1 c1 |            | a1 c1 |            | a1 b1 |
*/
dg::float4 GetPlaneByPointAndTwoVec(dg::float3 point, dg::float3 v0, dg::float3 v1) {
    auto n = dg::cross(v0, v1);
    return dg::float4(n.x, n.y, n.z, -n.x*point.x - n.y*point.y - n.z*point.z);
}

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
    plane: plane.x * x + plane.y * y + plane.z * z + plane.w = 0
    ray:
        x = x0 + l*t
        y = y0 + m*t
        z = z0 + n*t
        0 <= t <= +inf

    intersection:
        plane.x * (x0 + l*t) + plane.y * (y0 + m*t) + plane.z * (z0 + n*t) + plane.w = 0
*/
bool IntersectionRayAndPlane(dg::float3 rayStart, dg::float3 rayDir, dg::float4 plane, dg::float3& result) {
    float t = - dg::dot(plane, dg::float4(rayStart, 1.f)) / dg::dot(static_cast<dg::float3>(plane), rayDir);

    // 0 <= t <= +inf
    if (t < 0) {
        return false;
    }

    result = rayStart + rayDir * t;

    return true;
}

}
