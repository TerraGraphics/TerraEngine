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
    Cylinder 0Z:
        (x*x + y*y + z*z + R*R - r*r)^2 - 4*R*R*(x*x + y*y) = 0

    Ray from (x0, y0, z0) with direction (l, m, n):
        x = x0 + l*t
        y = y0 + m*t
        z = z0 + n*t
        0 <= t <= +inf

    Intersection:
        ((x0 + l*t)*(x0 + l*t) + (y0 + m*t)*(y0 + m*t) + (z0 + n*t)*(z0 + n*t) + R*R - r*r)^2 - 4*R*R*((x0 + l*t)*(x0 + l*t) + (y0 + m*t)*(y0 + m*t)) = 0
        =>
        A*t^4 + B*t^3 + C*t^2 + D*t + E = 0
        l2=l*l; m2=m*m; n2=n*n;
        x2=x0*x0; y2=y0*y0; z2=z0*z0;
        r2=r*r; R2=R*R
        xyzrR2 = x2 + y2 + z2 - r2 - R2
        if (l, m, n) - normalized => l2 + m2 + n2 == 1
        =>
        // l2*l2+2*l2*m2+2*l2*n2+m2*m2+2*m2*n2+n2*n2
        A = (l2 + m2 + n2)^2 = 1

        // 4*l2*l*x0+4*l2*m*y0+4*l2*n*z0+4*l*m2*x0+4*l*n2*x0+4*m2*m*y0+4*m2*n*z0+4*m*n2*y0+4*n2*n*z0
        B = 4*(x0*l+y0*m+z0*n)*(l2+m2+n2) = 4*(x0*l+y0*m+z0*n)

        // -2*l2*r2-2*l2*R2+6*l2*x2+2*l2*y2+2*l2*z2+8*l*m*x0*y0+8*l*n*x0*z0-2*m2*r2-2*m2*R2+2*m2*x2+6*m2*y2+2*m2*z2+8*m*n*y0*z0-2*n2*r2+2*n2*R2+2*n2*x2+2*n2*y2+6*n2*z2
        C = 2*(2*(x0*l + y0*m + z0*n)^2 + (2*n2 - 1)*R2 + x2 + y2 + z2 - r2) = B*B*0.25 + 2*(2*n2*R2 + xyzrR2)

        // -4*l*r2*x0-4*l*R2*x0+4*l*x2*x0+4*l*x0*y2+4*l*x0*z2-4*m*r2*y0-4*m*R2*y0+4*m*x2*y0+4*m*y2*y0+4*m*y0*z2-4*n*r2*z0+4*n*R2*z0+4*n*x2*z0+4*n*y2*z0+4*n*z2*z0
        D = 4*(x0*l + y0*m)*(x2 + y2 + z2 - r2 - R2) + 4*z0*n*(x2 + y2 + z2 - r2 + R2) = 4*(x0*l + y0*m + z0*n)*xyzrR2 + 8*z0*n*R2

        // +r2*r2-2*r2*R2-2*r2*x2-2*r2*y2-2*r2*z2+R2*R2-2*R2*x2-2*R2*y2+2*R2*z2+x2*x2+2*x2*y2+2*x2*z2+y2*y2+2*y2*z2+z2*z2
        E = R2*R2 - 2*R2*(x2 + y2 - z2 + r2) + (x2 + y2 + z2 - r2)^2 = (x2 + y2 + z2 - r2 - R2)^2 + 4*R2*(z2 - r2) = xyzrR2^2 + 4*R2*(z2 - r2)
        =>
        A = 1
        B = 4*(x0*l+y0*m+z0*n)
        C = B*B*0.25 + 2*(2*n2*R2 + xyzrR2)
        D = B*xyzrR2 + 8*z0*n*R2
        E = xyzrR2^2 + 4*R2*(z2 - r2)
*/
uint8_t IntersectionImpl(const RayT<double>& ray, const TorusT<double>& torus, double* t) {
    auto direction = dg::normalize(ray.direction);
    auto start = ray.start;

    double l = direction.x;
    double m = direction.y;
    double n = direction.z;
    double x0 = start.x;
    double y0 = start.y;
    double z0 = start.z;
    double r2 = torus.minorRadius * torus.minorRadius;
    double R2 = torus.majorRadius * torus.majorRadius;
    double xyzrR2 = x0*x0 + y0*y0 + z0*z0 - r2 - R2;
    double rotDir = direction[static_cast<uint8_t>(torus.axisRotation)]; // torus.axisRotation direction
    double rotPoint = start[static_cast<uint8_t>(torus.axisRotation)]; // torus.axisRotation start point

    double B = 4*(x0*l + y0*m + z0*n);
    double C = B*B*0.25 + 2*(2*rotDir*rotDir*R2 + xyzrR2);
    double D = B*xyzrR2 + 8*rotPoint*rotDir*R2;
    double E = xyzrR2*xyzrR2 + 4*R2*(rotPoint*rotPoint - r2);

    auto cnt = SolveQuartic(B, C, D, E, t);

    // 0 <= t <= +inf
    uint8_t resultCnt = 0;
    for (uint8_t i=0; i!=cnt; ++i) {
        if (t[i] >= 0) {
            t[resultCnt++] = t[i];
        }
    }

    return resultCnt;
}

uint8_t Intersection(const RayT<double>& ray, const TorusT<double>& torus, dg::double3* result) {
    double t[4];
    auto cnt = IntersectionImpl(ray, torus, t);
    for (uint8_t i=0; i!=cnt; ++i) {
        result[i] = ray[t[i]];
    }

    return cnt;
}

uint8_t Intersection(const RayT<float>& ray, const TorusT<float>& torus, dg::float3* result) {
    auto rayDouble = RayT(dg::ToVector3<double>(ray.start), dg::ToVector3<double>(ray.direction));
    auto torusDouble = TorusT(static_cast<double>(torus.minorRadius), static_cast<double>(torus.majorRadius), torus.axisRotation);

    double t[4];
    auto cnt = IntersectionImpl(rayDouble, torusDouble, t);
    for (uint8_t i=0; i!=cnt; ++i) {
        result[i] = dg::ToVector3<float>(rayDouble[t[i]]);
    }

    return cnt;
}

bool IsIntersection(const RayT<double>& ray, const TorusT<double>& torus) {
    double t[4];
    return (IntersectionImpl(ray, torus, t) != 0);
}

bool IsIntersection(const RayT<float>& ray, const TorusT<float>& torus) {
    double t[4];
    return (IntersectionImpl(
        RayT(dg::ToVector3<double>(ray.start), dg::ToVector3<double>(ray.direction)),
        TorusT(static_cast<double>(torus.minorRadius), static_cast<double>(torus.majorRadius), torus.axisRotation),
        t) != 0);
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
