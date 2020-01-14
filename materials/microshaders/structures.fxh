#ifndef _STRUCTURES_FXH_
#define _STRUCTURES_FXH_


#ifdef __cplusplus
#   ifndef BOOL
#      define BOOL int32_t // Do not use bool, because sizeof(bool)==1 !
#   endif

#   ifndef CHECK_STRUCT_ALIGNMENT
        // Note that defining empty macros causes GL shader compilation error on Mac, because
        // it does not allow standalone semicolons outside of main.
        // On the other hand, adding semicolon at the end of the macro definition causes gcc error.
#       define CHECK_STRUCT_ALIGNMENT(s) static_assert( sizeof(s) % 16 == 0, "sizeof(" #s ") is not multiple of 16" )
#   endif

#   ifndef DEFAULT_VALUE
#       define DEFAULT_VALUE(x) =x
#   endif

#else

#   ifndef BOOL
#       define BOOL bool
#   endif

#   ifndef DEFAULT_VALUE
#       define DEFAULT_VALUE(x)
#   endif
#   ifndef INIT_STRUCT
#       define INIT_STRUCT(x)
#   endif

// https://stackoverflow.com/questions/5149544/can-i-generate-a-random-number-inside-a-pixel-shader
static const float2 randFactor = float2(
        23.14069263277926, // e^pi
         2.665144142690225 // 2^sqrt(2)
);

float random0(float2 vecIn) {
    return dot(vecIn, randFactor);
}

float random(float2 vecIn) {
    return frac(cos(dot(vecIn, randFactor)) * 12345.6789);
}

#endif


struct ShaderCamera {
    float4x4 matViewProj;
    float4 vecPosition;
    float4 vecViewDirection;
};
#ifdef CHECK_STRUCT_ALIGNMENT
    CHECK_STRUCT_ALIGNMENT(ShaderCamera);
#endif

struct ShaderTransform {
    float4x4 matWorld;
    float4x4 matNormal;
};
#ifdef CHECK_STRUCT_ALIGNMENT
    CHECK_STRUCT_ALIGNMENT(ShaderTransform);
#endif

struct ShaderMaterial {
    float4 crlBase DEFAULT_VALUE(float4(0, 0, 0, 1));
};
#ifdef CHECK_STRUCT_ALIGNMENT
    CHECK_STRUCT_ALIGNMENT(ShaderMaterial);
#endif

#endif //_STRUCTURES_FXH_
