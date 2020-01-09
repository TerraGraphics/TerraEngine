meta {
    name: "ALPHA_TEST",
    group: "ALPHA",
    parameters : []
}
pixel = <<SHADER
void AlphaTest(in float alpha) {
    clip(alpha - 0.2);
}
SHADER
