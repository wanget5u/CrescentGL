#version 460 core
#include "Include/SceneRenderData.glsl"

out vec4 FragColor;

uniform sampler2D tex_AlbedoMap;

uniform vec3 v3_ObjectColor;
uniform vec3 v3_LightColor;
uniform vec3 v3_LightPosition;

in vec2 v_TextureCoordinate;
in vec3 v_Normal;
in vec3 v_FragPosition;

void main() {
//    FragColor = texture(u_AlbedoMap, v_TextureCoordinate) * v4_TintColor;

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * v3_LightColor;

    // diffuse
    vec3 norm = normalize(v_Normal);
    vec3 lightDirection = normalize(v3_LightPosition - v_FragPosition);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * v3_LightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDirection = normalize(v3_CameraPosition - v_FragPosition);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    vec3 specular = specularStrength * spec * v3_LightColor;

    vec3 result = (ambient + diffuse + specular) * v3_ObjectColor;
    FragColor = vec4(result, 1.0);
}