#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Input/InputSystem.h"

namespace Crescent::Input {

void GLFWKeyboardKeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
	System::Instance().OnKeyboardKeyCallback(key, action, mods);
}

void GLFWMouseButtonCallback(GLFWwindow* window, i32 button, i32 action, i32 mods) {
	System::Instance().OnMouseButtonCallback(button, action, mods);
}
	
void GLFWMouseScrollCallback(GLFWwindow* window, f64 xoffset, f64 yoffset) {
	System::Instance().OnMouseScrollCallback(xoffset, yoffset);
}

void GLFWCursorPosCallback(GLFWwindow* window, f64 xpos, f64 ypos) {
	System::Instance().OnCursorCallback(xpos, ypos);
}

System& System::Instance() {
	static System s_Instance;
	return s_Instance;
}

void System::Init(GLFWwindow* window) {
	m_Window = window;
	glfwGetCursorPos(m_Window, &m_LastCursorX, &m_LastCursorY);
	glfwSetKeyCallback(m_Window, GLFWKeyboardKeyCallback);
	glfwSetMouseButtonCallback(m_Window, GLFWMouseButtonCallback);
	glfwSetScrollCallback(m_Window, GLFWMouseScrollCallback);
	glfwSetCursorPosCallback(m_Window, GLFWCursorPosCallback);
}

void System::OnUpdate() {
	f64 cursorX = 0.0l;
	f64 cursorY = 0.0l;
	glfwGetCursorPos(m_Window, &cursorX, &cursorY);
	m_MouseDeltaX = static_cast<f32>(cursorX - m_LastCursorX);
	m_MouseDeltaY = static_cast<f32>(cursorY - m_LastCursorY);
	m_LastCursorX = cursorX;
	m_LastCursorY = cursorY;

	std::unordered_map<Context::Type, Context>::iterator it =
		m_Contexts.find(m_ActiveContext);
	if (it != m_Contexts.end()) {
		it->second.OnUpdate(m_Window, m_MouseDeltaX, m_MouseDeltaY, m_ScrollDelta);
	}

	m_ScrollDelta = 0.0f;
}

Context& System::CreateContext(Context::Type contextType) {
	m_Contexts.try_emplace(contextType, "context_" + std::to_string(static_cast<u32>(contextType)));
	return m_Contexts.at(contextType);
}

void System::SetActiveContext(Context::Type contextType) {
	m_ActiveContext = contextType;
}

Context* System::GetActiveContext() {
	auto it = m_Contexts.find(m_ActiveContext);
	if (it != m_Contexts.end()) {
		return &it->second;
	}
	return nullptr;
}

void System::OnKeyboardKeyCallback(i32 key, i32 action, i32 mods) {
	m_KeyboardKeyState[key] = (action != GLFW_RELEASE);
}
void System::OnMouseButtonCallback(i32 button, i32 action, i32 mods) {
	m_MouseButtonState[button] = (action != GLFW_RELEASE);
}
void System::OnMouseScrollCallback(f64 xOffset, f64 yOffset) {
	m_ScrollDelta += static_cast<f32>(yOffset);
}
void System::OnCursorCallback(f64 xPos, f64 yPos) {

}

}