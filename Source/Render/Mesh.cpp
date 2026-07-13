#include "Render/Mesh.h"

#include <glad/glad.h>

#include "Render/RenderStats.h"
#include "Render/GPUDisposalQueue.h"

namespace Crescent {
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
	, m_IndexCount(std::exchange(other.m_IndexCount, 0))
	, m_VertexCount(std::exchange(other.m_VertexCount, 0))
	, m_VBOCapacity(std::exchange(other.m_VBOCapacity, 0))
	, m_EBOCapacity(std::exchange(other.m_EBOCapacity, 0)) {}

Mesh::~Mesh() {
	if (m_VAO == 0) {
		return;
	}
	GPUDisposalQueue::SubmitVertexArrayForDeletion(m_VAO);
	GPUDisposalQueue::SubmitBufferForDeletion(m_VBO);
	GPUDisposalQueue::SubmitBufferForDeletion(m_EBO);
	m_VAO = 0;
	m_VBO = 0;
	m_EBO = 0;
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
	RenderStats::Instance().DrawCalls++;
	RenderStats::Instance().TriangleCount += (m_IndexCount / 3);
	RenderStats::Instance().VertexCount += m_VertexCount;
}

void Mesh::DrawInstanced(const u32 instanceCount) const {
	if (instanceCount == 0) {
		return;
	}
	Bind();
	if (m_IndexCount) {
		glDrawElementsInstanced(GL_TRIANGLES, static_cast<i32>(m_IndexCount), GL_UNSIGNED_INT, nullptr, instanceCount);
		RenderStats::Instance().TriangleCount += (m_IndexCount / 3) * instanceCount;
		RenderStats::Instance().VertexCount += m_VertexCount * instanceCount;
	}
	else {
		glDrawArraysInstanced(GL_TRIANGLES, 0, static_cast<i32>(m_VertexCount), instanceCount);
		RenderStats::Instance().TriangleCount += (m_VertexCount / 3) * instanceCount;
		RenderStats::Instance().VertexCount += m_VertexCount * instanceCount;
	}
	Unbind();
	RenderStats::Instance().DrawCalls++;
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
	
void Mesh::UploadData(const f32* vertices, u32 vertexDataSize, const u32* indices, const u32 indexCount) {
	if (m_VAO == 0) {
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);
	}
	m_IndexCount = indexCount;
	if (m_Layout.Stride > 0) {
		m_VertexCount = vertexDataSize / m_Layout.Stride;
	} else {
		m_VertexCount = 0;
	}
	glBindVertexArray(m_VAO);
	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	if (m_VBO != 0 && m_VBOCapacity >= vertexDataSize) {
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexDataSize, vertices);
	} else {
		glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertices, GL_DYNAMIC_DRAW);
		m_VBOCapacity = vertexDataSize;
	}
	// EBO
	const u32 indexDataSize = indexCount * sizeof(u32);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	if (m_EBO != 0 && m_EBOCapacity >= indexDataSize) {
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexDataSize, indices);
	} else {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indices, GL_DYNAMIC_DRAW);
		m_EBOCapacity = indexDataSize;
	}
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

u32 Mesh::GetIndexCount() const {
	return m_IndexCount;
}

u32 Mesh::GetVertexCount() const {
	return  m_VertexCount;
}

const Mesh::VertexLayout& Mesh::GetLayout() const {
	return m_Layout;
}

u32 Mesh::GetVAO() const noexcept {
	return m_VAO;
}
	
u32 Mesh::GetVBO() const noexcept {
	return m_VBO;
}

u32 Mesh::GetEBO() const noexcept {
	return m_EBO;
}

}
