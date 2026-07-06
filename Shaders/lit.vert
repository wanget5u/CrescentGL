#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 2) in vec2 aTextureCoordinate;

out vec2 textureCoordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
    textureCoordinate = aTextureCoordinate;
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    worldNormal = normalize(normalMatrix * aNormal);
}