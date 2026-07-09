#include "Input/InputSystem.h"
#include "Input/InputAction.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Crescent::Input {

System::~System() = default;

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

void System::SetCursorPos(f64 const x, f64 const y) {
	if (m_Window != nullptr) {
		glfwSetCursorPos(m_Window, x, y);
		m_LastCursorX = x;
		m_LastCursorY = y;
	}
}

void System::GetCursorPos(f64& x, f64& y) const {
	if (m_Window != nullptr) {
		glfwGetCursorPos(m_Window, &x, &y);
	}
	else {
		x = 0.0;
		y = 0.0;
	}
}

void System::WrapCursor(i32 const windowWidth, i32 const windowHeight) {
	if (m_Window == nullptr || windowWidth <= 4 || windowHeight <= 4) {
		return;
	}
	f64 cursorX = 0.0;
	f64 cursorY = 0.0;
	glfwGetCursorPos(m_Window, &cursorX, &cursorY);
	bool wrapped = false;
	constexpr f64 margin = 2.0;
	const f64 width = windowWidth;
	const f64 height = windowHeight;
	const f64 offsetX = width - 4.0;
	const f64 offsetY = height - 4.0;
	if (cursorX < margin) {
		cursorX += offsetX;
		wrapped = true;
	}
	else if (cursorX > width - margin) {
		cursorX -= offsetX;
		wrapped = true;
	}
	if (cursorY < margin) {
		cursorY += offsetY;
		wrapped = true;
	}
	else if (cursorY > height - margin) {
		cursorY -= offsetY;
		wrapped = true;
	}
	if (wrapped == true) {
		SetCursorPos(cursorX, cursorY);
	}
}

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
