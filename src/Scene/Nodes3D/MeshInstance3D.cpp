#include "../../../include/Scene/Nodes3D/MeshInstance3D.h"

#include "Render/BatchRenderer.h"
#include "Render/Material/Material.h"
#include "Scene/Tree.h"

namespace Crescent::Scene {

MeshInstance3D::MeshInstance3D(std::shared_ptr<Asset::Mesh> meshAsset, std::shared_ptr<Render::Material> material)
	: m_MeshAsset(std::move(meshAsset)), m_Material(std::move(material)) {}

void MeshInstance3D::OnTreeEnter() {
	Node3D::OnTreeEnter();
	if (m_Tree != nullptr) {
		m_Tree->GetBatchRenderer()->RegisterMesh(this);
	}
}

void MeshInstance3D::OnTreeExit() {
	Node3D::OnTreeExit();
	if (m_Tree != nullptr) {
		m_Tree->GetBatchRenderer()->UnregisterMesh(this);
	}
}

void MeshInstance3D::SetMeshAsset(std::shared_ptr<Asset::Mesh> meshAsset) noexcept {
	m_MeshAsset = std::move(meshAsset);
}

std::shared_ptr<Asset::Mesh> MeshInstance3D::GetMeshAsset() const noexcept {
	return m_MeshAsset;
}

void MeshInstance3D::SetMaterial(std::shared_ptr<Render::Material> material) noexcept {
	m_Material = std::move(material);
}

std::shared_ptr<Render::Material> MeshInstance3D::GetMaterial() const noexcept {
	return m_Material;
}

Render::Mesh * MeshInstance3D::GetMesh() const noexcept {
	if (m_MeshAsset != nullptr && m_MeshAsset->IsReady == true) {
		return m_MeshAsset->MeshObject.get();
	}
	return nullptr;
}

void MeshInstance3D::SetMesh(std::shared_ptr<Render::Mesh> proceduralMesh) noexcept {
	if (proceduralMesh == nullptr) {
		m_MeshAsset = nullptr;
		return;
	}
	m_MeshAsset = std::make_shared<Asset::Mesh>();
	m_MeshAsset->FilePath = "Procedural_" + std::to_string(m_ID);
	m_MeshAsset->Type = Asset::Type::Mesh;
	m_MeshAsset->MeshObject = std::move(proceduralMesh);
	m_MeshAsset->IsReady = true;
}

void MeshInstance3D::Draw(Math::Matrix4x4 const& worldMatrix) const {
	Render::Mesh* gpuMesh = GetMesh();
	if (gpuMesh == nullptr || m_Material == nullptr) {
		return;
	}
	m_Material->SetMatrix4("u_Model", worldMatrix);
	gpuMesh->Bind();
	gpuMesh->Draw();
}

}
