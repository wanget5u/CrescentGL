#pragma once
#include <memory>
#include <string>
#include "Input/InputContext.h"

namespace Crescent::Input {
struct System {
	static System& Instance() {
		static System s_Instance;
		return s_Instance;
	}
	void OnCreate(GLFWwindow* window);
	void OnUpdate();
	Context& CreateContext(Context::Type contextType);
	void SetActiveContext(Context::Type contextType);
	Context* GetActiveContext();
	bool IsKeyPressed(KeyCode keyCode) const;
	bool IsKeyHeld(KeyCode keyCode) const;
	bool IsMousePressed(MouseButton mouseButton) const;
	f32 GetMouseDeltaX() const;
	f32 GetMouseDeltaY() const;
	f32 GetScrollDelta() const;
	void OnKeyboardKeyCallback(i32 key, i32 action, i32 mods);
	void OnMouseButtonCallback(i32 button, i32 action, i32 mods);
	void OnMouseScrollCallback(f64 xOffset, f64 yOffset);
	void OnCursorCallback(f64 xPos, f64 yPos) const;
private:
	System() = default;
	GLFWwindow* m_Window{nullptr};
	Context::Type m_ActiveContext{};
	std::unordered_map<Context::Type, std::unique_ptr<Context>> m_Contexts{};
	std::unordered_map<i32, bool> m_KeyboardKeyState{};
	std::unordered_map<i32, bool> m_MouseButtonState{};
	f64 m_LastCursorX{0.0f};
	f64 m_LastCursorY{0.0f};
	f32 m_MouseDeltaX{0.0f};
	f32 m_MouseDeltaY{0.0f};
	f32 m_ScrollDelta{0.0f};
};

}