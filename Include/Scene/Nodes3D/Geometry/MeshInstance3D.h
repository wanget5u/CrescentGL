#pragma once
#include "GeometryInstance3D.h"
#include "Asset/Type/Type.h"

namespace Crescent::Scene {
/// Represents a single renderable 3D static mesh instance within the scene tree.
/// Links visual mesh data with a material profile
struct MeshInstance3D : GeometryInstance3D {
	MeshInstance3D() = default;
	explicit MeshInstance3D(
		std::shared_ptr<Asset::Mesh> meshAsset,
		std::shared_ptr<Render::Material> material = nullptr
	);
	~MeshInstance3D() override = default;
	void OnTreeEnter() override;
	void OnTreeExit() override;
	/// Issues the draw command for this single mesh instance
	void Draw(Math::Matrix4x4 const& worldMatrix) const;
};
}
