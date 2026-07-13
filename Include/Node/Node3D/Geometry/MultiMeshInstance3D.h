#pragma once
#include "Asset/AssetType.h"
#include "Node/Node3D/Geometry/InstancedVisual3D.h"

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
	/// Replaces all instance transforms
	void SetTransforms(DynamicList<Math::Matrix4x4> const& transforms);
	void SetTransforms(DynamicList<Math::Matrix4x4>&& transforms);
	/// Appends a single matrix
	void AddInstance(Math::Matrix4x4 const& transform);
	/// Modifies a specific instance transform
	void UpdateInstance(size_t index, Math::Matrix4x4 const& transform);
	/// Returns a mutable reference to the internal local transform array.
	[[nodiscard]] DynamicList<Math::Matrix4x4>& GetTransforms() noexcept;
	[[nodiscard]] const DynamicList<Math::Matrix4x4>& GetTransforms() const noexcept;
	/// Binds the instance data layouts and issues a hardware-instanced draw call
	void DrawInstanced() const;
	void Draw() const override;
private:
	DynamicList<Math::Matrix4x4> m_Transforms{};
	u32 m_InstanceCount{0};
};
}