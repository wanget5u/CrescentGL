#include <thread>
#include <chrono>
#include "log.h"
#include "Math/Math.h"
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
	// vertex shader
	const char* vertexShaderSource = "#version 460 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	u32 vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	i32 success;
	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (success == false) {
		glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
		Log::Error("Shader vertex compilation failed: ", infoLog);
	}

	// fragment shader
	const char* fragmentShaderSource = "#version 460 core\n"
		"out vec4 FragColor;\n"
		"uniform vec4 m_Color;\n"
		"void main()\n"
		"{\n"
		"	FragColor = m_Color;\n"
		"}\n";

	u32 fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (success == false) {
		glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
		Log::Error("Shader framgent compilation failed: ", infoLog);
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
		Log::Error("Shader program linking failed: ", infoLog);
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

	while (IsRunning() == true && m_Window->ShouldClose() == false) {
		ProcessInput();

		// background
		glClearColor(0.4f, 0.3f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		i32 colorParam = glGetUniformLocation(shaderProgram, "m_Color");

		glUniform4f(colorParam, 0.6f, 0.5f, 0.9f, 1.0f);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

		glUniform4f(colorParam, 0.9f, 0.5f, 0.6f, 1.0f);
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
