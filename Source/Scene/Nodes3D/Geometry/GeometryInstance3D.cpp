#include "Scene/Nodes3D/Geometry/GeometryInstance3D.h"

#include "Asset/AssetType.h"
#include "Render/Mesh.h"
#include "Render/Material/Material.h"
#include "Render/Primitives/BoxMesh.h"

namespace Crescent {

void GeometryInstance3D::OnTreeEnter() {
	VisualInstance3D::OnTreeEnter();
}

void GeometryInstance3D::OnTreeExit() {
	VisualInstance3D::OnTreeExit();
}

void GeometryInstance3D::SetMeshAsset(std::shared_ptr<MeshAsset> meshAsset) noexcept {
	m_MeshAsset = std::move(meshAsset);
}

std::shared_ptr<MeshAsset> GeometryInstance3D::GetMeshAsset() const noexcept {
	return m_MeshAsset;
}

void GeometryInstance3D::SetMaterialOverride(std::shared_ptr<Material> material) noexcept {
	m_Material = std::move(material);
}

std::shared_ptr<Material> GeometryInstance3D::GetMaterialOverride() const noexcept {
	if (m_Material == nullptr) {
		return Material::GetDefaultMaterial();
	}
	return m_Material;
}

void GeometryInstance3D::SetMaterial(std::shared_ptr<Material> material) noexcept {
	m_Material = std::move(material);
}

std::shared_ptr<Material> GeometryInstance3D::GetMaterial() const noexcept {
	if (m_Material == nullptr) {
		return Material::GetDefaultMaterial();
	}
	return m_Material;
}

Mesh* GeometryInstance3D::GetMesh() const noexcept {
	if (m_MeshAsset && m_MeshAsset->IsReady && m_MeshAsset->MeshObject) {
		return m_MeshAsset->MeshObject.get();
	}
	return nullptr;
}

void GeometryInstance3D::SetMesh(std::shared_ptr<Mesh> proceduralMesh) noexcept {
	if (proceduralMesh == nullptr) {
		m_MeshAsset = nullptr;
		return;
	}
	m_MeshAsset = std::make_shared<MeshAsset>();
	m_MeshAsset->FilePath = "Procedural_" + std::to_string(m_ID);
	m_MeshAsset->Type = AssetType::Mesh;
	m_MeshAsset->MeshObject = std::move(proceduralMesh);
	m_MeshAsset->IsReady = true;
}

}
