#include "Input/InputContext.h"
#include <utility>
#include <ranges>

namespace Crescent::Input {

Context::Context(std::string name) : m_Name(std::move(name)) {}

Action& Context::AddAction(std::string const& actionName) {
	auto [it, inserted] = m_Actions.try_emplace(actionName, std::make_unique<Action>());
	return *(it->second);
}

Action* Context::GetAction(std::string_view const actionName) {
	auto it = m_Actions.find(std::string(actionName));
	if (it != m_Actions.end()) {
		return it->second.get();
	}
	return nullptr;
}

void Context::OnUpdate(GLFWwindow* window, f32 const mouseDeltaX, f32 const mouseDeltaY, f32 const scrollDelta) {
	for (auto &action: m_Actions | std::views::values) {
		action->OnUpdate(window, mouseDeltaX, mouseDeltaY, scrollDelta);
	}
}

}
