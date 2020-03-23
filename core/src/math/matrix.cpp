#include "core/math/matrix.h"


namespace math {

dg::float3 GetTransform(const dg::float4x4& m) {
    return dg::float3(m._41, m._42, m._43);
}

void SetTransform(dg::float4x4& m, const dg::float3& value) {
    m._41 = value.x;
    m._42 = value.y;
    m._43 = value.z;
}

}