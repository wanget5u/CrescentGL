#include "Core/Window.h"

#include "Core/Log.h"

namespace Crescent {

static void FrameBufferCallback(GLFWwindow* window, i32 const width, i32 const height) {
	Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
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
	CalculateAspectRatio();
}

GLFWmonitor* Window::GetActiveMonitor() {
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	if (primaryMonitor != nullptr) {
		return primaryMonitor;
	}
	i32 monitorCount{};
	GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
	if (monitorCount > 0) {
		return monitors[0];
	}
	return nullptr;
}

const GLFWvidmode* Window::GetActiveMonitorVideoMode() {
	return glfwGetVideoMode(GetActiveMonitor());
}

i32 Window::GetActiveMonitorWidth() {
	const GLFWvidmode* videoMode = GetActiveMonitorVideoMode();
	return videoMode ? videoMode->width : 0;
}

i32 Window::GetActiveMonitorHeight() {
	const GLFWvidmode* videoMode = GetActiveMonitorVideoMode();
	return videoMode ? videoMode->height : 0;
}

i32 Window::GetActiveMonitorRefreshRate() {
	const GLFWvidmode* videoMode = GetActiveMonitorVideoMode();
	return videoMode ? videoMode->refreshRate : 0;
}

i32 Window::GetActiveMonitorVideoModeCenterX(i32 const targetWidth) {
	return (GetActiveMonitorVideoMode()->width - targetWidth) / 2;
}

i32 Window::GetActiveMonitorVideoModeCenterY(i32 const targetHeight) {
	return (GetActiveMonitorVideoMode()->height - targetHeight) / 2;
}

void Window::OnUpdate() const {
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void Window::ShowWindow() const {
	glfwShowWindow(m_Window);
}

void Window::MakeContextCurrent() const {
	glfwMakeContextCurrent(m_Window);
}

void Window::SwapBuffers() const {
	glfwSwapBuffers(m_Window);
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

bool Window::IsFullscreen() const {
	return m_IsFullscreen;
}

void Window::OnResize(u32 const width, u32 const height) {
	m_Properties.Width = width;
	m_Properties.Height = height;
	m_FrameBufferResized = true;
	CalculateAspectRatio();
}

void Window::CheckViewportResize() {
	if (m_FrameBufferResized.exchange(false) == true) {
		glViewport(0, 0, m_Properties.Width, m_Properties.Height);
	}
}

bool Window::ShouldClose() const { return glfwWindowShouldClose(m_Window); }
GLFWwindow * Window::GetWindow() const { return m_Window; }
i32 Window::GetWindowWidth() const { return m_Properties.Width; }
i32 Window::GetWindowHeight() const { return m_Properties.Height; }
f32 Window::GetAspectRatio() const { return m_LastAspectRatio; }

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

void Window::CenterOnPrimaryMonitor() const {
	i32 xPos = (GetActiveMonitorWidth() - GetWindowWidth()) / 2;
	i32 yPos = (GetActiveMonitorHeight() - GetWindowHeight()) / 2;
	glfwSetWindowPos(m_Window, xPos, yPos);
}

void Window::CalculateAspectRatio() {
	const i32 height = GetWindowHeight();
	if (height == 0) { m_LastAspectRatio = 1.0f; }
	m_LastAspectRatio = static_cast<f32>(GetWindowWidth()) / static_cast<f32>(height);
}
}
