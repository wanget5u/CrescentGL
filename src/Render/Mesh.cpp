#include "Render/Mesh.h"
#include <utility>

namespace Crescent {

Mesh::Mesh(const f32* vertices, u32 const vertexDataSize, const u32* indices, u32 const indexCount): m_IndexCount(indexCount) {
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(u32), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), reinterpret_cast<void*>(3 * sizeof(f32)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), reinterpret_cast<void*>(6 * sizeof(f32)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Mesh::~Mesh() {
	if (m_VAO == 0) {
		return;
	}
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
}

Mesh::Mesh(Mesh&& other) noexcept
	: m_VAO(std::exchange(other.m_VAO, 0))
	, m_VBO(std::exchange(other.m_VBO, 0))
	, m_EBO(std::exchange(other.m_EBO, 0))
	, m_IndexCount(std::exchange(other.m_IndexCount, 0)) {}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
	if (this != &other) {
		if (m_VAO != 0) {
			glDeleteVertexArrays(1, &m_VAO);
			glDeleteBuffers(1, &m_VBO);
			glDeleteBuffers(1, &m_EBO);
		}
		m_VAO = std::exchange(other.m_VAO, 0);
		m_VBO = std::exchange(other.m_VBO, 0);
		m_EBO = std::exchange(other.m_EBO, 0);
		m_IndexCount = std::exchange(other.m_IndexCount, 0);
	}
	return *this;
}

}
