#include "Application.h"
#include <thread>
#include <chrono>

namespace Crescent {

Application::Application() {
	m_Window = new Window(Window::Properties("CrescentGL", 1280, 720));
	ProcessAndFocus();
}

Application::~Application() {
	delete m_Window;
}

void Application::Run() {
	while (IsRunning() == true && m_Window->ShouldClose() == false) {
		ProcessInput();

		glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		m_Window->OnUpdate();
	}
}

bool Application::IsRunning() {
	return m_Running;
}

void Application::Close() {
	m_Running = false;
}

void Application::ProcessAndFocus() {
	glfwPollEvents();
	glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
	glfwFocusWindow(m_Window->GetWindow());
}

void Application::ProcessInput() {
	if (glfwGetKey(m_Window->GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		Close();
	}
}

}