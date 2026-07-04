#include <thread>
#include <chrono>
#include "Core/Log.h"
#include "Core/Time.h"
#include "Util/Util.h"
#include "Core/Application.h"

#include <complex>

#include "Input/InputSystem.h"
#include "Shader/Shader.h"

namespace Crescent {

Application::Application() {
	m_Window = new Window(Window::Properties("Window", SCREEN_WIDTH, SCREEN_HEIGHT));
	glfwPollEvents();
	m_Window->Show();
	SetupInputActions();
}

Application::~Application() {
	delete m_Window;
}

void Application::Run() {
	Shader shader("Shaders/default.vert", "Shaders/default.frag");

	f32 vertices[] = {
		// positions			// colors
		 0.0f,  0.5f,  0.0f,	1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.0f,	0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.0f,	0.0f, 0.0f, 1.0f
	};

	u32 indices[] = {
		0, 1, 2
	};

	u32 VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(f32), nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(f32), reinterpret_cast<void*>(3 * sizeof(f32)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shader.Use();
	while (IsRunning() == true && m_Window->ShouldClose() == false) {
		Time::OnUpdate(static_cast<f32>(glfwGetTime()));
		ProcessInput();
		UpdateDeltaTime();
		while (Time::AccumulatorHasSubstep()) {
			Time::ConsumeSubstep();
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

		m_Window->OnUpdate();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

bool Application::IsRunning() const {
	return m_Running;
}

void Application::Close() {
	m_Running = false;
}

void Application::SetupInputActions() {
	Input::System::Instance().Init(m_Window->GetWindow());
	Input::Context& appContext = Input::System::Instance().CreateContext(Input::Context::Type::Editor);
	Input::System::Instance().SetActiveContext(Input::Context::Type::Editor);

	Input::Action& closeAction = appContext.AddAction("Close");
	closeAction.BindKeyboardKey(Input::KeyCode::Escape);
	closeAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (actionEvent.Phase == Input::Action::Phase::Pressed) {
			Close();
		}
	});
}

void Application::ProcessInput() const {
	Input::System::Instance().OnUpdate();
}

void Application::UpdateDeltaTime() {
	f32 currentFrameTime = static_cast<f32>(glfwGetTime());
	m_DeltaTime = currentFrameTime - m_LastFrameTime;
	m_LastFrameTime = currentFrameTime;
}

}
