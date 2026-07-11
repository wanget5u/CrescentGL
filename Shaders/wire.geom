#version 460

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 v_Barycentric;

void main() {
    gl_Position = gl_in[0].gl_Position;
    v_Barycentric = vec3(1.0, 0.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    v_Barycentric = vec3(0.0, 1.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    v_Barycentric = vec3(0.0, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}