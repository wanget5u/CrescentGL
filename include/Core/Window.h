#pragma once
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Core/Core.h"

namespace Crescent {

class Window {
public:
	struct Properties {
		const char8* Title;
		u32 Width;
		u32 Height;

		explicit Properties(char8 const* title = "CrescentGL", u32 const width = 1920, u32 const height = 1080)
			: Title(title), Width(width), Height(height) { }
	};

	explicit Window(Properties const& properties = Properties());
	~Window();
	void OnUpdate();
	void Show() const { glfwShowWindow(m_Window); }
	void SetRenderCallback(std::function<void()> const& callback) { m_RenderCallback = callback; }
	void TriggerRender() const {
		if (m_RenderCallback != nullptr) {
			m_RenderCallback();
			glfwSwapBuffers(m_Window);
		}
	}
	void OnResize(u32 const width, u32 const height) {
		m_Properties.Width = width;
		m_Properties.Height = height;
		glViewport(0, 0, width, height);
		TriggerRender();
	}
	[[nodiscard]] bool ShouldClose() const { return glfwWindowShouldClose(m_Window); }
	[[nodiscard]] GLFWwindow* GetWindow() const { return m_Window; }
	[[nodiscard]] u32 GetWidth() const { return m_Properties.Width; }
	[[nodiscard]] u32 GetHeight() const { return m_Properties.Height; }

private:
	GLFWwindow* m_Window = nullptr;
	Properties m_Properties;
	std::function<void()> m_RenderCallback;

	bool Init(Properties const& properties);
	void Shutdown() const;
	bool Create();
};

}
