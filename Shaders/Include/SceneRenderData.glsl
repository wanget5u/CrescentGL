struct PointLightRenderData {
	vec3  v3_Position;
	float f32_Range;
	vec3  v3_Color;
	float f32_Energy;
	float f32_Constant;
	float f32_Linear;
	float f32_Quadratic;
	float f32_Padding0;
};

struct DirectionalLightRenderData {
	vec3  v3_Direction;
	float f32_Energy;
	vec3  v3_Color;
	uint  u32_CascadeCount;
	mat4  m4_LightSpaceMatrices[4];
	vec4  v4_CascadeSplits;
};

struct InstanceRenderData {
	mat4 m4_WorldMatrix;
	mat4 m4_NormalMatrix;
	uint u32_MaterialIndex;
	uint u32_ObjectID;
	uint u32_Padding0;
	uint u32_Padding1;
};
// UBO
layout(std140, binding = 0) uniform SceneRenderData {
	mat4  m4_View;
	mat4  m4_Projection;
	mat4  m4_ViewProjection;
	vec3  v3_CameraPosition;
	float f32_Time;
	uint  u32_PointLightCount;
	float f32_Padding0;
	float f32_Padding1;
	float f32_Padding2;
};
// SSBO
layout(std430, binding = 1) readonly buffer PointLightBuffer {
	PointLightRenderData u_PointLights[];
};
// SSBO
layout(std430, binding = 2) readonly buffer DirectionalLightBuffer {
	DirectionalLightRenderData u_DirectionalLights[];
};
// SSBO
layout(std430, binding = 3) readonly buffer InstanceBuffer {
	InstanceRenderData u_Instances[];
};
// UBO
layout(std140, binding = 4) uniform MaterialRenderData {
	vec4  v4_TintColor;
	float f32_MetallicFactor;
	float f32_RoughnessFactor;
	float f32_AmbientOcclusionFactor;
	uint  u32_PreserveUVCoordinates;
	vec2  v2_UVScale;
	vec2  v2_UVOffset;
};