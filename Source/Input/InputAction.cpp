#include "Input/InputAction.h"

#include <unordered_map>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/Log.h"

namespace Crescent::Input {
	Binding Binding::FromKey(KeyCode const keyCode) {
		Binding binding{};
		binding.type = Type::Key;
		binding.keyCode = keyCode;
		return binding;
	}

	Binding Binding::FromMouseButton(MouseButton const keyCode) {
		Binding binding{};
		binding.type = Type::MouseButton;
		binding.mouseButton = keyCode;
		return binding;
	}

	Binding Binding::FromMouseAxis(MouseAxis const axis, f32 const scale) {
		Binding binding{};
		binding.type = Type::MouseAxis;
		binding.mouseAxis = axis;
		binding.scale = scale;
		return binding;
	}

	Action::Event::Event(std::string const &actionName, Action::Phase const phase, f32 const value): ActionName(actionName), Phase(phase), Value(value) {}
	void Action::BindKeyboardKey(KeyCode const code) { m_Bindings.push_back(Binding::FromKey(code)); }
	void Action::BindMouseButton(MouseButton const button) { m_Bindings.push_back(Binding::FromMouseButton(button)); }
	void Action::BindMouseAxis(MouseAxis const axis, f32 const scale) { m_Bindings.push_back(Binding::FromMouseAxis(axis, scale)); }

	u32 Action::Subscribe(Callback callback) {
	u32 id = m_NextID++;
	m_Subscribers[id] = std::move(callback);
	return id;
}

void Action::Unsubscribe(u32 const id) {
	size_t erasedCount = m_Subscribers.erase(id);
	if (erasedCount == 0) {
		Log::Warning("[InputAction] Tried to erase non-existing subscription.");
	}
}

bool Action::IsActive() const {
	return m_IsActive;
}

void Action::OnUpdate(GLFWwindow* window, f32 const mouseDeltaX, f32 const mouseDeltaY, f32 const scrollDelta, bool const mouseCaptured, bool const keyboardCaptured) {
	m_IsActive = EvaluateBindings(window, mouseDeltaX, mouseDeltaY, scrollDelta, mouseCaptured, keyboardCaptured);
	if (m_Subscribers.empty() == true) {
		return;
	}
	Phase phase{};
	bool shouldFire = false;
	if (m_IsActive == true && m_WasActive == false) {
		phase = Phase::Pressed;
		shouldFire = true;
	}
	else if (m_IsActive == true && m_WasActive == true) {
		phase = Phase::Held;
		shouldFire = true;
	}
	else if (m_IsActive == false && m_WasActive == true) {
		phase = Phase::Released;
		shouldFire = true;
	}
	if (shouldFire == true) {
		Event event(m_Name, phase, m_AnalogValue);
		for (std::pair<u32, Callback> subscriber: m_Subscribers) {
			subscriber.second(event);
		}
	}
	m_WasActive = m_IsActive;
}

bool Action::EvaluateBindings(GLFWwindow* window, f32 const mouseDeltaX, f32 const mouseDeltaY, f32 const scrollDelta, bool const mouseCaptured, bool const keyboardCaptured) {
	for (Binding const& binding : m_Bindings) {
		if (binding.type == Binding::Type::Key && keyboardCaptured == true) {
			continue;
		}
		if ((binding.type == Binding::Type::MouseButton || binding.type == Binding::Type::MouseAxis) && mouseCaptured == true) {
			continue;
		}
		bool matched = false;
		switch (binding.type) {
		case Binding::Type::Key:
			matched = EvaluateKeyboardKeyBindings(window, binding);
			break;
		case Binding::Type::MouseButton:
			matched = EvaluateMouseButtonBindings(window, binding);
			break;
		case Binding::Type::MouseAxis:
			matched = EvaluateMouseScrollBindings(window, binding, mouseDeltaX, mouseDeltaY, scrollDelta);
			break;
		}
		if (matched == true) {
			return true;
		}
	}
	m_AnalogValue = 0.0f;
	return false;
}

bool Action::EvaluateKeyboardKeyBindings(GLFWwindow* window, Binding const& binding) {
	i32 state = glfwGetKey(window, static_cast<i32>(binding.keyCode));
	if (state == GLFW_PRESS || state == GLFW_REPEAT) {
		m_AnalogValue = 1.0f;
		return true;
	}
	return false;
}

bool Action::EvaluateMouseButtonBindings(GLFWwindow* window, Binding const& binding) {
	i32 state = glfwGetMouseButton(window, static_cast<i32>(binding.mouseButton));
	if (state == GLFW_PRESS) {
		m_AnalogValue = 1.0f;
		return true;
	}
	return false;
}

bool Action::EvaluateMouseScrollBindings([[maybe_unused]] GLFWwindow* window, Binding const& binding, f32 const mouseDeltaX, const f32 mouseDeltaY, const f32 scrollDelta) {
	f32 raw = 0.0f;
	switch (binding.mouseAxis) {
	case MouseAxis::X:
		raw = mouseDeltaX; break;
	case MouseAxis::Y:
		raw = mouseDeltaY; break;
	case MouseAxis::ScrollX:
		raw = 0.0f; break;
	case MouseAxis::ScrollY:
		raw = scrollDelta; break;
	}
	f32 value = raw * binding.scale;
	if (value != 0.0f) {
		m_AnalogValue = value;
		return true;
	}
	return false;
}

}
