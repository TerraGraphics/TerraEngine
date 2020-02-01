#ifndef _MATH_FXH_
#define _MATH_FXH_

// pi = 180 deg
static const float PI = 3.14159265;
// pi / 2 = 90 deg
static const float HalfPI = 1.57079632;
// pi / 3 = 60 deg
static const float ThirdPI = 1.04719755;
// pi / 4 = 45 deg
static const float QuarterPI = 0.78539816;

// https://stackoverflow.com/questions/5149544/can-i-generate-a-random-number-inside-a-pixel-shader
static const float2 randFactor = float2(
        23.14069263277926, // e^pi
         2.665144142690225 // 2^sqrt(2)
);

// any float
float randomFast(in float2 vecIn) {
    return dot(vecIn, randFactor);
}

// [0.0; 1.0)
float random01(in float2 vecIn) {
    return frac(cos(dot(vecIn, randFactor)) * 12345.6789);
}

float4x4 RotationX(in float angleInRadians) {
    float s, c;
    sincos(angleInRadians, s, c);
    return MatrixFromRows(
        float4( 1,  0,  0,  0),
        float4( 0,  c, -s,  0),
        float4( 0,  s,  c,  0),
        float4( 0,  0,  0,  1));
}

float4x4 RotationY(in float angleInRadians) {
    float s, c;
    sincos(angleInRadians, s, c);
    return MatrixFromRows(
        float4( c,  0,  s,  0),
        float4( 0,  1,  0,  0),
        float4(-s,  0,  c,  0),
        float4( 0,  0,  0,  1));
}

float4x4 RotationZ(in float angleInRadians) {
    float s, c;
    sincos(angleInRadians, s, c);
    return MatrixFromRows(
        float4( c, -s,  0,  0),
        float4( s,  c,  0,  0),
        float4( 0,  0,  1,  0),
        float4( 0,  0,  0,  1));
}

float4x4 Translation(in float3 v) {
    return MatrixFromRows(
         float4(1,  0,  0,  v.x),
         float4(0,  1,  0,  v.y),
         float4(0,  0,  1,  v.z),
         float4(0,  0,  0,    1));
}

float4x4 Scale(in float3 v) {
    return MatrixFromRows(
         float4(v.x,   0,   0,  0),
         float4(  0, v.y,   0,  0),
         float4(  0,   0, v.z,  0),
         float4(  0,   0,   0,  1));
}

#endif //_MATH_FXH_
