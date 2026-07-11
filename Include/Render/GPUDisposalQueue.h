#pragma once
#include <mutex>
#include "Collection/DynamicList.h"
#include "Core/Core.h"

namespace Crescent::Render {
/// Thread-safe queue for deferring OpenGL resource deallocation with `glDelete`
/// until the render thread executes it with an active OpenGL context
struct GPUDisposalQueue {
	static void SubmitTextureForDeletion(u32 textureID);
	static void SubmitBufferForDeletion(u32 bufferID);
	static void SubmitVertexArrayForDeletion(u32 vaoID);
	static void SubmitProgramForDeletion(u32 programID);
	static void SubmitShaderForDeletion(u32 shaderID);
	/// Flushes all queued deletions. Must be called on the thread with the active OpenGL context
	static void Flush();
	static void Clear();
private:
	static std::mutex s_Mutex;
	static DynamicList<u32> s_Textures;
	static DynamicList<u32> s_Buffers;
	static DynamicList<u32> s_VertexArrays;
	static DynamicList<u32> s_Programs;
	static DynamicList<u32> s_Shaders;
};
}
