#include "Render/Primitives/BoxMesh.h"

namespace Crescent {

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
	DynamicList<f32> vertices;
	vertices.ResizeUninitialized(24 * 8);

	DynamicList<u32> indices;
	indices.ResizeUninitialized(36);

	const f32 halfWidth  = m_Size.x * 0.5f;
	const f32 halfHeight = m_Size.y * 0.5f;
	const f32 halfDepth  = m_Size.z * 0.5f;
	size_t currentFloatIndex = 0;

    WriteVertex(vertices, currentFloatIndex,  halfWidth, -halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f); // 0 (BL)
    WriteVertex(vertices, currentFloatIndex, -halfWidth, -halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f); // 1 (BR)
    WriteVertex(vertices, currentFloatIndex, -halfWidth,  halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f); // 2 (TR)
    WriteVertex(vertices, currentFloatIndex,  halfWidth,  halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f); // 3 (TL)

    WriteVertex(vertices, currentFloatIndex, -halfWidth, -halfHeight,  halfDepth,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f); // 4 (BL)
    WriteVertex(vertices, currentFloatIndex,  halfWidth, -halfHeight,  halfDepth,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f); // 5 (BR)
    WriteVertex(vertices, currentFloatIndex,  halfWidth,  halfHeight,  halfDepth,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f); // 6 (TR)
    WriteVertex(vertices, currentFloatIndex, -halfWidth,  halfHeight,  halfDepth,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f); // 7 (TL)

	WriteVertex(vertices, currentFloatIndex, -halfWidth, -halfHeight, -halfDepth, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f); // 8 (BL)
    WriteVertex(vertices, currentFloatIndex, -halfWidth, -halfHeight,  halfDepth, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f); // 9 (BR)
    WriteVertex(vertices, currentFloatIndex, -halfWidth,  halfHeight,  halfDepth, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f); // 10 (TR)
    WriteVertex(vertices, currentFloatIndex, -halfWidth,  halfHeight, -halfDepth, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f); // 11 (TL)

    WriteVertex(vertices, currentFloatIndex,  halfWidth, -halfHeight,  halfDepth,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f); // 12 (BL)
    WriteVertex(vertices, currentFloatIndex,  halfWidth, -halfHeight, -halfDepth,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f); // 13 (BR)
    WriteVertex(vertices, currentFloatIndex,  halfWidth,  halfHeight, -halfDepth,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f); // 14 (TR)
    WriteVertex(vertices, currentFloatIndex,  halfWidth,  halfHeight,  halfDepth,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f); // 15 (TL)

	WriteVertex(vertices, currentFloatIndex, -halfWidth, -halfHeight, -halfDepth,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f); // 16 (BL)
    WriteVertex(vertices, currentFloatIndex,  halfWidth, -halfHeight, -halfDepth,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f); // 17 (BR)
    WriteVertex(vertices, currentFloatIndex,  halfWidth, -halfHeight,  halfDepth,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f); // 18 (TR)
    WriteVertex(vertices, currentFloatIndex, -halfWidth, -halfHeight,  halfDepth,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f); // 19 (TL)

    WriteVertex(vertices, currentFloatIndex, -halfWidth,  halfHeight,  halfDepth,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f); // 20 (BL)
    WriteVertex(vertices, currentFloatIndex,  halfWidth,  halfHeight,  halfDepth,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f); // 21 (BR)
    WriteVertex(vertices, currentFloatIndex,  halfWidth,  halfHeight, -halfDepth,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f); // 22 (TR)
    WriteVertex(vertices, currentFloatIndex, -halfWidth,  halfHeight, -halfDepth,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f); // 23 (TL)

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
