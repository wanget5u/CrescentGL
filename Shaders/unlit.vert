#version 460 core

uniform mat4 view;
uniform mat4 projection;

out vec2 TextureCoordinate;
out vec3 Normal;
out vec4 InstanceColor;

struct InstanceData {
	mat4 model;
	vec4 color;
};

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoordinate;
layout (location = 3) in mat4 aInstanceModel;
layout (std140, binding = 0) readonly buffer InstanceBuffer {
	InstanceData instances[];
};

void main() {
	TextureCoordinate = aTextureCoordinate;

	mat4 modelMatrix = instances[gl_InstanceID].model;
	InstanceColor = instances[gl_InstanceID].color;

	vec4 worldPosition = modelMatrix * vec4(aPosition, 1.0);
	Normal = mat3(transpose(inverse(modelMatrix))) * aNormal;

	gl_Position = projection * view * worldPosition;
}