#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 vertexColor;
out vec2 texCoord;

uniform float rotationAngle;
uniform float zoomFactor;

uniform mat4 transform;

void main() {
	gl_Position = transform * vec4(aPos, 1.0);
	vertexColor = aColor;

	vec2 shiftedCoord = aTexCoord - vec2(0.5, 0.5);

	shiftedCoord *= zoomFactor;

	float s = sin(rotationAngle);
	float c = cos(rotationAngle);

	vec2 rotatedCoord;
	rotatedCoord.x = shiftedCoord.x * c + shiftedCoord.y * s;
	rotatedCoord.y = -shiftedCoord.x * s + shiftedCoord.y * c;

	texCoord = rotatedCoord + vec2(0.5, 0.5);
}