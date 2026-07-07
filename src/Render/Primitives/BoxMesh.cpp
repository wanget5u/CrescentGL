#include "Render/Primitives/BoxMesh.h"

namespace Crescent::Render {

BoxMesh::BoxMesh(f32 const width, f32 const height, f32 const depth)
	: Mesh(VertexLayout::CreatePosNormalUV())
	, m_Size(width, height, depth) {
	BoxMesh::GenerateGeometry();
}

BoxMesh::BoxMesh(Math::Vector3 const& size)
	: Mesh(VertexLayout::CreatePosNormalUV())
	, m_Size(size) {
	BoxMesh::GenerateGeometry();
}

void BoxMesh::SetSize(f32 const width, f32 const height, f32 const depth) {
	m_Size = Math::Vector3(width, height, depth);
	BoxMesh::GenerateGeometry();
}

void BoxMesh::SetSize(Math::Vector3 const& size) {
	m_Size = size;
	BoxMesh::GenerateGeometry();
}

void BoxMesh::GenerateGeometry() {
	// Vertices
	DynamicList<f32> vertices;
	vertices.ResizeUninitialized(24 * 8); // (3 Pos + 3 Normal + 2 UV)
	// Indices
	DynamicList<u32> indices;
	indices.ResizeUninitialized(36); // 6 faces * 6 indices
	const f32 halfWidth  = m_Size.x;
	const f32 halfHeight = m_Size.y;
	const f32 halfDepth  = m_Size.z;
	size_t currentFloatIndex = 0;

	// 1. Front Face (-Z)
	WriteVertex(vertices, currentFloatIndex, -halfWidth, -halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f); // 0
	WriteVertex(vertices, currentFloatIndex,  halfWidth, -halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f); // 1
	WriteVertex(vertices, currentFloatIndex,  halfWidth,  halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f); // 2
	WriteVertex(vertices, currentFloatIndex, -halfWidth,  halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f); // 3

	// 2. Back Face (+Z)
	WriteVertex(vertices, currentFloatIndex, -halfWidth, -halfHeight,  halfDepth,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f); // 4
	WriteVertex(vertices, currentFloatIndex,  halfWidth, -halfHeight,  halfDepth,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f); // 5
	WriteVertex(vertices, currentFloatIndex,  halfWidth,  halfHeight,  halfDepth,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f); // 6
	WriteVertex(vertices, currentFloatIndex, -halfWidth,  halfHeight,  halfDepth,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f); // 7

	// 3. Left Face (-X)
	WriteVertex(vertices, currentFloatIndex, -halfWidth,  halfHeight,  halfDepth, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f); // 8
	WriteVertex(vertices, currentFloatIndex, -halfWidth,  halfHeight, -halfDepth, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f); // 9
	WriteVertex(vertices, currentFloatIndex, -halfWidth, -halfHeight, -halfDepth, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f); // 10
	WriteVertex(vertices, currentFloatIndex, -halfWidth, -halfHeight,  halfDepth, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f); // 11

	// 4. Right Face (+X)
	WriteVertex(vertices, currentFloatIndex,  halfWidth,  halfHeight,  halfDepth,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f); // 12
	WriteVertex(vertices, currentFloatIndex,  halfWidth,  halfHeight, -halfDepth,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f); // 13
	WriteVertex(vertices, currentFloatIndex,  halfWidth, -halfHeight, -halfDepth,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f); // 14
	WriteVertex(vertices, currentFloatIndex,  halfWidth, -halfHeight,  halfDepth,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f); // 15

	// 5. Bottom Face (-Y)
	WriteVertex(vertices, currentFloatIndex, -halfWidth, -halfHeight, -halfDepth,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f); // 16
	WriteVertex(vertices, currentFloatIndex,  halfWidth, -halfHeight, -halfDepth,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f); // 17
	WriteVertex(vertices, currentFloatIndex,  halfWidth, -halfHeight,  halfDepth,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f); // 18
	WriteVertex(vertices, currentFloatIndex, -halfWidth, -halfHeight,  halfDepth,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f); // 19

	// 6. Top Face (+Y)
	WriteVertex(vertices, currentFloatIndex, -halfWidth,  halfHeight, -halfDepth,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f); // 20
	WriteVertex(vertices, currentFloatIndex,  halfWidth,  halfHeight, -halfDepth,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f); // 21
	WriteVertex(vertices, currentFloatIndex,  halfWidth,  halfHeight,  halfDepth,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f); // 22
	WriteVertex(vertices, currentFloatIndex, -halfWidth,  halfHeight,  halfDepth,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f); // 23

	// 2 triangles per face: (0, 1, 2 | 0, 2, 3)
	size_t currentIndexIndex = 0;
	for (u32 face = 0; face < 6; ++face) {
		const u32 startVertex = face * 4;
		indices[currentIndexIndex++] = startVertex + 0;
		indices[currentIndexIndex++] = startVertex + 1;
		indices[currentIndexIndex++] = startVertex + 2;
		indices[currentIndexIndex++] = startVertex + 0;
		indices[currentIndexIndex++] = startVertex + 2;
		indices[currentIndexIndex++] = startVertex + 3;
	}

	UploadData(vertices, indices);
}

}
