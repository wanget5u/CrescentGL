#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "core.h"

namespace Crescent {

class Window {
public:
struct Properties {
	const char* Title;
	u32 Width;
	u32 Height;
	f32 DeltaTime;

	Properties(char const* title = "CrescentGL", u32 width = 1920, u32 height = 1080)
		: Title(title), Width(width), Height(height) { }
};

	Window(Properties const& properties = Properties());
	~Window();
	void OnUpdate();
	bool ShouldClose() const { return glfwWindowShouldClose(m_Window); }
	GLFWwindow* GetWindow() const { return m_Window; }
	u32 GetWidth() const { return m_Properties.Width; }
	u32 GetHeight() const { return m_Properties.Height; }
	f32 GetDeltaTime() const { return m_Properties.DeltaTime; }
	f32 GetTotalTime() const { return static_cast<f32>(glfwGetTime()); }

private:
	GLFWwindow* m_Window;
	Properties m_Properties;
	f32 m_LastFrameTime;

	bool Init(Properties const& properties);
	void Shutdown();
	bool CreateWindow();
	void UpdateDeltaTime();
	static void FrameBufferCallback(GLFWwindow* window, i32 width, i32 height);
};

}
