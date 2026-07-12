#pragma once
#include <functional>
#include <string>
#include <string_view>

#include "InputCodes.h"
#include "Core/Core.h"

namespace Crescent::Input {
enum class MouseAxis {
	X, Y, ScrollX, ScrollY
};
struct Binding {
	enum class Type { 
		Key, MouseButton, MouseAxis
	};
	Type type{};
	KeyCode keyCode{};
	MouseButton mouseButton{};
	MouseAxis mouseAxis{};
	f32 scale{1.0f};
	static Binding FromKey(KeyCode keyCode);
	static Binding FromMouseButton(MouseButton keyCode);
	static Binding FromMouseAxis(MouseAxis axis, f32 scale);
};
struct Action {
	enum class Phase {
		Pressed, Held, Released
	};
	struct Event {
		std::string_view ActionName;
		Phase Phase;
		f32 Value;
		Event(std::string const& actionName, Action::Phase phase, f32 value);
	};
	using Callback = std::function<void(Event const&)>;
	void BindKeyboardKey(KeyCode code);
	void BindMouseButton(MouseButton button);
	void BindMouseAxis(MouseAxis axis, f32 scale = 1.0f);
	u32 Subscribe(Callback callback);
	void Unsubscribe(u32 id);
	void OnUpdate(GLFWwindow* window, f32 mouseDeltaX, f32 mouseDeltaY, f32 scrollDelta, bool mouseCaptured = false, bool keyboardCaptured = false);
	[[nodiscard]] bool IsActive() const;
private:
	std::string m_Name{};
	std::vector<Binding> m_Bindings{};
	std::unordered_map<u32, Callback> m_Subscribers{};
	u32 m_NextID{0};
	bool m_WasActive{false};
	bool m_IsActive{false};
	f32 m_AnalogValue{0.0f};
	bool EvaluateBindings(GLFWwindow* window, f32 mouseDeltaX, f32 mouseDeltaY, f32 scrollDelta, bool mouseCaptured, bool keyboardCaptured);
	bool EvaluateKeyboardKeyBindings(GLFWwindow* window, Binding const& binding);
	bool EvaluateMouseButtonBindings(GLFWwindow* window, Binding const& binding);
	bool EvaluateMouseScrollBindings(GLFWwindow* window, Binding const& binding, f32 mouseDeltaX, f32 mouseDeltaY, f32 scrollDelta);
};
}