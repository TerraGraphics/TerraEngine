#include "core/math/advanced.h"


namespace math {

bool QuadEquation(dg::float3 p, dg::float2& result) {
    float d = p.y * p.y - 4.f * p.x * p.z;
    if (d < 0) {
        return false;
    }
    d = std::sqrt(d);
    result.x = (-p.y + d) * 0.5f / p.x;
    result.y = (-p.y - d) * 0.5f / p.x;

    return true;
}

}