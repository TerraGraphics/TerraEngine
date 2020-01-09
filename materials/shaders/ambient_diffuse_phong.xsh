meta {
    name: "AMBIENT_DIFFUSE_PHONG",
    group: "AMBIENT_DIFFUSE_LIGHT",
    parameters : []
}
pixel = <<SHADER
float3 LightAmbientDiffuse(in PSInput psIn, in float3 baseColor) {
	const float3 lightColor = float3(1, 1, 1);
    const float3 toLightDirection = float3(0, 1, 0);
    const float ambientIntensity = 0.7;

    float3 normal = normalize(psIn.normal);
    float cosNL = dot(normal, toLightDirection);

    float3 ambient = baseColor * ambientIntensity * lightColor;
    float3 diffuse = baseColor * cosNL * lightColor;

    return ambient + diffuse;
}
SHADER
