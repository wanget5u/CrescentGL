#version 460 core
#include "Include/RenderData.glsl"

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoordinate;
layout (location = 3) in mat4 aInstanceModel;

uniform mat4 u_Model;
uniform bool u_IsInstanced;

out vec2 v_TextureCoordinate;
out vec3 v_Normal;
out vec3 v_FragPosition;

void main() {
    mat4 modelMatrix = u_IsInstanced ? aInstanceModel : u_Model;
    vec4 worldPosition = modelMatrix * vec4(aPosition, 1.0);
    v_TextureCoordinate = aTextureCoordinate;

    gl_Position = u_ViewProjection * worldPosition;
    v_FragPosition = vec3(worldPosition);
    v_Normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
}