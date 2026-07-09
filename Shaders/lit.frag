#version 460 core
out vec4 FragColor;

in vec2 TextureCoordinate;
in vec3 WorldNormal;

uniform sampler2D u_AlbedoMap;
uniform vec4      u_TintColor;

uniform sampler2D u_MetallicMap;
uniform float     u_MetallicFactor;

uniform sampler2D u_RoughnessMap;
uniform float     u_RoughnessFactor;

uniform sampler2D u_NormalMap;

void main() {
    vec3  albedo        = texture(u_AlbedoMap,    TextureCoordinate).rgb * u_TintColor.rgb;
    float metallic      = texture(u_MetallicMap,  TextureCoordinate).r   * u_MetallicFactor;
    float roughness     = texture(u_RoughnessMap, TextureCoordinate).r   * u_RoughnessFactor;
    vec3  tangentNormal = texture(u_NormalMap,    TextureCoordinate).rgb * 2.0 - 1.0;

    FragColor = vec4(albedo, 1.0);
}