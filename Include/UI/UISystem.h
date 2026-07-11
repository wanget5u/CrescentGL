#pragma once
#include <atomic>
#include <memory>
#include <mutex>

#include "Panel.h"
#include "Collection/DynamicList.h"
#include "Input/InputListener.h"

struct GLFWwindow;
struct ImGuiContext;

namespace Crescent::UI {
struct System : Input::IInputListener {
	static System& Instance() {
		static System s_Instance;
		return s_Instance;
	}
	void OnCreatePlatform(GLFWwindow* window);  // call on Main Thread
	void OnCreateRenderer();                    // call on Render Thread
	void ShutdownRenderer();					// call on Render Thread
	void ShutdownPlatform();					// call on Main Thread
	void OnUpdate();							// call on Main Thread after PollEvents
	void OnRenderGUI(f32 deltaTime);			// call on Render Thread
	[[nodiscard]] bool WantCaptureMouse() const;
	[[nodiscard]] bool WantCaptureKeyboard() const;
	template <typename T>
	[[nodiscard]] T* GetPanel();
	template <typename T, typename... Args>
	void RegisterPanel(Args&&... args);
	/// IInputListener overrides
	void OnKeyboardKey(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) override;
	void OnMouseButton(GLFWwindow* window, i32 button, i32 action, i32 mods) override;
	void OnMouseScroll(GLFWwindow* window, f64 xOffset, f64 yOffset) override;
	void OnCursorPos(GLFWwindow* window, f64 xPos, f64 yPos) override;
	void OnChar(GLFWwindow* window, u32 c) override;
	void OnWindowFocus(GLFWwindow* window, i32 focused) override;
	void OnCursorEnter(GLFWwindow* window, i32 entered) override;
private:
	System() = default;
	~System() override;
	void PlatformNewFrame();
	void BeginFrame(f32 deltaTime) const;
	void EndFrame() const;
	void OnRenderGUIInternal(f32 deltaTime);
	DynamicList<std::unique_ptr<Panel>> m_Panels{};
	mutable std::mutex m_Mutex{};
	ImGuiContext* m_ImGuiContext{nullptr};
	std::atomic<bool> m_FramePrepared{false};
	bool m_PlatformInitialized{false};
	bool m_RendererInitialized{false};
};

template<typename T>
T* System::GetPanel() {
	static_assert(std::is_base_of_v<Panel, T>, "Panel must derive from UI::Panel");
	std::scoped_lock lock(m_Mutex);
	for (std::unique_ptr<Panel>& panel : m_Panels) {
		if (T* castPanel = dynamic_cast<T*>(panel.get())) {
			return castPanel;
		}
	}
	return nullptr;
}
template<typename T, typename ... Args>
void System::RegisterPanel(Args &&...args) {
	static_assert(std::is_base_of_v<Panel, T>, "Panel must derive from UI::Panel");
	std::scoped_lock lock(m_Mutex);
	std::unique_ptr<T> panel = std::make_unique<T>(std::forward<Args>(args)...);
	panel->OnAttach();
	m_Panels.PushBack(std::move(panel));
}
}
