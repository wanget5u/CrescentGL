#pragma once
#include "imgui.h"

#include "Panel.h"
#include "Math/Vector/Vector3.h"

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
	/// Left Side
	f32 m_CachedFPS{0.0f};
	f32 m_CachedCpuTimeMs{0.0f};
	f32 m_AccumulatedCpuTimeMs{0.0f};
	f32 m_CachedGpuTimeMs{0.0f};
	f32 m_AccumulatedGpuTimeMs{0.0f};
	u32 m_CachedDrawCalls{0};
	u32 m_CachedTriangleCount{0};
	u32 m_CachedVertexCount{0};
	/// Right Side
	std::string m_CachedSceneName{};
	std::string m_SceneState{};
	f32 m_CachedCameraSpeed{0.0f};
	f32 m_CachedCameraZoom{0.0f};
	Math::Vector3 m_CachedCameraPosition{};

	struct SegmentLayoutContext {
		f32 ResolutionScale{1.0f};
		f32 SeparatorSpacing{12.0f};
		f32 SeparatorWidth{1.0f};
		f32 CurrentX{0.0f};
		f32 MaxX{0.0f};
		bool HasSpace{true};
	};

	struct RightSegmentItem {
		char Text[64];
		ImVec4 Color;
		f32 MinWidthUnscaled{0.0f};
		f32 ActualWidth{0.0f};
		f32 TextWidth{0.0f};
	};

	void RenderLeftSection(SegmentLayoutContext& context) const;
	void DrawLeftItem(SegmentLayoutContext& context, f32 minWidthUnscaled, bool isFirst, const ImVec4& color, const char* text) const;
	void RenderRightSection(const SegmentLayoutContext& context, f32 leftEndPosX, f32& outRightStartPosX) const;
	f32 CalculateRightGroupWidth(const RightSegmentItem* items, u8 itemCount, u8 startIdx, f32 separatorSpacing, f32 separatorWidth) const;
	void RenderCenterSection(const SegmentLayoutContext& context, f32 leftEndPosX, f32 rightStartPosX) const;
};
}
