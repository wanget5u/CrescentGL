#version 460 core
#include "Include/SceneRenderData.glsl"

uniform sampler2D tex_AlbedoMap;
uniform sampler2D tex_MetallicMap;
uniform sampler2D tex_RoughnessMap;
uniform sampler2D tex_NormalMap;
uniform sampler2D tex_AmbientOcclusion;

in vec2 v2_TextureCoordinate;
in vec3 v3_WorldPosition;
in vec3 v3_Normal;
in mat3 m3_TBN;
in flat uint u32_FragMaterialIndex;

out vec4 FragColor;

const float PI = 3.14159265359;

// Normal Distribution Function (Trowbridge-Reitz GGX)
float DistributionGGX(vec3 n, vec3 h, float a2) {
    float nDotH = max(dot(n, h), 0.0);
    float denomInner = nDotH * nDotH * (a2 - 1.0) + 1.0;
    float denominator = PI * denomInner * denomInner;
    return a2 / max(denominator, 0.00001);
}

// Geometric Shadowing/Masking (Smith-Schlick GGX Correlated)
float GeometrySmithCorrelated(float nDotV, float nDotL, float a2) {
    float denominator = mix(2.0 * nDotL * nDotV, nDotL + nDotV, a2);
    return 0.5 / max(denominator, 0.00001);
}

// Fresnel Reflectance (Schlick Approximation)
vec3 FresnelSchlick(float hDotV, vec3 f0) {
    float inv = 1.0 - hDotV;
    float inv2 = inv * inv;
    float inv5 = inv2 * inv2 * inv;
    return f0 + (1.0 - f0) * inv5;
}

// Evaluates the complete Cook-Torrance specular BRDF term
vec3 EvaluateCookTorranceSpecular(vec3 n, vec3 v, vec3 l, float a2, vec3 f0, float nDotV, float nDotL, out vec3 fresnel) {
    vec3 halfVector = normalize(v + l + vec3(0.00001));
    float distribution = DistributionGGX(n, halfVector, a2);
    float visibility = GeometrySmithCorrelated(nDotV, nDotL, a2);
    float hDotV = max(dot(halfVector, v), 0.0);
    fresnel = FresnelSchlick(hDotV, f0);
    return distribution * fresnel * visibility;
}

/// Resolves world-space surface normal from either tangent-space normal map or vertex normal
vec3 ResolveSurfaceNormal() {
    vec3 tangentNormal = texture(tex_NormalMap, v2_TextureCoordinate).rgb * 2.0 - 1.0;
    
    // Check if normal map actually contains valid perturbation data

    bool hasValidTangentNormal = length(tangentNormal) > 0.1 && (abs(tangentNormal.x) > 0.001 || abs(tangentNormal.y) > 0.001 || abs(tangentNormal.z - 1.0) > 0.001);
                                 
    if (hasValidTangentNormal == true) {
        return normalize(m3_TBN * tangentNormal);
    }
    return normalize(v3_Normal);
}

// Computes physically accurate point light attenuation (Inverse Square + Smooth Cutoff Window)
bool CalculatePointLightRadiance(PointLightRenderData light, vec3 worldPos, out vec3 L, out float nDotL, vec3 normal, out vec3 radiance) {
    vec3 lightDirection = light.v3_Position - worldPos;
    float distanceSquared = dot(lightDirection, lightDirection);
    float rangeSquared = max(light.f32_Range * light.f32_Range, 0.00001);

    if (distanceSquared >= rangeSquared) {
        return false;
    }

    float distance = sqrt(distanceSquared);
    L = lightDirection / max(distance, 0.0001);
    nDotL = max(dot(normal, L), 0.0);

    if (nDotL <= 0.0) {
        return false;
    }

    // Quadratic attenuation curve
    float attenuationDenom = light.f32_Constant + light.f32_Linear * distance + light.f32_Quadratic * distanceSquared;
    float attenuation = 1.0 / max(attenuationDenom, 0.00001);

    // Smooth windowing to reach exactly 0 at max range
    float distOverRange = distanceSquared / rangeSquared;
    float window = clamp(1.0 - (distOverRange * distOverRange), 0.0, 1.0);
    attenuation *= window * window;

    radiance = light.v3_Color * light.f32_Energy * attenuation;
    return true;
}

void main() {
    vec3  albedo    =     texture(tex_AlbedoMap,        v2_TextureCoordinate).rgb * v4_TintColor.rgb;
    float metallic  =     texture(tex_MetallicMap,      v2_TextureCoordinate).r   * f32_MetallicFactor;
    float roughness = max(texture(tex_RoughnessMap,     v2_TextureCoordinate).r   * f32_RoughnessFactor, 0.045);
    float ao        =     texture(tex_AmbientOcclusion, v2_TextureCoordinate).r   * f32_AmbientOcclusionFactor;

    // Precompute roughness powers for GGX distribution
    float alpha = roughness * roughness;
    float a2    = alpha * alpha;

    // Resolve surface vectors & base reflectivity (F0)
    vec3 normal = ResolveSurfaceNormal();
    vec3 viewDirection = normalize(v3_CameraPosition - v3_WorldPosition);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    float nDotV = max(dot(normal, viewDirection), 0.0001);

    // Accumulate direct lighting from all point lights in the scene
    vec3 Lo = vec3(0.0);

    for (uint a = 0; a < u32_PointLightCount; ++a) {
        PointLightRenderData light = u_PointLights[a];
        vec3 L;
        float nDotL;
        vec3 radiance;

        if (CalculatePointLightRadiance(light, v3_WorldPosition, L, nDotL, normal, radiance) == false) {
            continue;
        }

        // Cook-Torrance Specular & Diffuse
        vec3 F;
        vec3 specular = EvaluateCookTorranceSpecular(normal, viewDirection, L, a2, F0, nDotV, nDotL, F);

        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

        Lo += (kD * albedo / PI + specular) * radiance * nDotL;
    }

    // Ambient term with direct lighting (Lo)
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
    color = max(color, vec3(0.0));

    // Reinhard & Gamma Correction
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(0.454545));

    FragColor = vec4(color, 1.0);
}