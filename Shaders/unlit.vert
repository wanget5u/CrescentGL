#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoordinate;
layout (location = 3) in mat4 aInstanceModel;

uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_Model;
uniform bool u_IsInstanced;

out vec2 TextureCoordinate;
out vec3 Normal;
out vec4 InstanceColor;

void main() {
	TextureCoordinate = aTextureCoordinate;
	InstanceColor = vec4(1.0, 1.0, 1.0, 1.0);

	mat4 modelMatrix = u_IsInstanced ? aInstanceModel : u_Model;
	vec4 worldPosition = modelMatrix * vec4(aPosition, 1.0);
	Normal = mat3(modelMatrix) * aNormal;

	gl_Position = u_Projection * u_View * worldPosition;
}