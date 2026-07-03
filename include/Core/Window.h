#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Core/Core.h"

namespace Crescent {

class Window {
public:
	struct Properties {
		const char* Title;
		u32 Width;
		u32 Height;

		Properties(char const* title = "CrescentGL", u32 width = 1920, u32 height = 1080)
			: Title(title), Width(width), Height(height) { }
	};

	Window(Properties const& properties = Properties());
	~Window();
	void OnUpdate();
	void Show() { glfwShowWindow(m_Window); }
	bool ShouldClose() const { return glfwWindowShouldClose(m_Window); }
	GLFWwindow* GetWindow() const { return m_Window; }
	u32 GetWidth() const { return m_Properties.Width; }
	u32 GetHeight() const { return m_Properties.Height; }

private:
	GLFWwindow* m_Window;
	Properties m_Properties;

	bool Init(Properties const& properties);
	void Shutdown();
	bool CreateWindow();
};

}
