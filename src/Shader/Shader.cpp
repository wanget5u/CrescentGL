#include "Shader/Shader.h"
#include "Core/Log.h"
#include "Util/FileOperations.h"

namespace Crescent {

Shader::Shader(const char8* vertexPath, const char8* fragmentPath) {
	const std::string vertexShaderContent = Util::ReadFile(vertexPath);
	const std::string fragmentShaderContent = Util::ReadFile(fragmentPath);
	const char8* vertexShaderCode = vertexShaderContent.c_str();
	const char8* fragmentShaderCode = fragmentShaderContent.c_str();
	u32 vertexShader{};
	u32 fragmentShader{};

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
	glCompileShader(vertexShader);
	const bool vertexSuccess = LogCompileErrors(vertexShader, Type::Vertex);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
	glCompileShader(fragmentShader);
	const bool fragmentSuccess = LogCompileErrors(fragmentShader, Type::Fragment);

	if (vertexSuccess == false || fragmentSuccess == false) {
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	LogCompileErrors(ID, Type::Program);

	glDetachShader(ID, vertexShader);
	glDetachShader(ID, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

bool Shader::LogCompileErrors(u32 const shader, Type const type) {
	i32 success;
	char8 infoLog[512];
	if (type == Type::Program) {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (success == false) {
			glGetProgramInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
			Log::Error("Shader Program linking Error (", GetTypeName(type) ,"):\n", infoLog);
			return false;
		}
	} else {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (success == false) {
			glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
			Log::Error("Shader Compilation error (", GetTypeName(type), "):\n", infoLog);
			return false;
		}
	}
	return true;
}

i32 Shader::GetUniformLocation(std::string_view name) const {
	const std::string nameKey(name);
	auto it = m_UniformLocationCache.find(nameKey);
	if (it != m_UniformLocationCache.end()) {
		return it->second;
	}
	const i32 location = glGetUniformLocation(ID, nameKey.c_str());
	if (location == -1) {
		Log::Warning("Couldn't find a ", name, " uniform location.");
	}
	m_UniformLocationCache[nameKey] = location;
	return location;
}

}
