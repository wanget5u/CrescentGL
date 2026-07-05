#include "Core/Window.h"
#include "Core/Log.h"

namespace Crescent {

static void FrameBufferCallback(GLFWwindow* window, i32 const width, i32 const height) {
	Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (windowInstance == nullptr) { return; }
	windowInstance->OnResize(width, height);
}

Window::Window(Properties const& properties) {
	m_Properties = properties;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, m_Properties.Visible ? GLFW_TRUE : GLFW_FALSE);
	CreateWindowInstance();
	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, FrameBufferCallback);
}

void Window::OnUpdate() const {
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void Window::ToggleFullscreen() {
	if (m_IsFullscreen == true) {
		glfwSetWindowMonitor(m_Window, nullptr, m_WindowedPosX, m_WindowedPosY, m_WindowedWidth, m_WindowedHeight, GLFW_DONT_CARE);
		m_IsFullscreen = false;
	} else {
		glfwGetWindowPos(m_Window, &m_WindowedPosX, &m_WindowedPosY);
		glfwGetWindowSize(m_Window, &m_WindowedWidth, &m_WindowedHeight);
		glfwSetWindowMonitor(m_Window, GetActiveMonitor(), 0, 0, GetActiveMonitorWidth(), GetActiveMonitorHeight(), GLFW_DONT_CARE);
		m_IsFullscreen = true;
	}
}

bool Window::CreateWindowInstance() {
	GLFWwindow* shareWindow = m_Properties.ShareWindow != nullptr ? m_Properties.ShareWindow->GetWindow() : nullptr;
	m_Window = glfwCreateWindow(m_Properties.Width, m_Properties.Height, m_Properties.Title, nullptr, shareWindow);
	if (m_Window == nullptr) {
		Log::Error("Failed to initialize GLFW window.");
		return false;
	}
	CenterOnPrimaryMonitor();
	glfwMakeContextCurrent(m_Window);
	if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == false) {
		Log::Error("Failed to initialize GLAD.");
		return false;
	}
	Log::Info("GLAD and OpenGL initialized successfully.");
	return true;
}

}
