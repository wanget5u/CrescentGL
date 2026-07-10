#version 460 core
#include "Include/SceneData.glsl"

uniform sampler2D u_AlbedoMap;
uniform vec4      u_TintColor;
uniform sampler2D u_MetallicMap;
uniform float     u_MetallicFactor;
uniform sampler2D u_RoughnessMap;
uniform float     u_RoughnessFactor;
uniform sampler2D u_NormalMap;
uniform float     u_AmbientOcclusionFactor;
uniform sampler2D u_AmbientOcclusion;

in vec2 v_TextureCoordinate;
in vec3 v_WorldPosition;
in vec4 v_InstanceColor;
in mat3 v_TBN;

out vec4 FragColor;

const float PI = 3.14159265359;

float DistributionGGX(vec3 n, vec3 h, float a2) {
    float nDotH = max(dot(n, h), 0.0);
    float denomInner = nDotH * nDotH * (a2 - 1.0) + 1.0;
    float denominator = max(PI * denomInner * denomInner, 0.00001);
    return a2 / denominator;
}

float GeometrySmithCorrelated(float nDotV, float nDotL, float a2) {
    float denominator = mix(2.0 * nDotL * nDotV, nDotL + nDotV, a2);
    if (denominator > 0.00001) {
        return 0.5 / denominator;
    }
    return 0.0;
}

vec3 FresnelSchlick(float hDotV, vec3 f0) {
    float inv = 1.0 - hDotV;
    float inv2 = inv * inv;
    float inv5 = inv2 * inv2 * inv;
    return f0 + (1.0 - f0) * inv5;
}

vec3 CookTorranceSpecular(vec3 n, vec3 v, vec3 l, float a2, vec3 f0, float nDotV, float nDotL, out vec3 fresnel) {
    vec3 halfVector = normalize(v + l);
    float distribution = DistributionGGX(n, halfVector, a2);
    float visibility = GeometrySmithCorrelated(nDotV, nDotL, a2);

    float hDotV = max(dot(halfVector, v), 0.0);
    fresnel = FresnelSchlick(hDotV, f0);

    return distribution * fresnel * visibility;
}

void main() {
    vec3  albedo    = texture(u_AlbedoMap,        v_TextureCoordinate).rgb * u_TintColor.rgb;
    float metallic  = texture(u_MetallicMap,      v_TextureCoordinate).r   * u_MetallicFactor;
    float roughness = texture(u_RoughnessMap,     v_TextureCoordinate).r   * u_RoughnessFactor;
    float ao        = texture(u_AmbientOcclusion, v_TextureCoordinate).r   * u_AmbientOcclusionFactor;

    float alpha = roughness * roughness;
    float a2    = alpha * alpha;

    vec3 tangentNormal = texture(u_NormalMap, v_TextureCoordinate).rgb * 2.0 - 1.0;
    vec3 normal = normalize(v_TBN * tangentNormal);

    vec3 viewDirection = normalize(u_CameraPosition - v_WorldPosition);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    float nDotV = max(dot(normal, viewDirection), 0.0001);
    vec3 Lo = vec3(0.0);

    for (int a = 0; a < u_PointLightCount; ++a) {
        PointLightData light = u_PointLights[a];

        vec3 lightDirection = light.Position - v_WorldPosition;
        float distanceSquared = dot(lightDirection, lightDirection);
        float rangeSquared = light.Range * light.Range;

        if (distanceSquared >= rangeSquared) {
            continue;
        }

        float distance = sqrt(distanceSquared);
        vec3 L = lightDirection / distance;

        float nDotL = max(dot(normal, L), 0.0);
        if (nDotL <= 0.0001) {
            continue;
        }

        float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * distanceSquared);

        float distOverRange = distanceSquared / rangeSquared;
        float window = clamp(1.0 - (distOverRange * distOverRange), 0.0, 1.0);
        attenuation *= window * window;

        vec3 radiance = light.Color * light.Energy * attenuation;
        vec3 F;
        vec3 specular = CookTorranceSpecular(normal, viewDirection, L, a2, F0, nDotV, nDotL, F);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        Lo += (kD * albedo / PI + specular) * radiance * nDotL;
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(0.454545));

    FragColor = vec4(color, 1.0);
}