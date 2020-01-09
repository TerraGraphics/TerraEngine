meta {
    name: "",
    group: "",
    parameters : []
}
pixel = <<SHADER
struct PSInput {
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEX_COORD;
};

void main(in PSInput psIn, out PSOutput psOut) : SV_TARGET {
    float4 baseColor = GetBaseColor(psIn);
    AlphaTest(baseColor.a);
    float3 abientDiffuse = LightAmbientDiffuse(psIn, baseColor.rgb);
    float3 specular = LightSpecular(psIn, baseColor.rgb);

    return float4(abientDiffuse + specular, 1.0);
}
SHADER
vertex = <<SHADER
cbuffer Camera {
    ShaderCamera camera;
};

struct VSInput {
    float3 position : ATTRIB0;
    float3 normal   : ATTRIB1;
    float2 uv       : ATTRIB2;

    // Instance attributes
    float4 WorldRow0 : ATTRIB3;
    float4 WorldRow1 : ATTRIB4;
    float4 WorldRow2 : ATTRIB5;
    float4 WorldRow3 : ATTRIB6;
    float4 NormalRow0 : ATTRIB7;
    float4 NormalRow1 : ATTRIB8;
    float4 NormalRow2 : ATTRIB9;
    float4 NormalRow3 : ATTRIB10;
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
    // HLSL matrices are row-major while GLSL matrices are column-major. We will
    // use convenience function MatrixFromRows() appropriately defined by the engine
    float4x4 matWorld = MatrixFromRows(vsIn.WorldRow0, vsIn.WorldRow1, vsIn.WorldRow2, vsIn.WorldRow3);
    float4x4 matNormal = MatrixFromRows(vsIn.NormalRow0, vsIn.NormalRow1, vsIn.NormalRow2, vsIn.NormalRow3);

    psIn.position = mul(camera.matViewProj, mul(float4(vsIn.position,1.0), matWorld));
    psIn.normal = mul(vsIn.normal, float3x3(matNormal));
    psIn.uv = vsIn.uv;
}
SHADER
