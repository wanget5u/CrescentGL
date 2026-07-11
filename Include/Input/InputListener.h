#pragma once
#include "Core/Core.h"

struct GLFWwindow;
namespace Crescent::Input {
struct IInputListener {
	virtual ~IInputListener() = default;
	virtual void OnKeyboardKey(GLFWwindow* /*window*/, i32 /*key*/, i32 /*scancode*/, i32 /*action*/, i32 /*mods*/) {}
	virtual void OnMouseButton(GLFWwindow* /*window*/, i32 /*button*/, i32 /*action*/, i32 /*mods*/) {}
	virtual void OnMouseScroll(GLFWwindow* /*window*/, f64 /*xOffset*/, f64 /*yOffset*/) {}
	virtual void OnCursorPos(GLFWwindow* /*window*/, f64 /*xPos*/, f64 /*yPos*/) {}
	virtual void OnChar(GLFWwindow* /*window*/, u32 /*c*/) {}
	virtual void OnWindowFocus(GLFWwindow* /*window*/, i32 /*focused*/) {}
	virtual void OnCursorEnter(GLFWwindow* /*window*/, i32 /*entered*/) {}
};
}
