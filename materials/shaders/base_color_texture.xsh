meta {
    name: "BASE_COLOR_TEXTURE",
    group: "BASE_COLOR_VALUE",
    parameters : [
        textures2D: ["texBase"]
    ]
}
pixel = <<SHADER
float4 GetBaseColor(in PSInput psIn) {
    return texBase.Sample(texBaseSampler, psIn.uv);
}
SHADER
