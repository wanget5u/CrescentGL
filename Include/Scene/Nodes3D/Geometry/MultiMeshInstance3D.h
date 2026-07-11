#pragma once
#include "InstancedVisual3D.h"
#include "../../../Asset/AssetType.h"

namespace Crescent::Scene {
struct MultiMeshInstance3D : InstancedVisual3D {
	MultiMeshInstance3D();
	explicit MultiMeshInstance3D(
		std::shared_ptr<Asset::Mesh> meshAsset,
		std::shared_ptr<Render::Material> material = nullptr
	);
	~MultiMeshInstance3D() override = default;
	void OnTreeEnter() override;
	void OnTreeExit() override;
	void SetMesh(std::shared_ptr<Render::Mesh> proceduralMesh) noexcept override;
	/// Hardware Instancing & Transform Array Management.
	/// Replaces all instance transforms and uploads them to the GPU InstanceVBO
	void SetTransforms(DynamicList<Math::Matrix4x4> const& transforms);
	void SetTransforms(DynamicList<Math::Matrix4x4>&& transforms);
	/// Appends a single matrix and marks or uploads to GPU buffer
	void AddInstance(Math::Matrix4x4 const& transform);
	/// Modifies a specific instance transform
	void UpdateInstance(size_t index, Math::Matrix4x4 const& transform);
	/// Returns a mutable reference to the internal local transform array.
	/// Modifying this directly requires an explicit call to UploadTransformsToGPU() before rendering
	[[nodiscard]] DynamicList<Math::Matrix4x4>& GetTransforms() noexcept;
	[[nodiscard]] const DynamicList<Math::Matrix4x4>& GetTransforms() const noexcept;
	/// Explicitly syncs the local m_Transforms array into the GPU m_InstanceVBO
	void UploadTransformsToGPU();
	/// Binds the instance data layouts and issues a hardware-instanced draw call
	void DrawInstanced() const;
	void Draw() const override;
private:
	/// Verifies that the instance vertex array layouts match the data, rebuilding them if dirty
	void CheckAndBuildInstanceVAO() const;
	/// Source mesh asset blueprint
	DynamicList<Math::Matrix4x4> m_Transforms{};
	u32 m_InstanceVAO{0};
	u32 m_InstanceVBO{0};
	u32 m_InstanceCount{0};
	/// Maximum matrix capacity currently allocated on the GPU VBO
	size_t m_InstanceBufferCapacity{0};
	/// Tracks if VAO bindings need rebuilding
	mutable bool m_InstanceVAODirty{true};
	/// Tracks if CPU matrix array edits need syncing to the GPU
	bool m_TransformsDirty{false};
};
}