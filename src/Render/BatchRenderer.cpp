#include "../../include/Render/BatchRenderer.h"

#include "Math/Matrix/Matrix4x4.h"
#include "Render/Material/Material.h"
#include "Scene/Nodes3D/Camera3D.h"
#include "Scene/Nodes3D/MeshInstance3D.h"
#include "Scene/Nodes3D/MultiMeshInstance3D.h"

namespace Crescent::Render {

void BatchRenderer::BeginBatchLoad() {
	m_IsBatchLoading = true;
}

void BatchRenderer::EndBatchLoad() {
	m_IsBatchLoading = false;
	if (m_StagedMeshInstances.IsEmpty() == false) {
		m_RegisteredMeshInstances.Reserve(m_RegisteredMeshInstances.GetSize() + m_StagedMeshInstances.GetSize());
		for (size_t a = 0; a < m_StagedMeshInstances.GetSize(); ++a) {
			m_RegisteredMeshInstances.PushBack(m_StagedMeshInstances[a]);
		}
		m_StagedMeshInstances.Clear();
	}
	if (m_StagedMultiMeshInstances.IsEmpty() == false) {
		m_RegisteredMultiMeshInstances.Reserve(m_RegisteredMultiMeshInstances.GetSize() + m_StagedMultiMeshInstances.GetSize());
		for (size_t a = 0; a < m_StagedMultiMeshInstances.GetSize(); ++a) {
			m_RegisteredMultiMeshInstances.PushBack(m_StagedMultiMeshInstances[a]);
		}
		m_StagedMultiMeshInstances.Clear();
	}
}

void BatchRenderer::BeginBatchUnload() {
	m_IsBatchUnloading = true;
}

void BatchRenderer::EndBatchUnload() {
	m_IsBatchUnloading = false;
	size_t writeIndex = 0;
	for (size_t readIndex = 0; readIndex < m_RegisteredMeshInstances.GetSize(); ++readIndex) {
		if (m_RegisteredMeshInstances[readIndex] != nullptr) {
			m_RegisteredMeshInstances[writeIndex] = m_RegisteredMeshInstances[readIndex];
			writeIndex++;
		}
	}
	while (m_RegisteredMeshInstances.GetSize() > writeIndex) {
		m_RegisteredMeshInstances.PopBack();
	}
	writeIndex = 0;
	for (size_t readIndex = 0; readIndex < m_RegisteredMultiMeshInstances.GetSize(); ++readIndex) {
		if (m_RegisteredMultiMeshInstances[readIndex] != nullptr) {
			m_RegisteredMultiMeshInstances[writeIndex] = m_RegisteredMultiMeshInstances[readIndex];
			writeIndex++;
		}
	}
	while (m_RegisteredMultiMeshInstances.GetSize() > writeIndex) {
		m_RegisteredMultiMeshInstances.PopBack();
	}
}

void BatchRenderer::RegisterMesh(Scene::MeshInstance3D* meshInstance) {
	if (meshInstance == nullptr) {
		return;
	}
	if (m_IsBatchLoading == true) {
		m_StagedMeshInstances.PushBack(meshInstance);
	}
	else {
		m_RegisteredMeshInstances.PushBack(meshInstance);
	}
}

void BatchRenderer::UnregisterMesh(Scene::MeshInstance3D* meshInstance) {
	if (meshInstance == nullptr) {
		return;
	}
	if (m_IsBatchUnloading == true) {
		for (size_t a = 0; a < m_RegisteredMeshInstances.GetSize(); ++a) {
			if (m_RegisteredMeshInstances[a] == meshInstance) {
				m_RegisteredMeshInstances[a] = nullptr;
				break;
			}
		}
	}
	else {
		m_RegisteredMeshInstances.Remove(meshInstance);
	}
}

void BatchRenderer::RegisterMultiMesh(Scene::MultiMeshInstance3D* multiMeshInstance) {
	if (multiMeshInstance == nullptr) {
		return;
	}
	if (m_IsBatchLoading == true) {
		m_StagedMultiMeshInstances.PushBack(multiMeshInstance);
	}
	else {
		m_RegisteredMultiMeshInstances.PushBack(multiMeshInstance);
	}
}

void BatchRenderer::UnregisterMultiMesh(Scene::MultiMeshInstance3D* multiMeshInstance) {
	if (multiMeshInstance == nullptr) {
		return;
	}
	if (m_IsBatchUnloading) {
		for (size_t a = 0; a < m_RegisteredMultiMeshInstances.GetSize(); ++a) {
			if (m_RegisteredMultiMeshInstances[a] == multiMeshInstance) {
				m_RegisteredMultiMeshInstances[a] = nullptr;
				break;
			}
		}
	}
	else {
		m_StagedMultiMeshInstances.Remove(multiMeshInstance);
	}
}

void BatchRenderer::RenderScene(Scene::Camera3D const* camera) {
	if (camera == nullptr) {
		return;
	}
	Math::Matrix4x4 const& viewMatrix = camera->GetViewMatrix();
	Math::Matrix4x4 viewProjection = camera->GetProjectionMatrix() * camera->GetViewMatrix();
	std::shared_ptr<Material> lastBoundMaterial = nullptr;

	for (size_t a = 0; a < m_RegisteredMeshInstances.GetSize(); ++a) {
		Scene::MeshInstance3D* meshInstance = m_RegisteredMeshInstances[a];
		if (meshInstance == nullptr) {
			continue;
		}
		std::shared_ptr<Material> material = meshInstance->GetMaterial();
		if (material != nullptr && material != lastBoundMaterial) {
			material->Bind();
			material->SetMatrix4("u_View", viewMatrix);
			material->SetMatrix4("u_Projection", viewProjection);
			lastBoundMaterial = material;
		}
		material->SetFloat("u_IsInstanced", 0.0f);
		meshInstance->Draw(meshInstance->Transform.GetWorldMatrix());
	}

	for (size_t a = 0; a < m_RegisteredMultiMeshInstances.GetSize(); ++a) {
		Scene::MultiMeshInstance3D* multiMeshInstance = m_RegisteredMultiMeshInstances[a];
		if (multiMeshInstance == nullptr) {
			continue;
		}
		std::shared_ptr<Material> material = multiMeshInstance->GetMaterial();
		if (material != nullptr && material != lastBoundMaterial) {
			material->Bind();
			material->SetMatrix4("u_View", viewMatrix);
			material->SetMatrix4("u_Projection", viewProjection);
			material->SetMatrix4("u_Model", multiMeshInstance->Transform.GetWorldMatrix());
			lastBoundMaterial = material;
		}
		material->SetFloat("u_IsInstanced", 1.0f);
		multiMeshInstance->DrawInstanced();
	}
}

}
