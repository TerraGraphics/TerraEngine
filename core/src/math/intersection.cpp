#include "core/math/intersection.h"

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
template<typename T> static bool IsIntersectionT(RayT<T> ray, const CylinderT<T>& cylinder) {
    // reduce the task to a cylinder OZ
    switch (cylinder.axisUp) {
    case Axis::X:
        std::swap(ray.start.x, ray.start.z);
        std::swap(ray.direction.x, ray.direction.z);
        break;
    case Axis::Y:
        std::swap(ray.start.y, ray.start.z);
        std::swap(ray.direction.y, ray.direction.z);
        break;
    default:
        break;
    }

    T a = ray.direction.x * ray.direction.x + ray.direction.y * ray.direction.y;
    T b = 2.f * (ray.start.x * ray.direction.x + ray.start.y * ray.direction.y);
    T c = ray.start.x * ray.start.x + ray.start.y * ray.start.y - cylinder.radius * cylinder.radius;

    T t[2];
    if (SolveQuad(a, b, c, t) != 2) {
        return false;
    }

    // 0 <= t <= +inf
    if ((t[0] < 0) && (t[1] < 0)) {
        return false;
    }

    // 0 <= z <= height
    T z = ray.start.z + ray.direction.z * t[0];
    if ((z >= 0) && (z <= cylinder.height)) {
        return true;
    }

    z = ray.start.z + ray.direction.z * t[1];
    if ((z >= 0) && (z <= cylinder.height)) {
        return true;
    }

    return false;
}

bool IsIntersection(const RayT<double>& ray, const CylinderT<double>& cylinder) {
    return IsIntersectionT(ray, cylinder);
}

bool IsIntersection(const RayT<float>& ray, const CylinderT<float>& cylinder) {
    return IsIntersectionT(ray, cylinder);
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
bool IsIntersection(const RayT<double>& ray, const PlaneT<double>& plane, dg::double3& result) {
    double t = -
        dg::dot(static_cast<dg::double4>(plane), dg::double4(ray.start, 1.f)) /
        dg::dot(static_cast<dg::double3>(plane), ray.direction);

    // 0 <= t <= +inf
    if (t < 0) {
        return false;
    }

    result = ray.start + ray.direction * t;

    return true;
}

bool IsIntersection(const RayT<float>& ray, const PlaneT<float>& plane, dg::float3& result) {
    float t = -
        dg::dot(static_cast<dg::float4>(plane), dg::float4(ray.start, 1.f)) /
        dg::dot(static_cast<dg::float3>(plane), ray.direction);

    // 0 <= t <= +inf
    if (t < 0) {
        return false;
    }

    result = ray.start + ray.direction * t;

    return true;
}

}
