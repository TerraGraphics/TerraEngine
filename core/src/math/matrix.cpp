#include "core/math/matrix.h"


namespace math {

// decompose matrix see in https://math.stackexchange.com/a/1463487

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

dg::float3 GetRotate(dg::float4x4 m) {
    dg::float3 s = GetScale(m);

    m._11 /= s.x;
    m._12 /= s.x;
    m._13 /= s.x;
    m._21 /= s.y;
    m._22 /= s.y;
    m._23 /= s.y;
    m._31 /= s.z;
    m._32 /= s.z;
    m._33 /= s.z;

    // see https://robotics.stackexchange.com/questions/8516/getting-pitch-yaw-and-roll-from-rotation-matrix-in-dh-parameter
    float pitch = std::atan2(-m._31, std::hypot(m._32, m._33));
    float yaw = std::atan2(m._21, m._11);
    float roll = std::atan2(m._32, m._33);

    return dg::float3(yaw, pitch, roll);
}

void SetRotate(dg::float4x4& m, const dg::float3& value) {
    // alpha = z = roll
    // betta = y = pitch
    // gamma = x = yaw

    float cx = std::cos(value.x);
    float sx = std::sin(value.x);
    float cy = std::cos(value.y);
    float sy = std::sin(value.y);
    float cz = std::cos(value.z);
    float sz = std::sin(value.z);
    dg::float3 s = GetScale(m);

    m._11 = cz * cy * s.x;
    m._12 = sz * cy * s.x;
    m._13 = -sy * s.x;

    m._21 = (cz * sy * sx - sz * cx) * s.y;
    m._22 = (sz * sy * sx + cz * cx) * s.y;
    m._23 = cy * sx * s.y;

    m._31 = (cz * sy * cx + sz * sx) * s.z;
    m._32 = (sz * sy * cx - cz * sx) * s.z;
    m._33 = cy * cx * s.y;
}

}
