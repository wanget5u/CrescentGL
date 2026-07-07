#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <glad/glad.h>
#include <Core/Core.h>

#include "../../Math/Vector/Vector2.h"
#include "../../Math/Vector/Vector3.h"
#include "../../Math/Vector/Vector4.h"
#include "Math/Matrix/Matrix4x4.h"

namespace Crescent::Render {
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
	u32 ID{0};
	explicit Shader(std::string_view vertexSource, std::string_view fragmentSource);
	~Shader();
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;
	void SetBool(std::string_view name, bool value) const;
	void SetInt(std::string_view name, i32 value) const;
	void SetFloat(std::string_view name, f32 value) const;
	void SetVector2(std::string_view name, Math::Vector2 const& value) const;
	void SetVector3(std::string_view name, Math::Vector3 const& value) const;
	void SetVector4(std::string_view name, Math::Vector4 const& value) const;
	void SetMatrix4(std::string_view name, Math::Matrix4x4 const& matrix) const;
private:
	mutable std::unordered_map<std::string, i32> m_UniformLocationCache;
	friend struct Material;
	void Use() const;
	static bool LogCompileErrors(u32 shader, Type type);
	[[nodiscard]] i32 GetUniformLocation(std::string_view name) const;
};

}
