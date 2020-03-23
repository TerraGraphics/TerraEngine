#include "core/math/matrix.h"


namespace math {

dg::float3 GetTranslation(const dg::float4x4& m) {
    return dg::float3(m._41, m._42, m._43);
}

void SetTranslation(dg::float4x4& m, const dg::float3& value) {
    m._41 = value.x;
    m._42 = value.y;
    m._43 = value.z;
}

dg::float3 GetScale(const dg::float4x4& m) {
    return dg::float3(
        dg::length(dg::float3(m._11, m._12, m._13)),
        dg::length(dg::float3(m._21, m._22, m._23)),
        dg::length(dg::float3(m._31, m._32, m._33))
    );
}

void SetScale(dg::float4x4& m, const dg::float3& value) {
    dg::float3 s = value / GetScale(m);
    m._11 *= s.x;
    m._12 *= s.x;
    m._13 *= s.x;
    m._21 *= s.y;
    m._22 *= s.y;
    m._23 *= s.y;
    m._31 *= s.z;
    m._32 *= s.z;
    m._33 *= s.z;
}

}
