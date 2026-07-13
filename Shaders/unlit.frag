#version 460 core
#include "Include/SceneRenderData.glsl"

out vec4 FragColor;

in vec2 v_TextureCoordinate;

uniform sampler2D tex_AlbedoMap;

void main() {
	FragColor = texture(tex_AlbedoMap, v_TextureCoordinate) * v4_TintColor;
}