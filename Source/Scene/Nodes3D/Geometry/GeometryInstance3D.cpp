#include "../../../../Include/Scene/Nodes3D/Geometry/GeometryInstance3D.h"

#include "../../../../Include/Asset/AssetType.h"
#include "Render/Mesh.h"
#include "Render/Material/Material.h"

namespace Crescent::Scene {

void GeometryInstance3D::OnTreeEnter() {
	VisualInstance3D::OnTreeEnter();
}

void GeometryInstance3D::OnTreeExit() {
	VisualInstance3D::OnTreeExit();
}

void GeometryInstance3D::SetMeshAsset(std::shared_ptr<Asset::Mesh> meshAsset) noexcept {
	m_MeshAsset = std::move(meshAsset);
}

std::shared_ptr<Asset::Mesh> GeometryInstance3D::GetMeshAsset() const noexcept {
	return m_MeshAsset;
}

void GeometryInstance3D::SetMaterialOverride(std::shared_ptr<Render::Material> material) noexcept {
	m_Material = std::move(material);
}

std::shared_ptr<Render::Material> GeometryInstance3D::GetMaterialOverride() const noexcept {
	if (m_Material == nullptr) {
		return Render::Material::GetDefaultMaterial();
	}
	return m_Material;
}

void GeometryInstance3D::SetMaterial(std::shared_ptr<Render::Material> material) noexcept {
	m_Material = std::move(material);
}

std::shared_ptr<Render::Material> GeometryInstance3D::GetMaterial() const noexcept {
	return m_Material;
}

Render::Mesh * GeometryInstance3D::GetMesh() const noexcept {
	if (m_MeshAsset && m_MeshAsset->IsReady && m_MeshAsset->MeshObject) {
		return m_MeshAsset->MeshObject.get();
	}
	return nullptr;
}

void GeometryInstance3D::SetMesh(std::shared_ptr<Render::Mesh> proceduralMesh) noexcept {
	if (proceduralMesh == nullptr) {
		m_MeshAsset = nullptr;
		return;
	}
	m_MeshAsset = std::make_shared<Asset::Mesh>();
	m_MeshAsset->FilePath = "Procedural_" + std::to_string(m_ID);
	m_MeshAsset->Type = Asset::AssetType::Mesh;
	m_MeshAsset->MeshObject = std::move(proceduralMesh);
	m_MeshAsset->IsReady = true;
}

}
