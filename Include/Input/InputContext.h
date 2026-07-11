#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "Core/Core.h"
#include "Input/InputAction.h"

namespace Crescent::Input {
struct Context {
	enum class Type : u32 {
		Global,
		SceneEditor,
		Game
	};
	explicit Context(std::string name);
	~Context();
	Context(Context&&) noexcept;
	Context& operator=(Context&&) noexcept;
	Action& AddAction(std::string const& actionName);
	Action* GetAction(std::string_view actionName);
	void SetMouseCaptureFilter(std::function<bool()> filter);
	void SetKeyboardCaptureFilter(std::function<bool()> filter);
	void OnUpdate(GLFWwindow* window, f32 mouseDeltaX, f32 mouseDeltaY, f32 scrollDelta);
private:
	std::string m_Name{};
	std::unordered_map<std::string, std::unique_ptr<Action>> m_Actions{};
	std::function<bool()> m_MouseCaptureFilter{nullptr};
	std::function<bool()> m_KeyboardCaptureFilter{nullptr};
};
}