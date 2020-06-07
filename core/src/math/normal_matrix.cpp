#include "core/math/normal_matrix.h"

template<typename T>
static T MakeNormalMatrix(const dg::float4x4& m) {
    float inverseDet = 1.f / (
        + m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
        - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
        + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));

    T r;
    r[0][0] = + (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * inverseDet;
    r[0][1] = - (m[1][0] * m[2][2] - m[2][0] * m[1][2]) * inverseDet;
    r[0][2] = + (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * inverseDet;
    r[1][0] = - (m[0][1] * m[2][2] - m[2][1] * m[0][2]) * inverseDet;
    r[1][1] = + (m[0][0] * m[2][2] - m[2][0] * m[0][2]) * inverseDet;
    r[1][2] = - (m[0][0] * m[2][1] - m[2][0] * m[0][1]) * inverseDet;
    r[2][0] = + (m[0][1] * m[1][2] - m[1][1] * m[0][2]) * inverseDet;
    r[2][1] = - (m[0][0] * m[1][2] - m[1][0] * m[0][2]) * inverseDet;
    r[2][2] = + (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * inverseDet;

    return r;
}

dg::float4x4 MakeNormalMatrix4x4(const dg::float4x4& m) {
    return MakeNormalMatrix<dg::float4x4>(m);
}

dg::float3x3 MakeNormalMatrix3x3(const dg::float4x4& m) {
    return MakeNormalMatrix<dg::float3x3>(m);
}
