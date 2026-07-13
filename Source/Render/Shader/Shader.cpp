#include "Render/Shader/Shader.h"

#include <sstream>
#include <glad/glad.h>
#include "Core/Log.h"
#include "Util/FileOperations.h"
#include "Render/GPUDisposalQueue.h"

namespace Crescent {

Shader::Shader(const std::string_view vertexSource, const std::string_view fragmentSource, const std::string_view geometrySource) {
	std::unordered_set<std::string> vertexIncludedFiles{};
	std::string parsedVertex = ParseIncludes(vertexSource, vertexIncludedFiles);
	std::unordered_set<std::string> fragmentIncludedFiles{};
	std::string parsedFragment = ParseIncludes(fragmentSource, fragmentIncludedFiles);
	std::unordered_set<std::string> geometryIncludedFiles{};
	std::string parsedGeometry{};
	bool useGeometry = geometrySource.empty() == false;
	if (useGeometry == true) {
		parsedGeometry = ParseIncludes(geometrySource, geometryIncludedFiles);
	}
	const char8* vertexShaderCode = parsedVertex.c_str();
	const char8* fragmentShaderCode = parsedFragment.c_str();
	const char8* geometryShaderCode = parsedGeometry.c_str();
	u32 vertexShader{};
	u32 fragmentShader{};
	u32 geometryShader{};

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
	glCompileShader(vertexShader);
	const bool vertexSuccess = LogCompileErrors(vertexShader, Type::Vertex);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
	glCompileShader(fragmentShader);
	const bool fragmentSuccess = LogCompileErrors(fragmentShader, Type::Fragment);

	bool geometrySuccess{false};
	if (useGeometry == true) {
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometryShaderCode, nullptr);
		glCompileShader(geometryShader);
		geometrySuccess = LogCompileErrors(geometryShader, Type::Geometry);
	}

	if (vertexSuccess == false || fragmentSuccess == false) {
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		if (useGeometry == true && geometrySuccess == false) {
			glDeleteShader(geometryShader);
		}
		return;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	if (useGeometry == true) {
		glAttachShader(ID, geometryShader);
	}

	glLinkProgram(ID);
	LogCompileErrors(ID, Type::Program);

	glDetachShader(ID, vertexShader);
	glDetachShader(ID, fragmentShader);
	if (useGeometry == true) {
		glDetachShader(ID, geometryShader);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (useGeometry == true) {
		glDeleteShader(geometryShader);
	}
}

Shader::~Shader() {
	if (ID != 0) {
		GPUDisposalQueue::SubmitProgramForDeletion(ID);
		ID = 0;
	}
}
Shader::Shader(Shader &&other) noexcept: ID(other.ID) { other.ID = 0; }
Shader & Shader::operator=(Shader &&other) noexcept {
	if (this != &other) {
		if (ID != 0) { GPUDisposalQueue::SubmitProgramForDeletion(ID); }
		ID = other.ID;
		other.ID = 0;
	}
	return *this;
}

std::string Shader::ParseIncludes(const std::string_view source, std::unordered_set<std::string>& includedFiles) {
	std::stringstream output{};
	std::string line{};
	std::stringstream sourceStream((std::string(source)));
	while (std::getline(sourceStream, line)) {
		// searching for the # to be the start if someone makes some indentation
		size_t hashPosition = line.find('#');
		if (hashPosition == std::string::npos || line.compare(hashPosition, 8, "#include") != 0) {
			output << line << "\n";
			continue;
		}
		// #include "Shaders/SceneRenderData.glsl"
		size_t start = line.find('"');
		size_t end = line.find('"', start + 1);
		if (start == std::string::npos || end == std::string::npos) {
			Log::Error("Shader #include directive: {}", line);
			output << line << "\n";
			continue;
		}
		std::string rawPath = line.substr(start + 1, end - start - 1);
		std::string includePath = rawPath.starts_with("Shaders/") ? rawPath : "Shaders/" + rawPath;
		// this acts like #pragma once
		if (includedFiles.contains(includePath)) {
			continue;
		}
		includedFiles.insert(includePath);
		std::string includeContent = Util::ReadFile(includePath);
		if (includeContent.empty()) {
			Log::Error("Shader include file empty or missing: {}", includePath);
			continue;
		}
		output << ParseIncludes(includeContent, includedFiles) << "\n";
	}
	return output.str();
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

void Shader::TrySetBool(std::string_view const name, bool const value) const {
	const i32 loc = TryGetUniformLocation(name);
	if (loc != -1) {
		glUniform1i(loc, static_cast<i32>(value));
	}
}

void Shader::TrySetInt(std::string_view const name, i32 const value) const {
	const i32 loc = TryGetUniformLocation(name);
	if (loc != -1) {
		glUniform1i(loc, value);
	}
}

void Shader::TrySetFloat(std::string_view const name, f32 const value) const {
	const i32 loc = TryGetUniformLocation(name);
	if (loc != -1) {
		glUniform1f(loc, value);
	}
}

void Shader::TrySetVector2(std::string_view const name, Math::Vector2 const &value) const {
	const i32 loc = TryGetUniformLocation(name);
	if (loc != -1) {
		glUniform2fv(loc, 1, value.data);
	}
}

void Shader::TrySetVector3(std::string_view const name, Math::Vector3 const &value) const {
	const i32 loc = TryGetUniformLocation(name);
	if (loc != -1) {
		glUniform3fv(loc, 1, value.data);
	}
}

void Shader::TrySetVector4(std::string_view const name, Math::Vector4 const &value) const {
	const i32 loc = TryGetUniformLocation(name);
	if (loc != -1) {
		glUniform4fv(loc, 1, value.data);
	}
}

void Shader::TrySetMatrix4(std::string_view const name, Math::Matrix4x4 const &matrix) const {
	const i32 loc = TryGetUniformLocation(name);
	if (loc != -1) {
		glUniformMatrix4fv(loc, 1, GL_FALSE, matrix.Data());
	}
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

i32 Shader::TryGetUniformLocation(const std::string_view name) const {
	auto const it = m_UniformLocationCache.find(name);
	if (it != m_UniformLocationCache.end()) {
		return it->second;
	}
	std::string nameKey(name);
	const i32 location = glGetUniformLocation(ID, nameKey.c_str());
	m_UniformLocationCache[std::move(nameKey)] = location;
	return location;
}

i32 Shader::GetUniformLocation(const std::string_view name) const {
	const i32 location = TryGetUniformLocation(name);
	if (location == -1) {
		std::string nameKey(name);
		const char* names[] = { nameKey.c_str() };
		GLuint index = GL_INVALID_INDEX;
		glGetUniformIndices(ID, 1, names, &index);
		if (index != GL_INVALID_INDEX) {
			GLint blockIndex = -1;
			glGetActiveUniformsiv(ID, 1, &index, GL_UNIFORM_BLOCK_INDEX, &blockIndex);
			if (blockIndex != -1) {
				return location;
			}
		}
		Log::Error("Uniform '{}' does not exist or was optimized away in shader ID {}.", name, ID);
		assert(false && "Mismatched or missing shader uniform parameter.\n");
	}
	return location;
}

}
