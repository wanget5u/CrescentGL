#version 460

in vec3 v_Barycentric;
out vec4 FragColor;

void main() {
    vec3 dBary = fwidth(v_Barycentric);
    vec3 pixelDist = v_Barycentric / max(dBary, vec3(0.00001));
    float minEdgeDistPixels = min(pixelDist.x, min(pixelDist.y, pixelDist.z));

    float lineThicknessPixels = 1.0;
    float f_alpha = 1.0 - smoothstep(lineThicknessPixels - 0.5, lineThicknessPixels + 0.5, minEdgeDistPixels);

    if (f_alpha < 0.01) {
        discard;
    }

    FragColor = vec4(vec3(0.0), f_alpha);
}