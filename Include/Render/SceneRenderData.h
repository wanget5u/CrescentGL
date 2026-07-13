#pragma once
#include "Core/Core.h"
#include "Math/Matrix/Matrix4x4.h"
#include "Math/Vector/Vector3.h"
#include "Math/Vector/Vector4.h"

namespace Crescent::GPU {
struct alignas(16) SceneRenderData {
	Math::Matrix4x4 View{};				//   64B | 0
	Math::Matrix4x4 Projection{};		//   64B | 64
	Math::Matrix4x4 ViewProjection{};	//   64B | 128
	//------------------192-(12x16B)------------------
	Math::Vector3 CameraPosition{};		//   12B | 192
	f32 Time{};							//    4B | 204
	//------------------208-(13x16B)------------------
	u32 PointLightCount{};				//    4B | 208
	u32 DirectionalLightCount{};		//    4B | 212
	f32 Padding0{};						//    4B | 216
	f32 Padding1{};						//    4B | 220
	//------------------224-(14x16B)------------------
};
struct alignas(16) DirectionalLightRenderData {
	Math::Vector3 Direction{}; 					// 12B | 0
	f32 Energy{0.0f};				 			//  4B | 12
	//-----------------------16-(1x16B)--------------------
	Math::Vector3 Color{1.0f, 1.0f, 1.0f};		// 12B | 16
	u32 CascadeCount{0};		 				//  4B | 28
	//-----------------------32-(2x16B)--------------------
	Math::Matrix4x4 LightSpaceMatrices[4];		// 256B | 32
	//-----------------------272-(17x16B)------------------
	Math::Vector4 CascadeSplits{};				// 16B | 272
	//-----------------------288-(18x16B)------------------
};
struct alignas(16) PointLightRenderData {
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
struct alignas(16) InstanceRenderData {
	Math::Matrix4x4 WorldMatrix;  // 64B | 0
	Math::Matrix4x4 NormalMatrix; // 64B | 64
	//--------------128-(8x16B)--------------
	u32 MaterialIndex;			  // 4B | 128
	u32 ObjectID;				  // 4B | 132
	u32 Padding0;				  // 4B | 136
	u32 Padding1;				  // 4B | 140
	//--------------144-(9x16B)--------------
};
struct alignas(16) MaterialRenderData {
	Math::Vector4 TintColor{1.0f, 1.0f, 1.0f, 1.0f}; // 16B | 0
	f32 MetallicFactor{1.0f};						 //  4B | 16
	f32 RoughnessFactor{1.0f};						 //  4B | 20
	f32 AmbientOcclusionFactor{1.0f};				 //  4B | 24
	u32 PreserveUVCoordinates{0};					 //  4B | 28
	//------------------------32-(2x16B)------------------------
	Math::Vector2 UVScale{1.0f, 1.0f};				 //  8B | 32
	Math::Vector2 UVOffset{0.0f, 0.0f};				 //  8B | 40
	//------------------------48-(3x16B)------------------------
};
}
