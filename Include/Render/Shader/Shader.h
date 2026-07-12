#pragma once
#include <unordered_map>
#include <unordered_set>

#include "Core/Core.h"
#include "../../Math/Vector/Vector2.h"
#include "../../Math/Vector/Vector3.h"
#include "../../Math/Vector/Vector4.h"
#include "Math/Matrix/Matrix4x4.h"

namespace Crescent {
struct StringHash {
	using is_transparent = void;
	size_t operator()(const char* text) const noexcept {
		return std::hash<std::string_view>{}(text);
	}
	size_t operator()(const std::string_view text) const noexcept {
		return std::hash<std::string_view>{}(text);
	}
	size_t operator()(const std::string& text) const noexcept {
		return std::hash<std::string>{}(text);
	}
};
struct Shader {
	friend struct Material;
	enum class Type {
		Vertex,
		Fragment,
		Geometry,
		Program
	};
	static const char8* GetTypeName(Type const type) {
		switch (type) {
			case Type::Vertex:   return "Vertex";
			case Type::Fragment: return "Fragment";
			case Type::Geometry: return "Geometry";
			case Type::Program:  return "Program";
			default:             return "Unknown";
		}
	}
	u32 ID{0};
	explicit Shader(std::string_view vertexSource, std::string_view fragmentSource, std::string_view geometrySource = "");
	~Shader();
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;
	std::string ParseIncludes(std::string_view source, std::unordered_set<std::string>& includedFiles);
	void SetBool(std::string_view name, bool value) const;
	void SetInt(std::string_view name, i32 value) const;
	void SetFloat(std::string_view name, f32 value) const;
	void SetVector2(std::string_view name, Math::Vector2 const& value) const;
	void SetVector3(std::string_view name, Math::Vector3 const& value) const;
	void SetVector4(std::string_view name, Math::Vector4 const& value) const;
	void SetMatrix4(std::string_view name, Math::Matrix4x4 const& matrix) const;
	void TrySetBool(std::string_view name, bool value) const;
	void TrySetInt(std::string_view name, i32 value) const;
	void TrySetFloat(std::string_view name, f32 value) const;
	void TrySetVector2(std::string_view name, Math::Vector2 const& value) const;
	void TrySetVector3(std::string_view name, Math::Vector3 const& value) const;
	void TrySetVector4(std::string_view name, Math::Vector4 const& value) const;
	void TrySetMatrix4(std::string_view name, Math::Matrix4x4 const& matrix) const;
	[[nodiscard]] i32 TryGetUniformLocation(std::string_view name) const;
private:
	mutable std::unordered_map<std::string, i32, StringHash, std::equal_to<>> m_UniformLocationCache;
	void Use() const;
	static bool LogCompileErrors(u32 shader, Type type);
	[[nodiscard]] i32 GetUniformLocation(std::string_view name) const;
};
}
