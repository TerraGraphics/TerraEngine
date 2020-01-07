#include "structures.fxh"

cbuffer Camera {
    ShaderCamera camera;
};

cbuffer Transform {
    ShaderTransform transform;
};

struct VSInput {
    float3 position : ATTRIB0;
    float3 normal   : ATTRIB1;
    float2 uv       : ATTRIB2;
};

struct PSInput {
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEX_COORD;
};

// Note that if separate shader objects are not supported (this is only the case for old GLES3.0 devices), vertex
// shader output variable name must match exactly the name of the pixel shader input variable.
// If the variable has structure type (like in this example), the structure declarations must also be indentical.
void main(in  VSInput vsIn, out PSInput psIn) {
    psIn.position = mul(mul(camera.matViewProj, transform.matWorld), float4(vsIn.position,1.0));
    psIn.normal = mul((float3x3)transform.matNormal, vsIn.normal);
    psIn.uv = vsIn.uv;
}
