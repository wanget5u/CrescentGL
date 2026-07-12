#pragma once

struct GLFWwindow;

namespace Crescent::Win32 {
	void SubclassGLFWWindow(GLFWwindow* window);
	void UnsubclassGLFWWindow(GLFWwindow* window);
	bool EnableANSI();
}