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
	void SetActiveContext(Context::Type const contextType) { m_ActiveContext = contextType; }
	Context* GetActiveContext();

	bool IsKeyPressed(KeyCode keyCode) const {
		return glfwGetKey(m_Window, static_cast<i32>(keyCode)) == GLFW_PRESS;
	}
	bool IsKeyHeld(KeyCode keyCode) const {
		i32 keyState = glfwGetKey(m_Window, static_cast<i32>(keyCode));
		return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
	}
	bool IsMousePressed(MouseButton mouseButton) const {
		return glfwGetMouseButton(m_Window, static_cast<i32>(mouseButton)) == GLFW_PRESS;
	}

	f32 GetMouseDeltaX() const { return m_MouseDeltaX; }
	f32 GetMouseDeltaY() const { return m_MouseDeltaY; }
	f32 GetScrollDelta() const { return m_ScrollDelta; }

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