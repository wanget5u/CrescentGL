#include "Input/InputSystem.h"
#include "Input/InputAction.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Crescent::Input {

void GLFWKeyboardKeyCallback(
	GLFWwindow* window,
	i32 const key,
	i32 const scancode,
	i32 const action,
	i32 const mods) {
	System::Instance().OnKeyboardKeyCallback(window, key, scancode, action, mods);
}
void GLFWMouseButtonCallback(
	GLFWwindow* window,
	i32 const button,
	i32 const action,
	i32 const mods) {
	System::Instance().OnMouseButtonCallback(window, button, action, mods);
}
void GLFWMouseScrollCallback(
	GLFWwindow* window,
	f64 const xOffset,
	f64 const yOffset) {
	System::Instance().OnMouseScrollCallback(window, xOffset, yOffset);
}
void GLFWCursorPosCallback(
	GLFWwindow* window,
	f64 const xPos,
	f64 const yPos) {
	System::Instance().OnCursorCallback(window, xPos, yPos);
}
void GLFWCharCallback(
	GLFWwindow* window,
	u32 const c) {
	System::Instance().OnCharCallback(window, c);
}
void GLFWWindowFocusCallback(
	GLFWwindow* window,
	i32 const focused) {
	System::Instance().OnWindowFocusCallback(window, focused);
}
void GLFWCursorEnterCallback(
	GLFWwindow* window,
	i32 const entered) {
	System::Instance().OnCursorEnterCallback(window, entered);
}

void System::OnCreate(GLFWwindow* window) {
	m_Window = window;
	glfwGetCursorPos(m_Window, &m_LastCursorX, &m_LastCursorY);
	glfwSetKeyCallback(m_Window, GLFWKeyboardKeyCallback);
	glfwSetMouseButtonCallback(m_Window, GLFWMouseButtonCallback);
	glfwSetScrollCallback(m_Window, GLFWMouseScrollCallback);
	glfwSetCursorPosCallback(m_Window, GLFWCursorPosCallback);
	glfwSetCharCallback(m_Window, GLFWCharCallback);
	glfwSetWindowFocusCallback(m_Window, GLFWWindowFocusCallback);
	glfwSetCursorEnterCallback(m_Window, GLFWCursorEnterCallback);

	CreateContext(Context::Type::Global);
	CreateContext(Context::Type::SceneEditor);
	CreateContext(Context::Type::Game);
}

void System::OnUpdate() {
	f64 cursorX = 0.0l;
	f64 cursorY = 0.0l;
	glfwGetCursorPos(m_Window, &cursorX, &cursorY);
	m_MouseDeltaX = static_cast<f32>(cursorX - m_LastCursorX);
	m_MouseDeltaY = static_cast<f32>(cursorY - m_LastCursorY);
	m_LastCursorX = cursorX;
	m_LastCursorY = cursorY;
	for (Context::Type const contextType : m_ActiveContexts) {
		std::unordered_map<Context::Type, std::unique_ptr<Context>>::iterator it
			= m_Contexts.find(contextType);
		if (it != m_Contexts.end()) {
			it->second->OnUpdate(m_Window, m_MouseDeltaX, m_MouseDeltaY, m_ScrollDelta);
		}
	}
	m_ScrollDelta = 0.0f;
}

void System::SetContextActive(const Context::Type contextType, const bool active) {
	std::vector<Context::Type>::iterator it
		= std::ranges::find(m_ActiveContexts, contextType);
	if (active == true && it == m_ActiveContexts.end()) {
		m_ActiveContexts.push_back(contextType);
	}
	else if (active == false && it != m_ActiveContexts.end()) {
		m_ActiveContexts.erase(it);
	}
}

bool System::IsContextActive(const Context::Type contextType) const {
	return std::ranges::find(m_ActiveContexts, contextType) != m_ActiveContexts.end();
}

Context& System::CreateContext(Context::Type contextType) {
	m_Contexts.try_emplace(
		contextType, std::make_unique<Context>("context_" + std::to_string(static_cast<u32>(contextType))));
	return *m_Contexts.at(contextType);
}

Context* System::GetContext(const Context::Type contextType) {
	std::unordered_map<Context::Type, std::unique_ptr<Context>>::iterator it
		= m_Contexts.find(contextType);
	return (it != m_Contexts.end()) ? it->second.get() : nullptr;
}

GLFWwindow* System::GetWindow() const {
	return m_Window;
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
	glfwSetCursorPos(m_Window, x, y);
	m_LastCursorX = x;
	m_LastCursorY = y;
}

void System::GetCursorPos(f64& x, f64& y) const {
	glfwGetCursorPos(m_Window, &x, &y);
}

void System::RegisterListener(IInputListener* listener) {
	if (listener == nullptr) {
		return;
	}
	for (IInputListener* l : m_Listeners) {
		if (l == listener) {
			return;
		}
	}
	m_Listeners.push_back(listener);
}

void System::UnregisterListener(const IInputListener* listener) {
	for (size_t a = 0; a < m_Listeners.size(); ++a) {
		if (m_Listeners[a] == listener) {
			m_Listeners.erase(m_Listeners.begin() + a);
			break;
		}
	}
}

void System::OnKeyboardKeyCallback(GLFWwindow* window, i32 const key, i32 const scancode, i32 const action, i32 const mods) {
	m_KeyboardKeyState[key] = (action != GLFW_RELEASE);
	std::vector<IInputListener*> copy = m_Listeners;
	for (IInputListener* listener : copy) {
		listener->OnKeyboardKey(window, key, scancode, action, mods);
	}
}

void System::OnMouseButtonCallback(GLFWwindow* window, i32 const button, i32 const action, i32 const mods) {
	m_MouseButtonState[button] = (action != GLFW_RELEASE);
	std::vector<IInputListener*> copy = m_Listeners;
	for (IInputListener* listener : copy) {
		listener->OnMouseButton(window, button, action, mods);
	}
}

void System::OnMouseScrollCallback(GLFWwindow* window, f64 const xOffset, f64 const yOffset) {
	m_ScrollDelta += static_cast<f32>(yOffset);
	std::vector<IInputListener*> copy = m_Listeners;
	for (IInputListener* listener : copy) {
		listener->OnMouseScroll(window, xOffset, yOffset);
	}
}

void System::OnCursorCallback(GLFWwindow* window, f64 const xPos, f64 const yPos) const {
	std::vector<IInputListener*> copy = m_Listeners;
	for (IInputListener* listener : copy) {
		listener->OnCursorPos(window, xPos, yPos);
	}
}

void System::OnCharCallback(GLFWwindow* window, u32 const c) const {
	std::vector<IInputListener*> copy = m_Listeners;
	for (IInputListener* listener : copy) {
		listener->OnChar(window, c);
	}
}

void System::OnWindowFocusCallback(GLFWwindow* window, i32 const focused) const {
	std::vector<IInputListener*> copy = m_Listeners;
	for (IInputListener* listener : copy) {
		listener->OnWindowFocus(window, focused);
	}
}

void System::OnCursorEnterCallback(GLFWwindow* window, i32 const entered) const {
	std::vector<IInputListener*> copy = m_Listeners;
	for (IInputListener* listener : copy) {
		listener->OnCursorEnter(window, entered);
	}
}

}
