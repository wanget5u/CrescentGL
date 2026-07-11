#pragma once
#include "Panel.h"

namespace Crescent::UI {
struct DebugPanel : Panel {
	explicit DebugPanel(std::string_view name = "Debug Bar");
	void OnUpdate(f32 deltaTime) override;
	void OnRender() override;
	void OnAttach() override;
	void OnDetach() override;
private:
	constexpr static f32 MetricsUpdateRate = 0.1f;
	f32 m_Timer{MetricsUpdateRate};
	u32 m_FrameCount{0};
	f32 m_CachedFPS{0.0f};
	f32 m_CachedCpuTimeMs{0.0f};
	f32 m_AccumulatedCpuTimeMs{0.0f};
	f32 m_CachedGpuTimeMs{0.0f};
	f32 m_AccumulatedGpuTimeMs{0.0f};
	u32 m_CachedDrawCalls{0};
	u32 m_CachedTriangleCount{0};
	u32 m_CachedVertexCount{0};
};
}
