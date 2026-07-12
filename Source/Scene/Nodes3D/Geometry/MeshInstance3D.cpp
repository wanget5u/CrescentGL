#include "Scene/Nodes3D/Geometry/MeshInstance3D.h"

#include "Render/BatchRenderer.h"
#include "Render/Material/Material.h"
#include "Scene/Tree.h"

namespace Crescent {

MeshInstance3D::MeshInstance3D(std::shared_ptr<MeshAsset> meshAsset, std::shared_ptr<Material> material) {
	m_MeshAsset = std::move(meshAsset);
	m_Material = std::move(material);
}

void MeshInstance3D::OnTreeEnter() {
	GeometryInstance3D::OnTreeEnter();
	m_Tree->GetBatchRenderer()->Register(this);
}

void MeshInstance3D::OnTreeExit() {
	GeometryInstance3D::OnTreeExit();
	m_Tree->GetBatchRenderer()->Unregister(this);
}

void MeshInstance3D::Draw(Math::Matrix4x4 const& worldMatrix) const {
	Mesh* gpuMesh = GetMesh();
	if (gpuMesh == nullptr || m_Material == nullptr) {
		return;
	}
	m_Material->SetMatrix4("u_Model", worldMatrix);
	gpuMesh->Bind();
	gpuMesh->Draw();
}

}
