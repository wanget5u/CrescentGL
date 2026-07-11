#version 460 core
#include "Include/RenderData.glsl"

out vec4 FragColor;

uniform sampler2D u_AlbedoMap;
uniform vec4 u_TintColor;

uniform vec3 u_ObjectColor;
uniform vec3 u_LightColor;

uniform vec3 u_LightPosition;

in vec2 v_TextureCoordinate;
in vec3 v_Normal;
in vec3 v_FragPosition;

void main() {
//    FragColor = texture(u_AlbedoMap, v_TextureCoordinate) * u_TintColor;

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * u_LightColor;

    // diffuse
    vec3 norm = normalize(v_Normal);
    vec3 lightDirection = normalize(u_LightPosition - v_FragPosition);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * u_LightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDirection = normalize(u_CameraPosition - v_FragPosition);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    vec3 specular = specularStrength * spec * u_LightColor;

    vec3 result = (ambient + diffuse + specular) * u_ObjectColor;
    FragColor = vec4(result, 1.0);
}