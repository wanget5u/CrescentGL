#version 460 core
out vec4 FragColor;

in vec2 textureCoordinate;

uniform sampler2D u_AlbedoMap;
uniform vec4      u_TintColor;

void main() {
	FragColor = texture(u_AlbedoMap, textureCoordinate) * u_TintColor;
}