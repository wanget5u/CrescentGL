#include "Render/GPUDisposalQueue.h"
#include <glad/glad.h>

namespace Crescent::Render {

std::mutex GPUDisposalQueue::s_Mutex;
DynamicList<u32> GPUDisposalQueue::s_Textures{};
DynamicList<u32> GPUDisposalQueue::s_Buffers{};
DynamicList<u32> GPUDisposalQueue::s_VertexArrays{};
DynamicList<u32> GPUDisposalQueue::s_Programs{};
DynamicList<u32> GPUDisposalQueue::s_Shaders{};

void GPUDisposalQueue::SubmitTextureForDeletion(const u32 textureID) {
	if (textureID == 0) {
		return;
	}
	std::scoped_lock lock(s_Mutex);
	s_Textures.PushBack(textureID);
}

void GPUDisposalQueue::SubmitBufferForDeletion(const u32 bufferID) {
	if (bufferID == 0) {
		return;
	}
	std::scoped_lock lock(s_Mutex);
	s_Buffers.PushBack(bufferID);
}

void GPUDisposalQueue::SubmitVertexArrayForDeletion(const u32 vaoID) {
	if (vaoID == 0) {
		return;
	}
	std::scoped_lock lock(s_Mutex);
	s_VertexArrays.PushBack(vaoID);
}

void GPUDisposalQueue::SubmitProgramForDeletion(const u32 programID) {
	if (programID == 0) {
		return;
	}
	std::scoped_lock lock(s_Mutex);
	s_Programs.PushBack(programID);
}

void GPUDisposalQueue::SubmitShaderForDeletion(const u32 shaderID) {
	if (shaderID == 0) {
		return;
	}
	std::scoped_lock lock(s_Mutex);
	s_Shaders.PushBack(shaderID);
}

void GPUDisposalQueue::Flush() {
	std::scoped_lock lock(s_Mutex);
	if (s_Textures.IsEmpty() == false) {
		glDeleteTextures(static_cast<i32>(s_Textures.GetSize()), s_Textures.GetData());
		s_Textures.Clear();
	}
	if (s_Buffers.IsEmpty() == false) {
		glDeleteBuffers(static_cast<i32>(s_Buffers.GetSize()), s_Buffers.GetData());
		s_Buffers.Clear();
	}
	if (s_VertexArrays.IsEmpty() == false) {
		glDeleteVertexArrays(static_cast<i32>(s_VertexArrays.GetSize()), s_VertexArrays.GetData());
		s_VertexArrays.Clear();
	}
	for (size_t a = 0; a < s_Programs.GetSize(); ++a) {
		if (s_Programs[a] != 0) {
			glDeleteProgram(s_Programs[a]);
		}
	}
	s_Programs.Clear();
	for (size_t a = 0; a < s_Shaders.GetSize(); ++a) {
		if (s_Shaders[a] != 0) {
			glDeleteShader(s_Shaders[a]);
		}
	}
	s_Shaders.Clear();
}

void GPUDisposalQueue::Clear() {
	std::scoped_lock lock(s_Mutex);
	s_Textures.Clear();
	s_Buffers.Clear();
	s_VertexArrays.Clear();
	s_Programs.Clear();
	s_Shaders.Clear();
}

}
