#pragma once
#include <glad/glad.h>
#include "Core/Core.h"

namespace Crescent {

struct Mesh {
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Lifecycle
	Mesh(const f32* vertices, u32 vertexDataSize, const u32* indices, u32 indexCount);
	~Mesh();
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&& other) noexcept;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&& other) noexcept;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// State Management
	void Bind() const { glBindVertexArray(m_VAO); }
	void Unbind() const { glBindVertexArray(0); }
	void Draw() const {
		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, static_cast<i32>(m_IndexCount), GL_UNSIGNED_INT, nullptr);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Getters
	[[nodiscard]] u32 GetIndexCount() const { return m_IndexCount; }

private:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Mesh Properties
	u32 m_VAO{0};
	u32 m_VBO{0};
	u32 m_EBO{0};
	u32 m_IndexCount{0};
};

}
