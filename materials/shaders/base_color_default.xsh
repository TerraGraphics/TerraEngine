meta {
    name: "",
    group: "BASE_COLOR_VALUE",
    parameters : [
        cbuffers: ["material"]
    ]
}
pixel = <<SHADER
float4 GetBaseColor(in PSInput psIn) {
    return material.crlBase;
}
SHADER
