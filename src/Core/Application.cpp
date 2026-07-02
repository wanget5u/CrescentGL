#include <thread>
#include <chrono>
#include "log.h"
#include "Math/Math.h"
#include "Util/Util.h"
#include "Core/Application.h"

namespace Crescent {

Application::Application() {
	m_Window = new Window(Window::Properties("CrescentGL", SCREEN_WIDTH, SCREEN_HEIGHT));
	ProcessAndFocus();
}

Application::~Application() {
	delete m_Window;
}

void Application::Run() {
	std::string vertexShaderStr = Crescent::Util::ReadFile("Shaders/default.vert");
	std::string fragmentShaderStr = Crescent::Util::ReadFile("Shaders/default.frag");

	const char* vertexShaderSource = vertexShaderStr.c_str();
	const char* fragmentShaderSource = fragmentShaderStr.c_str();

	u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	i32 success;
	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (success == false) {
		glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
		Log::Error("Shader vertex compilation failed: \n", infoLog);
	}

	u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (success == false) {
		glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
		Log::Error("Shader framgent compilation failed: \n", infoLog);
	}

	// shader program linkage
	u32 shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (success == false) {
		glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
		Log::Error("Shader program linking failed: \n", infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	f32 vertices[] = {
		-0.4f,  0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		 0.5f, -0.4f,  0.0f,

		 0.4f, -0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f,
		-0.5f,  0.4f,  0.0f
	};
	u32 indices[] = {
		0, 1, 2,
		3, 4, 5
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

	// set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	f32 totalTime = 0.0f;

	while (IsRunning() == true && m_Window->ShouldClose() == false) {
		ProcessInput();

		// background
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		totalTime +=  m_Window->GetDeltaTime();
		f32 linearFactor = fmodf(totalTime * 0.5f, 1.0f);
		f32 smoothFactor = Crescent::Math::SmoothStep(0.0f, 1.0f, linearFactor);
		
		i32 timeFactorParam = glGetUniformLocation(shaderProgram, "u_TimeFactor");
		glUniform1f(timeFactorParam, smoothFactor);

		i32 triangleIDParam = glGetUniformLocation(shaderProgram, "u_TriangleID");
		glUniform1i(triangleIDParam, 0);

		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
		
		glUniform1i(triangleIDParam, 1);

		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3 * sizeof(u32)));

		m_Window->OnUpdate();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);
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
