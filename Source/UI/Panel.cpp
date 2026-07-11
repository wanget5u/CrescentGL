#include "UI/Panel.h"

#include <algorithm>
#include "imgui.h"

namespace Crescent::UI {

bool Panel::IsVisible() const noexcept {
	return m_IsVisible;
}

void Panel::SetVisibility(const bool isVisible) {
	m_IsVisible = isVisible;
}

f32 Panel::GetResolutionScale() const noexcept {
	if (ImGui::GetCurrentContext() == nullptr) {
		return 1.0f;
	}
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	if (viewport == nullptr || viewport->WorkSize.y <= 0.0f) {
		return 1.0f;
	}
	return std::clamp(viewport->WorkSize.y / 1080.0f, 0.6f, 2.5f);
}

f32 Panel::GetViewportWidth() const noexcept {
	if (ImGui::GetCurrentContext() == nullptr) {
		return 1920.0f;
	}
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	return viewport ? viewport->WorkSize.x : 1920.0f;
}

f32 Panel::GetViewportHeight() const noexcept {
	if (ImGui::GetCurrentContext() == nullptr) {
		return 1080.0f;
	}
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	return viewport ? viewport->WorkSize.y : 1080.0f;
}

void Panel::BeginScaledFont() const {
	if (ImGui::GetCurrentContext() != nullptr) {
		ImGui::SetWindowFontScale(GetResolutionScale());
	}
}

void Panel::EndScaledFont() const {
	if (ImGui::GetCurrentContext() != nullptr) {
		ImGui::SetWindowFontScale(1.0f);
	}
}

}
