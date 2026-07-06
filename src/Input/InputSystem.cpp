#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Input/InputSystem.h"

namespace Crescent::Input {

void GLFWKeyboardKeyCallback(
	[[maybe_unused]] GLFWwindow* window,
	[[maybe_unused]] i32 const key,
	[[maybe_unused]] i32 const scancode,
	[[maybe_unused]] i32 const action,
	[[maybe_unused]] i32 const mods) {
	System::Instance().OnKeyboardKeyCallback(key, action, mods);
}
void GLFWMouseButtonCallback(
	[[maybe_unused]] GLFWwindow* window,
	[[maybe_unused]] i32 const button,
	[[maybe_unused]] i32 const action,
	[[maybe_unused]] i32 const mods) {
	System::Instance().OnMouseButtonCallback(button, action, mods);
}
void GLFWMouseScrollCallback(
	[[maybe_unused]] GLFWwindow* window,
	[[maybe_unused]] f64 const xOffset,
	[[maybe_unused]] f64 const yOffset) {
	System::Instance().OnMouseScrollCallback(xOffset, yOffset);
}
void GLFWCursorPosCallback(
	[[maybe_unused]] GLFWwindow* window,
	[[maybe_unused]] f64 const xPos,
	[[maybe_unused]] f64 const yPos) {
	System::Instance().OnCursorCallback(xPos, yPos);
}

void System::OnCreate(GLFWwindow* window) {
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
	auto it = m_Contexts.find(m_ActiveContext);
	if (it != m_Contexts.end()) {
		it->second->OnUpdate(m_Window, m_MouseDeltaX, m_MouseDeltaY, m_ScrollDelta);
	}
	m_ScrollDelta = 0.0f;
}

Context& System::CreateContext(Context::Type contextType) {
	m_Contexts.try_emplace(
		contextType, std::make_unique<Context>("context_" + std::to_string(static_cast<u32>(contextType))));
	return *m_Contexts.at(contextType);
}

void System::SetActiveContext(Context::Type const contextType) { m_ActiveContext = contextType; }

Context* System::GetActiveContext() {
	auto it = m_Contexts.find(m_ActiveContext);
	if (it != m_Contexts.end()) {
		return it->second.get();
	}
	return nullptr;
}

bool System::IsKeyPressed(KeyCode keyCode) const {
	return glfwGetKey(m_Window, static_cast<i32>(keyCode)) == GLFW_PRESS;
}

bool System::IsKeyHeld(KeyCode keyCode) const {
	i32 keyState = glfwGetKey(m_Window, static_cast<i32>(keyCode));
	return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
}

bool System::IsMousePressed(MouseButton mouseButton) const {
	return glfwGetMouseButton(m_Window, static_cast<i32>(mouseButton)) == GLFW_PRESS;
}

f32 System::GetMouseDeltaX() const { return m_MouseDeltaX; }
f32 System::GetMouseDeltaY() const { return m_MouseDeltaY; }
f32 System::GetScrollDelta() const { return m_ScrollDelta; }

void System::OnKeyboardKeyCallback([[maybe_unused]] i32 const key, [[maybe_unused]] i32 const action, [[maybe_unused]] i32 const mods) {
	m_KeyboardKeyState[key] = (action != GLFW_RELEASE);
}
void System::OnMouseButtonCallback([[maybe_unused]] i32 const button, [[maybe_unused]] i32 const action, [[maybe_unused]] i32 const mods) {
	m_MouseButtonState[button] = (action != GLFW_RELEASE);
}
void System::OnMouseScrollCallback([[maybe_unused]] f64 const xOffset, [[maybe_unused]] f64 const yOffset) {
	m_ScrollDelta += static_cast<f32>(yOffset);
}
void System::OnCursorCallback([[maybe_unused]] f64 const xPos, [[maybe_unused]] f64 const yPos) const {
	// TODO:
}

}