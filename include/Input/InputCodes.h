#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/Core.h"

namespace Crescent::Input {
enum class KeyCode : i32 {
	Space		= GLFW_KEY_SPACE,
	Escape		= GLFW_KEY_ESCAPE,
	W			= GLFW_KEY_W,
	A			= GLFW_KEY_A,
	S			= GLFW_KEY_S,
	D			= GLFW_KEY_D,
	F11			= GLFW_KEY_F11,
	LeftShift	= GLFW_KEY_LEFT_SHIFT,
};
enum class MouseButton : i32 {
	Left		= GLFW_MOUSE_BUTTON_1,
	Right		= GLFW_MOUSE_BUTTON_2,
	Middle		= GLFW_MOUSE_BUTTON_3,
};
}