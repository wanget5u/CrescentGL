#include "Scene/Nodes3D/Geometry/MultiMeshInstance3D.h"

#include <glad/glad.h>

#include "Render/BatchRenderer.h"
#include "Render/SceneRenderData.h"
#include "Render/Material/Material.h"
#include "Render/GPUDisposalQueue.h"

namespace Crescent {

MultiMeshInstance3D::MultiMeshInstance3D() {
	glGenBuffers(1, &m_InstanceSSBO);
	AddInstance(Math::Matrix4x4::GetIdentity());
}

MultiMeshInstance3D::MultiMeshInstance3D(std::shared_ptr<MeshAsset> meshAsset, std::shared_ptr<Material> material)
	: MultiMeshInstance3D() {
	m_MeshAsset = std::move(meshAsset);
	m_Material = std::move(material);
}

MultiMeshInstance3D::~MultiMeshInstance3D() {
	if (m_InstanceSSBO != 0) {
		GPUDisposalQueue::SubmitBufferForDeletion(m_InstanceSSBO);
		m_InstanceSSBO = 0;
	}
}

void MultiMeshInstance3D::OnTreeEnter() {
	InstancedVisual3D::OnTreeEnter();
}

void MultiMeshInstance3D::OnTreeExit() {
	InstancedVisual3D::OnTreeExit();
}

void MultiMeshInstance3D::SetMesh(std::shared_ptr<Mesh> proceduralMesh) noexcept {
	GeometryInstance3D::SetMesh(std::move(proceduralMesh));
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
	if (m_InstanceCount == 0 || m_InstanceSSBO == 0) {
		m_TransformsDirty = false;
		return;
	}

	DynamicList<GPU::InstanceRenderData> gpuInstances{};
	gpuInstances.Reserve(m_Transforms.GetSize());
	for (size_t a = 0; a < m_Transforms.GetSize(); ++a) {
		GPU::InstanceRenderData instance{};
		instance.WorldMatrix = m_Transforms[a];
		instance.NormalMatrix = Math::Matrix4x4::Transpose(Math::Matrix4x4::Inverse(m_Transforms[a]));
		instance.MaterialIndex = 0;
		instance.ObjectID = static_cast<u32>(a);
		gpuInstances.PushBack(instance);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_InstanceSSBO);
	const size_t requiredBytes = gpuInstances.GetSizeInBytes();
	if (m_InstanceBufferCapacity < requiredBytes) {
		glBufferData(GL_SHADER_STORAGE_BUFFER, requiredBytes, gpuInstances.GetData(), GL_DYNAMIC_DRAW);
		m_InstanceBufferCapacity = requiredBytes;
	}
	else {
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, requiredBytes, gpuInstances.GetData());
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	m_TransformsDirty = false;
}

void MultiMeshInstance3D::DrawInstanced() const {
	Mesh* gpuMesh = GetMesh();
	if (gpuMesh == nullptr || gpuMesh->GetVAO() == 0 || m_InstanceCount == 0) {
		return;
	}
	if (m_TransformsDirty == true) {
		const_cast<MultiMeshInstance3D*>(this)->UploadTransformsToGPU();
	}
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_InstanceSSBO);
	gpuMesh->DrawInstanced(m_InstanceCount);
}

void MultiMeshInstance3D::Draw() const {
	DrawInstanced();
}

}
