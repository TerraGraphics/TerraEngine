meta {
    name: "",
    group: "AMBIENT_DIFFUSE_LIGHT",
    parameters : []
}
pixel = <<SHADER
float3 LightAmbientDiffuse(in PSInput psIn, in float3 baseColor) {
    return baseColor;
}
SHADER
