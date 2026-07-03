#pragma once
#include <string>
#include "Input/InputContext.h"

namespace Crescent::Input {

class System {
public:
	static System& Instance();

	void Init(GLFWwindow* window);
	void OnUpdate();

	Input::Context& CreateContext(Input::Context::Type contextType);
	void SetActiveContext(Input::Context::Type contextType);
	Input::Context* GetActiveContext();

	bool IsKeyPressed(Input::KeyCode keyCode) const {
		return glfwGetKey(m_Window, static_cast<i32>(keyCode)) == GLFW_PRESS;
	}
	bool IsKeyHeld(Input::KeyCode keyCode) const {
		i32 keyState = glfwGetKey(m_Window, static_cast<i32>(keyCode));
		return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
	}
	bool IsMousePressed(Input::MouseButton mouseButton) const {
		return glfwGetMouseButton(m_Window, static_cast<i32>(mouseButton)) == GLFW_PRESS;
	}

	f32 GetMouseDeltaX() const { return m_MouseDeltaX; }
	f32 GetMouseDeltaY() const { return m_MouseDeltaY; }
	f32 GetScrollDelta() const { return m_ScrollDelta; }

	void OnKeyboardKeyCallback(i32 key, i32 action, i32 mods);
	void OnMouseButtonCallback(i32 button, i32 action, i32 mods);
	void OnMouseScrollCallback(f64 xOffset, f64 yOffset);
	void OnCursorCallback(f64 xPos, f64 yPos);

private:
	System() = default;

	GLFWwindow* m_Window = nullptr;
	Input::Context::Type m_ActiveContext;
	std::unordered_map<Input::Context::Type, Input::Context> m_Contexts;

	std::unordered_map<i32, bool> m_KeyboardKeyState;
	std::unordered_map<i32, bool> m_MouseButtonState;

	f64 m_LastCursorX = 0.0;
	f64 m_LastCursorY = 0.0;
	f32 m_MouseDeltaX = 0.0f;
	f32 m_MouseDeltaY = 0.0f;
	f32 m_ScrollDelta = 0.0f;
};

}