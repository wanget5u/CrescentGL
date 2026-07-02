#include "Core/Window.h"
#include "log.h"

namespace Crescent {

Window::Window(Window::Properties const& windowProperties) {
	Init(windowProperties);
}

Window::~Window() {
	Shutdown();
}

bool Window::Init(Window::Properties const& windowProperties) {
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

	CreateWindow();

	glfwSetWindowUserPointer(m_Window, &m_Properties);
	glViewport(0, 0, m_Properties.Width, m_Properties.Height);
	glfwSetFramebufferSizeCallback(m_Window, FrameBufferCallback);

	m_LastFrameTime = static_cast<f32>(glfwGetTime());
	return true;
}

void Window::OnUpdate() {
	UpdateDeltaTime();
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void Window::Shutdown() {
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

bool Window::CreateWindow() {
	m_Window = glfwCreateWindow(m_Properties.Width, m_Properties.Height, m_Properties.Title, nullptr, nullptr);
	if (m_Window == nullptr) {
		Log::Error("Failed to initialize GLFW window.");
		return false;
	}
	glfwMakeContextCurrent(m_Window);
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == false) {
		Log::Error("Failed to initialize GLAD.");
		return false;
	}
	Log::Print("GLAD and OpenGL initialized successfully.");
	return true;
}

void Window::UpdateDeltaTime() {
	f32 currentFrameTime = static_cast<f32>(glfwGetTime());
	m_Properties.DeltaTime = currentFrameTime - m_LastFrameTime;
	m_LastFrameTime = currentFrameTime;
}

void Window::FrameBufferCallback(GLFWwindow* window, i32 width, i32 height) {
	Window::Properties* windowProperties = static_cast<Window::Properties*>(glfwGetWindowUserPointer(window));
	if (windowProperties != nullptr) {
		windowProperties->Width = width;
		windowProperties->Height = height;
		glViewport(0, 0, width, height);
	} else {
		Log::Error(
			"[Window] FrameBuffer size callback failed: Window user pointer is null.\n"
			"\tCannot update window dimensions to {}x{}. (GLFWwindow handle: {})",
			width, height, static_cast<void*>(window)
		);
		assert(false && "GLFW Window User Pointer was never initialized!");
	}
}

}
