#include <utility>

#include "Input/InputContext.h"

namespace Crescent::Input {

Context::Context(std::string name) : m_Name(std::move(name)) {}

Action& Context::AddAction(std::string const& actionName) {
	return m_Actions[actionName];
}

Action* Context::GetAction(std::string_view const actionName) {
	auto it = m_Actions.find(std::string(actionName));
	if (it != m_Actions.end()) {
		return &it->second;
	}
	return nullptr;
}

void Context::OnUpdate(GLFWwindow* window, f32 mouseDeltaX, f32 mouseDeltaY, f32 scrollDelta) {
	for (auto& [actionName, action] : m_Actions) {
		action.OnUpdate(window, mouseDeltaX, mouseDeltaY, scrollDelta);
	}
}

}