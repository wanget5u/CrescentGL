#pragma once
#include <string_view>
#include <functional>
#include <string>
#include "Core/Core.h"
#include "InputCodes.h"

namespace Crescent::Input {

enum class MouseAxis {
	X,			// mouse left/right
	Y,			// mouse up  /down
	ScrollX,	// horizontal scroll wheel
	ScrollY		// vertical scroll wheel
};

struct Binding {
	enum class Type { 
		Key, 
		MouseButton, 
		MouseAxis 
	};

	Type type;
	KeyCode keyCode = KeyCode::Space;
	MouseButton mouseCode = MouseButton::Button0;
	MouseAxis mouseAxis = MouseAxis::Y;
	f32 scale = 1.0f;

	static Binding FromKey(KeyCode const keyCode) {
		Binding binding; 
		binding.type = Type::Key; 
		binding.keyCode = keyCode;
		return binding; 
	}
	static Binding FromMouseButton(MouseButton const keyCode) {
		Binding binding;
		binding.type = Type::MouseButton; 
		binding.mouseCode = keyCode;
		return binding; 
	}
	static Binding FromMouseAxis(MouseAxis const axis, f32 const scale) {
		Binding binding;
		binding.type = Type::MouseAxis; 
		binding.mouseAxis = axis;
		binding.scale = scale;
		return binding; 
	}
};

class Action {
public:
	enum class Phase {
		Pressed,
		Held,
		Released
	};
	struct Event {
		std::string_view ActionName;
		Phase Phase;
		f32 Value;

		Event(std::string const& actionName, Action::Phase const phase, f32 const value)
			: ActionName(actionName), Phase(phase), Value(value) {}
	};

	using Callback = std::function<void(Event const&)>;
	using SubscriptionID = u32;

	void BindKeyboardKey(KeyCode code);
	void BindMouseButton(MouseButton button);
	void BindMouseAxis(MouseAxis axis, f32 scale = 1.0f);
	SubscriptionID Subscribe(Callback callback);
	void Unsubscribe(SubscriptionID id);
	void OnUpdate(GLFWwindow* window, f32 mouseDeltaX, f32 mouseDeltaY, f32 scrollDelta);

private:
	std::string m_Name;
	std::vector<Binding> m_Bindings;
	std::unordered_map<u32, Callback> m_Subscribers;
	u32 m_NextID = 0;
	bool m_WasActive = false;
	bool m_IsActive = false;
	f32 m_AnalogValue = 0.0f;
	
	bool EvaluateBindings(GLFWwindow* window, f32 mouseDeltaX, f32 mouseDeltaY, f32 scrollDelta);
	bool EvaluateKeyboardKeyBindings(GLFWwindow* window, Binding const& binding);
	bool EvaluateMouseButtonBindings(GLFWwindow* window, Binding const& binding);
	bool EvaluateMouseScrollBindings(GLFWwindow* window, Binding const& binding, f32 mouseDeltaX, f32 mouseDeltaY, f32 scrollDelta);
};

}