#pragma once

#include "Node3D.h"
#include "Asset/Type/Type.h"

namespace Crescent::Render {
	struct Mesh;
	struct Material;
}

namespace Crescent::Scene {
struct MultiMeshInstance3D : Node3D {
	MultiMeshInstance3D();
	explicit MultiMeshInstance3D(
		std::shared_ptr<Asset::Mesh> meshAsset,
		std::shared_ptr<Render::Material> material = nullptr
	);
	~MultiMeshInstance3D() override = default;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Base Node Components
	///
	void OnTreeEnter() override;
	void OnTreeExit() override;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// MultiMeshInstance3D Components
	///
	void SetMeshAsset(std::shared_ptr<Asset::Mesh> meshAsset) noexcept;
	[[nodiscard]] std::shared_ptr<Asset::Mesh> GetMeshAsset() const noexcept;
	///
	void SetMaterial(std::shared_ptr<Render::Material> material) noexcept;
	[[nodiscard]] std::shared_ptr<Render::Material> GetMaterial() const noexcept;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// GPU-Level Convenience Accessors & Procedural Setters
	///
	///
	[[nodiscard]] Render::Mesh* GetMesh() const noexcept;
	void SetMesh(std::shared_ptr<Render::Mesh> proceduralMesh) noexcept;
	/// Hardware Instancing & Transform Array Management.
	/// Replaces all instance transforms and uploads them to the GPU InstanceVBO
	void SetTransforms(DynamicList<Math::Matrix4x4> const& transforms);
	void SetTransforms(DynamicList<Math::Matrix4x4>&& transforms);
	/// Appends a single matrix and marks or uploads to GPU buffer
	void AddInstance(Math::Matrix4x4 const& transform);
	/// Modifies a specific instance transform
	void UpdateInstance(size_t index, Math::Matrix4x4 const& transform);
	///
	[[nodiscard]] DynamicList<Math::Matrix4x4>& GetTransforms() noexcept;
	[[nodiscard]] const DynamicList<Math::Matrix4x4>& GetTransforms() const noexcept;
	/// Explicitly syncs the local m_Transforms array into the GPU m_InstanceVBO
	void UploadTransformsToGPU();
	///
	void DrawInstanced() const;
private:
	///
	void CheckAndBuildInstanceVAO() const;
	///
	std::shared_ptr<Asset::Mesh> m_MeshAsset{nullptr};
	///
	std::shared_ptr<Render::Material> m_Material{nullptr};
	///
	DynamicList<Math::Matrix4x4> m_Transforms{};
	///
	u32 m_InstanceVAO{0};
	u32 m_InstanceVBO{0};
	u32 m_InstanceCount{0};
	size_t m_InstanceBufferCapacity{0};
	///
	mutable bool m_InstanceVAODirty{true};
	///
	bool m_TransformsDirty{false};
};
}