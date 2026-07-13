#version 460
#include "Include/SceneRenderData.glsl"

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoordinate;

uniform mat4 m4_Model;
uniform bool b_IsInstanced;

out vec3 v_Barycentric;

void main() {
    mat4 modelMatrix = b_IsInstanced ? u_Instances[gl_InstanceID].m4_WorldMatrix : m4_Model;
    vec4 worldPosition = modelMatrix * vec4(aPosition, 1.0);
    gl_Position = m4_ViewProjection * worldPosition;
}