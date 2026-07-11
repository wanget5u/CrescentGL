#pragma once
#include "Core/Core.h"
#include "Math/Matrix/Matrix4x4.h"
#include "Math/Vector/Vector3.h"

namespace Crescent::Render::GPU {
// GL_UNIFORM_BUFFER (std140) UBO
struct alignas(16) PointLightData {
	Math::Vector3 Position; // 12B | 0
	f32 Range;				//  4B | 12
	//------------16-(1x16B)------------
	Math::Vector3 Color;	// 12B | 16
	f32 Energy;				//  4B | 28
	//------------32-(2x16B)------------
	f32 Constant;			//  4B | 32
	f32 Linear;				//  4B | 36
	f32 Quadratic;			//  4B | 40
	f32 Padding;			//  4B | 44
	//------------48-(3x16B)-----------
};
// GL_UNIFORM_BUFFER (std140) UBO
struct alignas(16) RenderData {
	Math::Matrix4x4 View{};				//   64B | 0
	Math::Matrix4x4 Projection{};		//   64B | 64
	Math::Matrix4x4 ViewProjection{};	//   64B | 128
	//------------------192-(12x16B)------------------
	Math::Vector3 CameraPosition{};		//   12B | 192
	f32 Time{};							//    4B | 204
	//------------------208-(13x16B)------------------
	i32 PointLightCount{};				//    4B | 208
	f32 Padding0{};						//    4B | 212
	f32 Padding1{};						//    4B | 216
	f32 Padding2{};						//    4B | 220
	//------------------224-(14x16B)------------------
	PointLightData PointLights[32]{}; // 1536B | 224
	//-----------------1760-(110x16B)-----------------
}; // TODO: SSBO
}
