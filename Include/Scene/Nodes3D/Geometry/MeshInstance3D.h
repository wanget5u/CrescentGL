#pragma once
#include "Scene/Nodes3D/Geometry/GeometryInstance3D.h"
#include "Asset/AssetType.h"

namespace Crescent {
/// Represents a single renderable 3D static mesh instance within the scene tree.
/// Links visual mesh data with a material profile
struct MeshInstance3D : GeometryInstance3D {
	MeshInstance3D() = default;
	explicit MeshInstance3D(
		std::shared_ptr<MeshAsset> meshAsset,
		std::shared_ptr<Material> material = nullptr
	);
	~MeshInstance3D() override = default;
	void OnTreeEnter() override;
	void OnTreeExit() override;
	/// Issues the draw command for this single mesh instance
	void Draw(Math::Matrix4x4 const& worldMatrix) const;
};
}
