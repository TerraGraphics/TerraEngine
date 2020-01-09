meta {
    name: "",
    group: "SPECULAR_LIGHT",
    parameters : []
}
pixel = <<SHADER
float3 LightSpecular(in PSInput psIn, in float4 baseColor) {
    return float3(0);
}
SHADER
