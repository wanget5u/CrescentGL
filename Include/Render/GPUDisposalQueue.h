#pragma once
#include "Collection/DynamicList.h"
#include "Core/Core.h"

namespace Crescent {
struct GPUDisposalQueue {
	static void SubmitTextureForDeletion(u32 textureID);
	static void SubmitBufferForDeletion(u32 bufferID);
	static void SubmitVertexArrayForDeletion(u32 vaoID);
	static void SubmitProgramForDeletion(u32 programID);
	static void SubmitShaderForDeletion(u32 shaderID);
	static void Flush();
	static void Clear();
private:
	static DynamicList<u32> s_Textures;
	static DynamicList<u32> s_Buffers;
	static DynamicList<u32> s_VertexArrays;
	static DynamicList<u32> s_Programs;
	static DynamicList<u32> s_Shaders;
};
}
