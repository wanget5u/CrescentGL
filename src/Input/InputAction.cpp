#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <unordered_map>
#include "Core/Log.h"
#include "Input/InputAction.h"

namespace Crescent::Input {

void Action::BindKeyboardKey(Input::KeyCode code) {
	m_Bindings.push_back(Binding::FromKey(code));
}

void Action::BindMouseButton(Input::MouseButton button) {
	m_Bindings.push_back(Binding::FromMouseButton(button));
}

void Action::BindMouseAxis(Input::MouseAxis axis, f32 scale) {
	m_Bindings.push_back(Binding::FromMouseAxis(axis, scale));
}

Action::SubscriptionID Action::Subscribe(Callback callback) {
	u32 id = m_NextID++;
	m_Subscribers[id] = std::move(callback);
	return id;
}

void Action::Unsubscribe(Action::SubscriptionID id) {
	size_t erasedCount = m_Subscribers.erase(id);
	if (erasedCount == 0) {
		Log::Warning("[InputAction] Tried to erase non-existing subscription.");
	}
}

void Action::OnUpdate(GLFWwindow* window, f32 mouseDeltaX, f32 mouseDeltaY, f32 scrollDelta) {
	m_IsActive = EvaluateBindings(window, mouseDeltaX, mouseDeltaY, scrollDelta);

	if (m_Subscribers.empty() == true) {
		return;
	}

	Action::Phase phase;
	bool shouldFire = false;

	if (m_IsActive == true && m_WasActive == false) {
		phase = Action::Phase::Pressed;
		shouldFire = true;
	}
	else if (m_IsActive == true && m_WasActive == true) {
		phase = Action::Phase::Held;
		shouldFire = true;
	}
	else if (m_IsActive == false && m_WasActive == true) {
		phase = Action::Phase::Released;
		shouldFire = true;
	}

	if (shouldFire == true) {
		Action::Event event(m_Name, phase, m_AnalogValue);
		for (auto const& [id, callback] : m_Subscribers) {
			callback(event);
		}
	}
	m_WasActive = m_IsActive;
}

bool Action::EvaluateBindings(GLFWwindow* window, f32 mouseDeltaX, f32 mouseDeltaY, f32 scrollDelta) {
	for (Binding const& binding : m_Bindings) {
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
	i32 state = glfwGetKey(window, static_cast<i32>(binding.keyCode));
	if (state == GLFW_PRESS) {
		m_AnalogValue = 1.0f;
		return true;
	}
	return false;
}

bool Action::EvaluateMouseScrollBindings(GLFWwindow* window, Binding const& binding, f32 mouseDeltaX, f32 mouseDeltaY, f32 scrollDelta) {
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