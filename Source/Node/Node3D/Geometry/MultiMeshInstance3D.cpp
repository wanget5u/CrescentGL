#include "Node/Node3D/Geometry/MultiMeshInstance3D.h"

#include "Render/Material/Material.h"

namespace Crescent {

MultiMeshInstance3D::MultiMeshInstance3D() {
	AddInstance(Math::Matrix4x4::GetIdentity());
}

MultiMeshInstance3D::MultiMeshInstance3D(std::shared_ptr<MeshAsset> meshAsset, std::shared_ptr<Material> material)
	: MultiMeshInstance3D() {
	m_MeshAsset = std::move(meshAsset);
	m_Material = std::move(material);
}

MultiMeshInstance3D::~MultiMeshInstance3D() = default;

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
	m_InstanceCount = static_cast<u32>(m_Transforms.GetSize());
}

void MultiMeshInstance3D::SetTransforms(DynamicList<Math::Matrix4x4> &&transforms) {
	m_Transforms = std::move(transforms);
	m_InstanceCount = static_cast<u32>(m_Transforms.GetSize());
}

void MultiMeshInstance3D::AddInstance(Math::Matrix4x4 const& transform) {
	m_Transforms.PushBack(transform);
	m_InstanceCount = static_cast<u32>(m_Transforms.GetSize());
}

void MultiMeshInstance3D::UpdateInstance(const size_t index, Math::Matrix4x4 const& transform) {
	if (index < m_Transforms.GetSize()) {
		m_Transforms[index] = transform;
	}
}

DynamicList<Math::Matrix4x4>& MultiMeshInstance3D::GetTransforms() noexcept {
	return m_Transforms;
}

const DynamicList<Math::Matrix4x4>& MultiMeshInstance3D::GetTransforms() const noexcept {
	return m_Transforms;
}

void MultiMeshInstance3D::DrawInstanced() const {
	Mesh* gpuMesh = GetMesh();
	if (gpuMesh == nullptr || gpuMesh->GetVAO() == 0 || m_Transforms.IsEmpty() == true) {
		return;
	}
	gpuMesh->DrawInstanced(static_cast<u32>(m_Transforms.GetSize()));
}

void MultiMeshInstance3D::Draw() const {
	DrawInstanced();
}

}
