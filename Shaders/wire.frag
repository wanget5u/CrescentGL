#version 460

in vec3 v_Barycentric;
out vec4 FragColor;

void main() {
    float f_thickness = 0.0001;
    float f_closestEdge = min(v_Barycentric.x, min(v_Barycentric.y, v_Barycentric.z));
    float f_width = fwidth(f_closestEdge);
    float f_alpha = 1.0 - smoothstep(f_thickness, f_thickness + f_width, f_closestEdge);
    if (f_alpha < 0.01) {
        discard;
    }
    FragColor = vec4(vec3(0.0), f_alpha);
}