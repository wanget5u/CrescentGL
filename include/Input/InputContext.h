#pragma once
#include <string>
#include "Core/Core.h"
#include "Input/InputAction.h"

namespace Crescent::Input {

class Context {
public:
	enum class Type : u32 {
		Editor,
		Game
	};

	explicit Context(std::string name);
	Action& AddAction(std::string const& actionName);
	Action* GetAction(std::string_view actionName);
	void OnUpdate(GLFWwindow* window, f32 mouseDeltaX, f32 mouseDeltaY, f32 scrollDelta);

private:
	std::string m_Name;
	std::unordered_map<std::string, Action> m_Actions;
};

}