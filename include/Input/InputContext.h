#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "Core/Core.h"

struct GLFWwindow;

namespace Crescent::Input {
struct Action;
struct Context {
	enum class Type : u32 {
		Editor,
		Game
	};
	explicit Context(std::string name);
	~Context();
	Context(Context&&) noexcept;
	Context& operator=(Context&&) noexcept;
	Action& AddAction(std::string const& actionName);
	Action* GetAction(std::string_view actionName);
	void OnUpdate(GLFWwindow* window, f32 mouseDeltaX, f32 mouseDeltaY, f32 scrollDelta);
private:
	std::string m_Name{""};
	std::unordered_map<std::string, std::unique_ptr<Action>> m_Actions{};
};
}
