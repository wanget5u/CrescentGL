#version 460 core
out vec4 FragColor;

uniform vec4 vertexColor;

void main() {
	FragColor = vertexColor;
}