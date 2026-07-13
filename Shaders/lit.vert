#version 460 core
#include "Include/SceneRenderData.glsl"

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoordinate;
layout (location = 3) in vec3 aTangent;

uniform mat4 m4_Model;
uniform bool b_IsInstanced;

out vec2 v2_TextureCoordinate;
out vec3 v3_WorldPosition;
out vec3 v3_Normal;
out mat3 m3_TBN;
out flat uint u32_FragMaterialIndex;

void main() {
    mat4 modelMatrix;
    mat3 normalMatrix;

    if (b_IsInstanced == true) {
        InstanceRenderData instance = u_Instances[gl_InstanceID];
        modelMatrix = instance.m4_WorldMatrix;
        normalMatrix = mat3(instance.m4_NormalMatrix);
        u32_FragMaterialIndex = instance.u32_MaterialIndex;
    }
    else {
        modelMatrix = m4_Model;
        normalMatrix = mat3(transpose(inverse(modelMatrix)));
        u32_FragMaterialIndex = 0;
    }

    vec2 finalUV = aTextureCoordinate;
    if (u32_PreserveUVCoordinates != 0) {
        vec3 objectScale = vec3(
            length(modelMatrix[0].xyz),
            length(modelMatrix[1].xyz),
            length(modelMatrix[2].xyz)
        );
        vec3 absN = abs(aNormal);
        vec2 scaleUV;
        if (absN.x > absN.y && absN.x > absN.z) {
            scaleUV = vec2(objectScale.z, objectScale.y);
        } else if (absN.y >= absN.x && absN.y >= absN.z) {
            scaleUV = vec2(objectScale.x, objectScale.z);
        } else {
            scaleUV = vec2(objectScale.x, objectScale.y);
        }
        finalUV *= scaleUV;
    }
    v2_TextureCoordinate = finalUV * v2_UVScale + v2_UVOffset;

    vec4 worldPosition = modelMatrix * vec4(aPosition, 1.0);
    v3_WorldPosition = vec3(worldPosition);

    vec3 N = normalize(normalMatrix * aNormal);
    v3_Normal = N;

    if (length(aTangent) > 0.001) {
        vec3 T = normalize(normalMatrix * aTangent);
        T = normalize(T - dot(T, N) * N);
        vec3 B = cross(N, T);
        m3_TBN = mat3(T, B, N);
    } else {
        vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
        vec3 T = normalize(cross(up, N));
        vec3 B = cross(N, T);
        m3_TBN = mat3(T, B, N);
    }

    gl_Position = m4_ViewProjection * worldPosition;
}