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

		explicit Properties(char const* title = "CrescentGL", u32 const width = 1920, u32 const height = 1080)
			: Title(title), Width(width), Height(height) { }
	};

	explicit Window(Properties const& properties = Properties());
	~Window();
	void OnUpdate();
	void Show() const { glfwShowWindow(m_Window); }
	[[nodiscard]] bool ShouldClose() const { return glfwWindowShouldClose(m_Window); }
	[[nodiscard]] GLFWwindow* GetWindow() const { return m_Window; }
	[[nodiscard]] u32 GetWidth() const { return m_Properties.Width; }
	[[nodiscard]] u32 GetHeight() const { return m_Properties.Height; }

private:
	GLFWwindow* m_Window = nullptr;
	Properties m_Properties;

	bool Init(Properties const& properties);
	void Shutdown() const;
	bool Create();
};

}
