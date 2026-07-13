#pragma once
#include "Scene/Nodes3D/Geometry/InstancedVisual3D.h"
#include "Asset/AssetType.h"

namespace Crescent {
struct MultiMeshInstance3D : InstancedVisual3D {
	MultiMeshInstance3D();
	explicit MultiMeshInstance3D(
		std::shared_ptr<MeshAsset> meshAsset,
		std::shared_ptr<Material> material = nullptr
	);
	~MultiMeshInstance3D() override;
	void OnTreeEnter() override;
	void OnTreeExit() override;
	void SetMesh(std::shared_ptr<Mesh> proceduralMesh) noexcept override;
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
	DynamicList<Math::Matrix4x4> m_Transforms{};
	u32 m_InstanceSSBO{0};
	u32 m_InstanceCount{0};
	size_t m_InstanceBufferCapacity{0};
	bool m_TransformsDirty{false};
};
}