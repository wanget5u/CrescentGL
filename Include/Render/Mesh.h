#pragma once
#include "Core/Core.h"
#include "Collection/Collections.h"
#include "Math/Vector/Vector2.h"
#include "Math/Vector/Vector3.h"

namespace Crescent::Render {
struct Material;
struct Mesh {
	struct Vertex {
		Math::Vector3 Position{};
		Math::Vector3 Normal{};
		Math::Vector2 TexCoord{};
	};
	struct VertexAttribute {
		u32 Location{};
		u32 ComponentCount{};
		u32 Type{};
		u32 Offset{};
	};
	struct VertexLayout {
		DynamicList<VertexAttribute> VertexAttributes{};
		u32 Stride{0};
		[[nodiscard]] static VertexLayout CreatePosNormalUV();
		[[nodiscard]] static VertexLayout CreatePosUV();
	};
	Mesh(
		const f32* vertices,
		u32 vertexDataSize,
		const u32* indices,
		u32 indexCount,
		VertexLayout&& layout = VertexLayout::CreatePosNormalUV()
	);
	Mesh(Mesh&& other) noexcept;
	virtual ~Mesh();
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	void Bind() const;
	void Unbind() const;
	void Draw() const;
	void DrawInstanced(u32 instanceCount) const;
	void UploadData(const DynamicList<Vertex>& vertices, const DynamicList<u32>& indices);
	void UploadData(const DynamicList<f32>& vertices, const DynamicList<u32>& indices);
	void UploadData(const f32* vertices, u32 vertexDataSize, const u32* indices, u32 indexCount);
	[[nodiscard]] u32 GetIndexCount() const;
	[[nodiscard]] u32 GetVertexCount() const;
	[[nodiscard]] const VertexLayout& GetLayout() const;
	[[nodiscard]] u32 GetVAO() const noexcept;
	[[nodiscard]] u32 GetVBO() const noexcept;
	[[nodiscard]] u32 GetEBO() const noexcept;
protected:
	static void WriteVertex(
		DynamicList<f32>& vertices,
		size_t& index,
		f32 const positionX,
		f32 const positionY,
		f32 const positionZ,
		f32 const normalX,
		f32 const normalY,
		f32 const normalZ,
		f32 const texCoordU,
		f32 const texCoordV) {
		vertices[index++] = positionX;
		vertices[index++] = positionY;
		vertices[index++] = positionZ;
		vertices[index++] = normalX;
		vertices[index++] = normalY;
		vertices[index++] = normalZ;
		vertices[index++] = texCoordU;
		vertices[index++] = texCoordV;
	}
	Mesh();
	explicit Mesh(VertexLayout&& layout);
	virtual void GenerateGeometry() {}
private:
	VertexLayout              m_Layout     {};
	u32 					  m_VAO		   {0};
	u32 					  m_VBO		   {0};
	u32 					  m_EBO		   {0};
	u32 					  m_IndexCount {0};
	u32						  m_VertexCount{0};
	size_t                    m_VBOCapacity{0};
	size_t                    m_EBOCapacity{0};
};

}
