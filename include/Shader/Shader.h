#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <glad/glad.h>
#include <Core/Core.h>
#include "../Math/Vector/Vector2.h"
#include "../Math/Vector/Vector3.h"
#include "../Math/Vector/Vector4.h"

namespace Crescent {

struct Shader {
	enum class Type {
		Vertex,
		Fragment,
		Program
	};

	static const char8* GetTypeName(Type const type) {
		switch (type) {
			case Type::Vertex:   return "Vertex";
			case Type::Fragment: return "Fragment";
			case Type::Program:  return "Program";
			default:             return "Unknown";
		}
	}

	u32 ID{ 0 };

	explicit Shader(const char8* vertexPath, const char8* fragmentPath);
	~Shader() {
		if (ID != 0) {
			glDeleteProgram(ID);
		}
	}

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	Shader(Shader&& other) noexcept : ID(other.ID) { other.ID = 0; }
	Shader& operator=(Shader&& other) noexcept {
		if (this != &other) {
			if (ID != 0) {
				glDeleteProgram(ID);
			}
			ID = other.ID;
			other.ID = 0;
		}
		return *this;
	}

	void Use() const {
		glUseProgram(ID);
	};

	void SetBool(std::string_view const name, bool const value) const {
		glUniform1i(GetUniformLocation(name), static_cast<i32>(value));
	}
	void SetInt(std::string_view const name, i32 const value) const {
		glUniform1i(GetUniformLocation(name), value);
	}
	void SetFloat(std::string_view const name, f32 const value) const {
		glUniform1f(GetUniformLocation(name), value);
	}
	void SetVector2(std::string_view const name, Math::Vector2 const& value) const {
		glUniform2fv(GetUniformLocation(name), 1, value.data);
	}
	void SetVector3(std::string_view const name, Math::Vector3 const& value) const {
		glUniform3fv(GetUniformLocation(name), 1, value.data);
	}
	void SetVector4(std::string_view const name, Math::Vector4 const& value) const {
		glUniform4fv(GetUniformLocation(name), 1, value.data);
	}
	// void SetMatrix4(std::string_view name, Math::Matrix4x4 const& matrix) const {
	// 	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, matrix.data);
	// }

private:
	mutable std::unordered_map<std::string, i32> m_UniformLocationCache;

	static bool LogCompileErrors(u32 shader, Type type);
	[[nodiscard]] i32 GetUniformLocation(std::string_view name) const;
};

}
