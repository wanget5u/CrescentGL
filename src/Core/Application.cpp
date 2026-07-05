#include <thread>
#include <chrono>
#include "Core/Log.h"
#include "Core/Time.h"
#include "Util/Util.h"
#include "Core/Application.h"

#include <complex>

#include "Input/InputSystem.h"
#include "Math/Trigonometry.h"
#include "Math/Matrix/Matrix4x4.h"
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
		// positions         // colors          // texture coords
		1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
	   -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
	   -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f
   };

	u32 indices[] = {
		0, 1, 2,
		0, 2, 3
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), reinterpret_cast<void*>(3 * sizeof(f32)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), reinterpret_cast<void*>(6 * sizeof(f32)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	i32 width{};
	i32 height{};
	i32 nrChannels{};

	glActiveTexture(GL_TEXTURE0);
	u32 texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	uchar8 *data1 = stbi_load("Assets/Textures/Tiles081_1K-JPG_Color.jpg", &width, &height, &nrChannels, 0);
	if (data1 != nullptr) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		Log::Warning("stbi_load1");
	}
	stbi_image_free(data1);

	glBindTexture(GL_TEXTURE_2D, texture1);

	glActiveTexture(GL_TEXTURE1);
	u32 texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	uchar8 *data2 = stbi_load("Assets/Textures/Tiles129B_1K-JPG_Color.jpg", &width, &height, &nrChannels, 0);
	if (data2 != nullptr) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		Log::Warning("stbi_load2");
	}
	stbi_image_free(data2);

	shader.Use();
	glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
	shader.SetInt("texture2", 1);

	f32 alphaBlendValue{};
	f32 zoomFactorValue{};
	f32 sinus{};
	f32 cosinus{};

	auto renderFrame = [this, &shader, texture1, texture2, VAO, &alphaBlendValue, &zoomFactorValue]() {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		alphaBlendValue = (std::sin(Time::GetTotalTime() * 1.25f) * 0.2f) + 0.5f;
		zoomFactorValue = (std::sin(Time::GetTotalTime() * 1.5f) * 0.1f) + 0.7f;
		shader.SetFloat("alphaBlend", alphaBlendValue);
		shader.SetFloat("rotationAngle", Time::GetTotalTime());
		shader.SetFloat("zoomFactor", zoomFactorValue);

		for (u8 a = 0; a < 64; a++) {
			f32 mul = a * 2.0f;
			Math::Matrix4x4 matrix{};

			matrix.Scale(Math::Vector3(zoomFactorValue, zoomFactorValue, zoomFactorValue));
			matrix.RotateLocal(Math::DegToRad((Time::GetTotalTime() * 20.0f + mul) + mul), Math::Vector3(0.0f, 0.0f, 1.0f));
			f32 spacing = 0.03f;
			matrix.TranslateWorld(Math::Vector3(0.2f - (a * spacing), -0.2f + (a * spacing), 0.0f));
			matrix.RotateWorld(Math::DegToRad((Time::GetTotalTime() * 50.0f + mul) + mul), Math::Vector3(0.0f, 0.0f, 1.0f));
			shader.SetMatrix4("transform", matrix);

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		}
	};

	m_Window->SetRenderCallback(renderFrame);

	while (IsRunning() == true && m_Window->ShouldClose() == false) {
		Time::OnUpdate(static_cast<f32>(glfwGetTime()));
		ProcessInput();
		UpdateDeltaTime();
		while (Time::AccumulatorHasSubstep()) {
			Time::ConsumeSubstep();
		}
		renderFrame();
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
