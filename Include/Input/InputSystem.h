#pragma once
#include <memory>
#include <vector>

#include "Input/InputContext.h"
#include "Input/InputCodes.h"
#include "Input/InputListener.h"

namespace Crescent::Input {
struct System {
	static System& Instance() {
		static System s_Instance;
		return s_Instance;
	}
	void OnCreate(GLFWwindow* window);
	void OnUpdate();
	void SetContextActive(Context::Type contextType, bool active = true);
	bool IsContextActive(Context::Type contextType) const;
	Context* GetContext(Context::Type contextType);
	GLFWwindow* GetWindow() const;
	bool IsKeyPressed(KeyCode keyCode) const;
	bool IsKeyHeld(KeyCode keyCode) const;
	bool IsMousePressed(MouseButton mouseButton) const;
	f32 GetMouseDeltaX() const;
	f32 GetMouseDeltaY() const;
	f32 GetScrollDelta() const;
	void SetCursorPos(f64 x, f64 y);
	void GetCursorPos(f64& x, f64& y) const;
	void RegisterListener(IInputListener* listener);
	void UnregisterListener(const IInputListener* listener);
	void OnKeyboardKeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);
	void OnMouseButtonCallback(GLFWwindow* window, i32 button, i32 action, i32 mods);
	void OnMouseScrollCallback(GLFWwindow* window, f64 xOffset, f64 yOffset);
	void OnCursorCallback(GLFWwindow* window, f64 xPos, f64 yPos) const;
	void OnCharCallback(GLFWwindow* window, u32 c) const;
	void OnWindowFocusCallback(GLFWwindow* window, i32 focused) const;
	void OnCursorEnterCallback(GLFWwindow* window, i32 entered) const;
private:
	GLFWwindow* m_Window{nullptr};
	std::vector<Context::Type> m_ActiveContexts{};
	std::unordered_map<Context::Type, std::unique_ptr<Context>> m_Contexts{};
	std::unordered_map<i32, bool> m_KeyboardKeyState{};
	std::unordered_map<i32, bool> m_MouseButtonState{};
	std::vector<IInputListener*> m_Listeners{};
	f64 m_LastCursorX{0.0f};
	f64 m_LastCursorY{0.0f};
	f32 m_MouseDeltaX{0.0f};
	f32 m_MouseDeltaY{0.0f};
	f32 m_ScrollDelta{0.0f};
	System() = default;
	~System() = default;
	Context& CreateContext(Context::Type contextType);
};
}
