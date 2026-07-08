#pragma once
#include "Collection/DynamicList.h"

namespace Crescent::Scene {
	struct MeshInstance3D;
	struct MultiMeshInstance3D;
	struct Camera3D;
}

namespace Crescent::Render {
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
struct BatchRenderer {
	BatchRenderer() = default;
	~BatchRenderer() = default;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Batch Loading / Unloading
	///
	/// Prevents array reallocation and flickering when spawning/destroying many nodes in a single frame
	void BeginBatchLoad();
	void EndBatchLoad();
	void BeginBatchUnload();
	void EndBatchUnload();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Registration API (Called by Node3D inside OnTreeEnter() / OnTreeExit())
	/// TODO: I will have to move these into a generic mesh component because if i maybe add Mesh2D
	/// TODO: it will scale horribly in the current state
	void RegisterMesh(Scene::MeshInstance3D* meshInstance);
	void UnregisterMesh(Scene::MeshInstance3D* meshInstance);
	void RegisterMultiMesh(Scene::MultiMeshInstance3D* multiMeshInstance);
	void UnregisterMultiMesh(Scene::MultiMeshInstance3D* multiMeshInstance);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Render Execution
	///
	void RenderScene(Scene::Camera3D const* camera);
private:
	///
	bool m_IsBatchLoading{false};
	///
	bool m_IsBatchUnloading{false};
	///
	DynamicList<Scene::MeshInstance3D*> m_RegisteredMeshInstances{};
	///
	DynamicList<Scene::MeshInstance3D*> m_StagedMeshInstances{};
	///
	DynamicList<Scene::MultiMeshInstance3D*> m_RegisteredMultiMeshInstances{};
	///
	DynamicList<Scene::MultiMeshInstance3D*> m_StagedMultiMeshInstances{};
};
}