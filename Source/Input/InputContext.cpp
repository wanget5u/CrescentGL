#include "Input/InputContext.h"
#include "Input/InputAction.h"

#include <utility>

namespace Crescent::Input {

Context::Context(std::string name) : m_Name(std::move(name)) {}

Context::~Context() = default;
Context::Context(Context&&) noexcept = default;
Context& Context::operator=(Context&&) noexcept = default;

Action& Context::AddAction(std::string const& actionName) {
	std::pair<std::unordered_map<std::string, std::unique_ptr<Action>>::iterator, bool> result
		= m_Actions.try_emplace(actionName, std::make_unique<Action>());
	return *(result.first->second);
}

Action* Context::GetAction(std::string_view const actionName) {
	std::unordered_map<std::string, std::unique_ptr<Action>>::iterator it
		= m_Actions.find(std::string(actionName));
	if (it != m_Actions.end()) {
		return it->second.get();
	}
	return nullptr;
}

void Context::OnUpdate(GLFWwindow* window, f32 const mouseDeltaX, f32 const mouseDeltaY, f32 const scrollDelta) {
	for (std::pair<const std::string, std::unique_ptr<Action>>& action: m_Actions) {
		action.second->OnUpdate(window, mouseDeltaX, mouseDeltaY, scrollDelta);
	}
}

}
