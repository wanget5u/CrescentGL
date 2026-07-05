#include "Core/Window.h"
#include "Core/Log.h"

namespace Crescent {

static void FrameBufferCallback(GLFWwindow* window, i32 width, i32 height) {
	Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (windowInstance != nullptr) {
		windowInstance->OnResize(width, height);
	}
	else {
		Log::Error(
			"[Window] FrameBuffer size callback failed: Window user pointer is null.\n"
			"\tCannot update window dimensions to {}x{}. (GLFWwindow handle: {})",
			width, height, static_cast<void*>(window)
		);
		assert(false && "GLFW Window User Pointer was never initialized!");
	}
}

static void WindowPosCallback(GLFWwindow* window, i32 xpos, i32 ypos) {
	Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (windowInstance != nullptr) {
		windowInstance->TriggerRender();
	}
}

static void WindowRefreshCallback(GLFWwindow* window) {
	Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (windowInstance != nullptr) {
		windowInstance->TriggerRender();
	}
}

Window::Window(Properties const& windowProperties) {
	Init(windowProperties);
}

Window::~Window() {
	Shutdown();
}

bool Window::Init(Properties const& windowProperties) {
	m_Properties.Title = windowProperties.Title;
	m_Properties.Width = windowProperties.Width;
	m_Properties.Height = windowProperties.Height;
	if (glfwInit() == false) {
		Log::Error("Failed to initialize GLFW.");
		return false;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	Create();

	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, FrameBufferCallback);
	glfwSetWindowPosCallback(m_Window, WindowPosCallback);
	glfwSetWindowRefreshCallback(m_Window, WindowRefreshCallback);

	return true;
}

void Window::OnUpdate() {
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void Window::Shutdown() const {
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

bool Window::Create() {
	m_Window = glfwCreateWindow(m_Properties.Width, m_Properties.Height, m_Properties.Title, nullptr, nullptr);
	if (m_Window == nullptr) {
		Log::Error("Failed to initialize GLFW window.");
		return false;
	}
	glfwMakeContextCurrent(m_Window);
	if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == false) {
		Log::Error("Failed to initialize GLAD.");
		return false;
	}
	Log::Info("GLAD and OpenGL initialized successfully.");
	return true;
}

}
