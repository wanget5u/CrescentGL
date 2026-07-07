#include "Render/Mesh.h"

namespace Crescent::Render {
Mesh::VertexLayout Mesh::VertexLayout::CreatePosNormalUV() {
	VertexLayout layout{};
	layout.Stride = 8 * sizeof(f32);
	layout.VertexAttributes.PushBack({ 0, 3, GL_FLOAT, 0 });
	layout.VertexAttributes.PushBack({ 1, 3, GL_FLOAT, 3 * sizeof(f32) });
	layout.VertexAttributes.PushBack({ 2, 2, GL_FLOAT, 6 * sizeof(f32) });
	return layout;
}

Mesh::VertexLayout Mesh::VertexLayout::CreatePosUV() {
	VertexLayout layout{};
	layout.Stride = 5 * sizeof(f32);
	layout.VertexAttributes.PushBack({ 0, 3, GL_FLOAT, 0 });
	layout.VertexAttributes.PushBack({ 1, 2, GL_FLOAT, 3 * sizeof(f32) });
	return layout;
}

Mesh::Mesh() : m_Layout(VertexLayout::CreatePosNormalUV()) {
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);
}

Mesh::Mesh(VertexLayout&& layout) : m_Layout(std::move(layout)) {
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);
}

Mesh::Mesh(const f32 *vertices, const u32 vertexDataSize, const u32* indices, const u32 indexCount, VertexLayout&& layout)
	: m_Layout(std::move(layout)) {
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);
	UploadData(vertices, vertexDataSize, indices, indexCount);
}

Mesh::Mesh(Mesh&& other) noexcept
	: m_Layout(std::move(other.m_Layout))
	, m_VAO(std::exchange(other.m_VAO, 0))
	, m_VBO(std::exchange(other.m_VBO, 0))
	, m_EBO(std::exchange(other.m_EBO, 0))
	, m_IndexCount(std::exchange(other.m_IndexCount, 0)) {}

Mesh::~Mesh() {
	if (m_VAO == 0) {
		return;
	}
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
}

void Mesh::Bind() const {
	glBindVertexArray(m_VAO);
}

void Mesh::Unbind() const {
	glBindVertexArray(0);
}

void Mesh::Draw() const {
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, static_cast<i32>(m_IndexCount), GL_UNSIGNED_INT, nullptr);
}

void Mesh::SetMaterial(std::shared_ptr<Material> material) {
	m_Material = std::move(material);
}

std::shared_ptr<Material> Mesh::GetMaterial() const {
	return m_Material;
}

void Mesh::Render() const {
	if (m_Material != nullptr) {
		m_Material->Bind();
	}
	Draw();
}

void Mesh::UploadData(const DynamicList<Vertex>& vertices, const DynamicList<u32>& indices) {
	UploadData(
		reinterpret_cast<const f32*>(vertices.GetData()),
		static_cast<u32>(vertices.GetSizeInBytes()),
		indices.GetData(),
		static_cast<u32>(indices.GetSize())
	);
}

void Mesh::UploadData(const DynamicList<f32>& vertices, const DynamicList<u32>& indices) {
	UploadData(
		vertices.GetData(),
		static_cast<u32>(vertices.GetSizeInBytes()),
		indices.GetData(),
		static_cast<u32>(indices.GetSize())
	);
}
	
void Mesh::UploadData(const f32* vertices, u32 vertexDataSize, const u32* indices, u32 indexCount) {
	if (m_VAO == 0) {
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);
	}
	m_IndexCount = indexCount;
	glBindVertexArray(m_VAO);
	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertices, GL_STATIC_DRAW);
	// EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(u32), indices, GL_STATIC_DRAW);
	// Attribute Config
	for (const VertexAttribute& attribute : m_Layout.VertexAttributes) {
		glVertexAttribPointer(
			attribute.Location,
			static_cast<i32>(attribute.ComponentCount),
			attribute.Type,
			GL_FALSE,
			static_cast<i32>(m_Layout.Stride),
			reinterpret_cast<void*>(static_cast<uintptr_t>(attribute.Offset))
		);
		glEnableVertexAttribArray(attribute.Location);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

u32 Mesh::GetIndexCount() const { return m_IndexCount; }
const Mesh::VertexLayout & Mesh::GetLayout() const { return m_Layout; }
}
