#include "Render/Shader/Shader.h"

#include <glad/glad.h>
#include "Core/Log.h"
#include "Util/FileOperations.h"

namespace Crescent::Render {

Shader::Shader(const std::string_view vertexSource, const std::string_view fragmentSource) {
	const char8* vertexShaderCode = vertexSource.data();
	const char8* fragmentShaderCode = fragmentSource.data();
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

Shader::~Shader() { if (ID != 0) {glDeleteProgram(ID);} }
Shader::Shader(Shader &&other) noexcept: ID(other.ID) { other.ID = 0; }
Shader & Shader::operator=(Shader &&other) noexcept {
	if (this != &other) {
		if (ID != 0) { glDeleteProgram(ID); }
		ID = other.ID;
		other.ID = 0;
	}
	return *this;
}

void Shader::SetBool(std::string_view const name, bool const value) const {
	glUniform1i(GetUniformLocation(name), static_cast<i32>(value));
}

void Shader::SetInt(std::string_view const name, i32 const value) const {
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(std::string_view const name, f32 const value) const {
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVector2(std::string_view const name, Math::Vector2 const &value) const {
	glUniform2fv(GetUniformLocation(name), 1, value.data);
}

void Shader::SetVector3(std::string_view const name, Math::Vector3 const &value) const {
	glUniform3fv(GetUniformLocation(name), 1, value.data);
}

void Shader::SetVector4(std::string_view const name, Math::Vector4 const &value) const {
	glUniform4fv(GetUniformLocation(name), 1, value.data);
}

void Shader::SetMatrix4(std::string_view const name, Math::Matrix4x4 const &matrix) const {
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, matrix.Data());
}

void Shader::Use() const {
	glUseProgram(ID);
}

bool Shader::LogCompileErrors(u32 const shader, Type const type) {
	i32 success;
	char8 infoLog[512];
	if (type == Type::Program) {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (success == false) {
			glGetProgramInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
			Log::Error("Shader Program linking Error ({}):\n{}", GetTypeName(type), infoLog);
			return false;
		}
	} else {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (success == false) {
			glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
			Log::Error("Shader Compilation error ({}):\n{}", GetTypeName(type), infoLog);
			return false;
		}
	}
	return true;
}

i32 Shader::GetUniformLocation(const std::string_view name) const {
	std::map<std::string, i32, std::less<>>::iterator it
		= m_UniformLocationCache.find(name);
	if (it != m_UniformLocationCache.end()) {
		return it->second;
	}
	std::string nameKey(name);
	const i32 location = glGetUniformLocation(ID, nameKey.c_str());
	m_UniformLocationCache[std::move(nameKey)] = location;
	return location;
}

}
