#version 460 core
#include "Include/SceneData.glsl"

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoordinate;
layout (location = 3) in mat4 aInstanceModel;
layout (location = 7) in vec3 aTangent;

uniform mat4 u_Model;
uniform bool u_IsInstanced;

out vec2 v_TextureCoordinate;
out vec3 v_WorldPosition;
out vec4 v_InstanceColor;
out mat3 v_TBN;

mat4 getRotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(
    oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
    oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
    oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
    0.0,                                0.0,                                0.0,                                1.0
    );
}

void main() {
    v_TextureCoordinate = aTextureCoordinate;

    mat4 modelMatrix = u_IsInstanced ? aInstanceModel : u_Model;

    if (u_IsInstanced) {
        int id = gl_InstanceID;
        int x = (id / 128) % 128;
        int y = (id / 16) % 16;
        int z = id % 128;

        float yOffset = sin(u_Time * 2.0 + float(x) * 0.3) * 0.5;
        modelMatrix[3][1] += yOffset;

        float angle = radians(u_Time * 45.0) + float(x + y + z) * 0.2;
        mat4 rotMatrix = getRotationMatrix(vec3(0.5, 1.0, 0.2), angle);

        modelMatrix = modelMatrix * rotMatrix;
    }

    vec4 worldPosition = modelMatrix * vec4(aPosition, 1.0);
    v_WorldPosition = worldPosition.xyz;

    mat3 normalMatrix = mat3(modelMatrix);

    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);

    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    v_TBN = mat3(T, B, N);

    v_InstanceColor = vec4(1.0, 1.0, 1.0, 1.0);
    gl_Position = u_ViewProjection * worldPosition;
}