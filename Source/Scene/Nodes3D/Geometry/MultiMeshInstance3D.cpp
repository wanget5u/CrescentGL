#include "../../../../Include/Scene/Nodes3D/Geometry/MultiMeshInstance3D.h"

#include <glad/glad.h>

#include "Render/BatchRenderer.h"
#include "Render/Material/Material.h"
#include "Scene/Tree.h"

namespace Crescent::Scene {

MultiMeshInstance3D::MultiMeshInstance3D() {
	glGenVertexArrays(1, &m_InstanceVAO);
	glGenBuffers(1, &m_InstanceVBO);
}

MultiMeshInstance3D::MultiMeshInstance3D(std::shared_ptr<Asset::Mesh> meshAsset, std::shared_ptr<Render::Material> material) {
	m_MeshAsset = std::move(meshAsset);
	m_Material = std::move(material);
}

void MultiMeshInstance3D::OnTreeEnter() {
	InstancedVisual3D::OnTreeEnter();
}

void MultiMeshInstance3D::OnTreeExit() {
	InstancedVisual3D::OnTreeExit();
}

void MultiMeshInstance3D::SetMesh(std::shared_ptr<Render::Mesh> proceduralMesh) noexcept {
	GeometryInstance3D::SetMesh(std::move(proceduralMesh));
	m_InstanceVAODirty = true;
}

void MultiMeshInstance3D::SetTransforms(DynamicList<Math::Matrix4x4> const& transforms) {
	m_Transforms.Clear();
	m_Transforms.Append(transforms.GetData(), transforms.GetSize());
	UploadTransformsToGPU();
}

void MultiMeshInstance3D::SetTransforms(DynamicList<Math::Matrix4x4> &&transforms) {
	m_Transforms = std::move(transforms);
	UploadTransformsToGPU();
}

void MultiMeshInstance3D::AddInstance(Math::Matrix4x4 const& transform) {
	m_Transforms.PushBack(transform);
	m_TransformsDirty = true;
}

void MultiMeshInstance3D::UpdateInstance(const size_t index, Math::Matrix4x4 const& transform) {
	if (index < m_Transforms.GetSize()) {
		m_Transforms[index] = transform;
		m_TransformsDirty = true;
	}
}

DynamicList<Math::Matrix4x4>& MultiMeshInstance3D::GetTransforms() noexcept {
	return m_Transforms;
}

const DynamicList<Math::Matrix4x4>& MultiMeshInstance3D::GetTransforms() const noexcept {
	return m_Transforms;
}

void MultiMeshInstance3D::UploadTransformsToGPU() {
	m_InstanceCount = static_cast<u32>(m_Transforms.GetSize());
	if (m_InstanceCount == 0 || m_InstanceVBO == 0) {
		m_TransformsDirty = false;
		return;
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
	const size_t requiredBytes = m_Transforms.GetSizeInBytes();
	if (m_InstanceBufferCapacity < requiredBytes) {
		glBufferData(GL_ARRAY_BUFFER, requiredBytes, m_Transforms.GetData(), GL_DYNAMIC_DRAW);
		m_InstanceBufferCapacity = requiredBytes;
	}
	else {
		glBufferSubData(GL_ARRAY_BUFFER, 0, requiredBytes, m_Transforms.GetData());
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_TransformsDirty = false;
}

void MultiMeshInstance3D::DrawInstanced() const {
	Render::Mesh* gpuMesh = GetMesh();
	if (m_InstanceCount == 0) {
		return;
	}
	if (m_TransformsDirty == true) {
		const_cast<MultiMeshInstance3D*>(this)->UploadTransformsToGPU();
	}
	if (m_InstanceVAODirty == true) {
		CheckAndBuildInstanceVAO();
	}
	glBindVertexArray(m_InstanceVAO);
	if (gpuMesh->GetIndexCount() > 0) {
		glDrawElementsInstanced(
			GL_TRIANGLES,
			static_cast<i32>(gpuMesh->GetIndexCount()),
			GL_UNSIGNED_INT,
			nullptr,
			static_cast<i32>(m_InstanceCount)
		);
	}
	else {
		glDrawArraysInstanced(
			GL_TRIANGLES,
			0,
			static_cast<i32>(gpuMesh->GetVertexCount()),
			static_cast<i32>(m_InstanceCount)
		);
	}
	glBindVertexArray(0);
}

void MultiMeshInstance3D::Draw() const {
	DrawInstanced();
}

void MultiMeshInstance3D::CheckAndBuildInstanceVAO() const {
	Render::Mesh* gpuMesh = GetMesh();
	if (gpuMesh->GetVBO() == 0) {
		return;
	}
	glBindVertexArray(m_InstanceVAO);
	glBindBuffer(GL_ARRAY_BUFFER, gpuMesh->GetVBO());
	Render::Mesh::VertexLayout const& layout = gpuMesh->GetLayout();
	for (Render::Mesh::VertexAttribute const& attribute : layout.VertexAttributes) {
		glEnableVertexAttribArray(attribute.Location);
		glVertexAttribPointer(
			attribute.Location,
			static_cast<i32>(attribute.ComponentCount),
			attribute.Type,
			GL_FALSE,
			static_cast<i32>(layout.Stride),
			reinterpret_cast<void*>(static_cast<uintptr_t>(attribute.Offset))
		);
	}
	if (gpuMesh->GetEBO() != 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->GetEBO());
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
	size_t vector4Size = sizeof(f32) * 4;
	for (u8 a = 0; a < 4; ++a) {
		glEnableVertexAttribArray(3 + a);
		glVertexAttribPointer(
			3 + a,
			4,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Math::Matrix4x4),
			reinterpret_cast<void*>(a * vector4Size)
		);
		glVertexAttribDivisor(3 + a, 1);
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_InstanceVAODirty = false;
	Log::Info("MultiMeshInstance3D: Built m_InstanceVAO ({}) "
		   "referencing base Mesh VBO ({}).", m_InstanceVAO, gpuMesh->GetVBO());
}

}
