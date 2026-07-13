#version 460 core
#include "Include/SceneRenderData.glsl"

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoordinate;

uniform mat4 m4_Model;
uniform bool b_IsInstanced;

out vec2 v_TextureCoordinate;
out vec3 v_Normal;
out vec3 v_FragPosition;

void main() {
    mat4 modelMatrix = b_IsInstanced ? u_Instances[gl_InstanceID].m4_WorldMatrix : m4_Model;
    mat3 normalMatrix = b_IsInstanced ? mat3(u_Instances[gl_InstanceID].m4_NormalMatrix) : mat3(transpose(inverse(m4_Model)));

    vec4 worldPosition = modelMatrix * vec4(aPosition, 1.0);
    v_TextureCoordinate = aTextureCoordinate;

    gl_Position = m4_ViewProjection * worldPosition;
    v_FragPosition = vec3(worldPosition);
    v_Normal = normalMatrix * aNormal;
}