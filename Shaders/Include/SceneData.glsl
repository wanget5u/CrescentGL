struct PointLightData {
	vec3 Position;
	float Range;
	vec3 Color;
	float Energy;
	float Constant;
	float Linear;
	float Quadratic;
	float Padding;
};

layout(std140, binding = 0) uniform SceneData {
	mat4 u_View;
	mat4 u_Projection;
	mat4 u_ViewProjection;
	vec3 u_CameraPosition;
	float u_Time;
	int u_PointLightCount;
	float u_Padding0;
	float u_Padding1;
	float u_Padding2;
	PointLightData u_PointLights[32];
};