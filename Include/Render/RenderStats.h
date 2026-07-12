#pragma once
#include "Core/Core.h"

namespace Crescent {
struct RenderStats {
	f32 CPUFrameTimeMs{0.0f};
	u32 DrawCalls{0};
	u32 TriangleCount{0};
	u32 VertexCount{0};
	f32 GPUFrameTimeMs{0.0f};
	static RenderStats& Instance() {
		static RenderStats s_Instance;
		return s_Instance;
	}
	void Reset() noexcept {
		DrawCalls = 0;
		TriangleCount = 0;
		VertexCount = 0;
	}
};
}