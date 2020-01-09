meta {
    name: "SPECULAR_PHONG",
    group: "SPECULAR_LIGHT",
    parameters : [
        cbuffers: ["camera"]
    ]
}
pixel = <<SHADER
float3 LightSpecular(in PSInput psIn, in float4 baseColor) {
    const float3 lightColor = float3(1, 1, 1);
    const float3 toLightDirection = float3(0, 1, 0);
    // shininess > 0
	const float shininess = 100.0;

    float3 normal = normalize(psIn.normal); // duplcate in LightAmbientDiffuse
    float cosNL = dot(normal, toLightDirection); // duplicate in LightAmbientDiffuse
    float3 reflection = 2.0f * normal * cosNL - toLightDirection;
    float3 toEyeDirection = -camera.vecViewDirection.xyz;
    float cosRV = max(0, dot(reflection, toEyeDirection));
    float powSpecular = pow(cosRV, shininess);

    return powSpecular * baseColor;
}
SHADER
