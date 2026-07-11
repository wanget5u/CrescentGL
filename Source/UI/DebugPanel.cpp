#include "UI/DebugPanel.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "Core/Time.h"
#include "Render/RenderStats.h"

namespace Crescent::UI {

DebugPanel::DebugPanel(const std::string_view name) : Panel(name) {}

void DebugPanel::OnUpdate(const f32 deltaTime) {
	m_Timer += deltaTime;
	m_FrameCount++;
	Render::Stats& stats = Render::Stats::Instance();
	m_AccumulatedCpuTimeMs += stats.CPUFrameTimeMs;
	m_AccumulatedGpuTimeMs += stats.GPUFrameTimeMs;
	if (m_Timer >= MetricsUpdateRate) {
		m_CachedFPS = static_cast<f32>(m_FrameCount) / m_Timer;
		m_CachedCpuTimeMs = m_AccumulatedCpuTimeMs / static_cast<f32>(m_FrameCount);
		m_CachedGpuTimeMs = m_AccumulatedGpuTimeMs / static_cast<f32>(m_FrameCount);
		m_CachedDrawCalls = stats.DrawCalls;
		m_CachedTriangleCount = stats.TriangleCount;
		m_CachedVertexCount = stats.VertexCount;
		m_AccumulatedCpuTimeMs = 0.0f;
		m_AccumulatedGpuTimeMs = 0.0f;
		m_Timer = 0.0f;
		m_FrameCount = 0;
	}
}

void DebugPanel::OnRender() {
	if (m_IsVisible == false) {
		return;
	}
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	const f32 resolutionScale = GetResolutionScale();
	const f32 fontHeight = ImGui::GetTextLineHeight() * resolutionScale;
	const f32 verticalPadding = 4.0f * resolutionScale;
	const f32 dynamicPanelHeight = fontHeight + (verticalPadding * 2.0f);
	ImVec2 windowPos = ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y - dynamicPanelHeight);
	ImVec2 windowSize = ImVec2(viewport->WorkSize.x, dynamicPanelHeight);

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar |
	                               ImGuiWindowFlags_NoResize |
	                               ImGuiWindowFlags_NoMove |
	                               ImGuiWindowFlags_NoScrollbar |
	                               ImGuiWindowFlags_NoSavedSettings |
	                               ImGuiWindowFlags_NoDocking |
	                               ImGuiWindowFlags_NoFocusOnAppearing;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f * resolutionScale, verticalPadding));

	if (ImGui::Begin(m_Name.c_str(), &m_IsVisible, windowFlags) == true) {
		BeginScaledFont();
		char8 fpsBuf[32], cpuBuf[32], gpuBuf[32], drawBuf[32], trisBuf[32], vertBuf[32];
		snprintf(fpsBuf, sizeof(fpsBuf), "FPS: %.1f", m_CachedFPS);
		snprintf(cpuBuf, sizeof(cpuBuf), "CPU: %.2f ms", m_CachedCpuTimeMs);
		snprintf(gpuBuf, sizeof(gpuBuf), "GPU: %.2f ms", m_CachedGpuTimeMs);
		snprintf(drawBuf, sizeof(drawBuf), "Draw: %u", m_CachedDrawCalls);
		snprintf(trisBuf, sizeof(trisBuf), "Tris: %u", m_CachedTriangleCount);
		snprintf(vertBuf, sizeof(vertBuf), "Vert: %u", m_CachedVertexCount);
		const char* sections[] = { fpsBuf, cpuBuf, gpuBuf, drawBuf, trisBuf, vertBuf };
		constexpr i32 sectionCount = 6;
		const f32 fixedSegmentWidth = 120.0f * resolutionScale;
		const f32 separatorSpacing = 12.0f * resolutionScale;
		const f32 availableWidth = ImGui::GetContentRegionAvail().x;
		f32 currentSlotX = ImGui::GetCursorPosX();
		const f32 maxX = currentSlotX + availableWidth;
		for (i32 a = 0; a < sectionCount; ++a) {
			f32 requiredSpace = fixedSegmentWidth;
			if (a > 0) {
				requiredSpace += separatorSpacing;
			}
			if (currentSlotX + requiredSpace > maxX) {
				break;
			}
			if (a > 0) {
				ImGui::SameLine();
				ImGui::SetCursorPosX(currentSlotX);
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
				ImGui::SameLine(0.0f, separatorSpacing);
			}
			f32 slotStartX = (a == 0 ? currentSlotX : ImGui::GetCursorPosX());
			if (a == 0) {
				ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%s", sections[a]);
			}
			else {
				ImGui::TextUnformatted(sections[a]);
			}
			currentSlotX = slotStartX + fixedSegmentWidth;
		}
		EndScaledFont();
	}
	ImGui::End();
	ImGui::PopStyleVar(3);
}

void DebugPanel::OnAttach() {

}

void DebugPanel::OnDetach() {

}

}
