#pragma once

#include "Node3D.h"
#include "Asset/Type/Type.h"

namespace Crescent::Render {
	struct Mesh;
	struct Material;
}

namespace Crescent::Scene {
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Represents a single renderable 3D static mesh instance within the scene tree.
/// Links visual mesh data with a material profile
struct MeshInstance3D : Node3D {
	MeshInstance3D() = default;
	explicit MeshInstance3D(
		std::shared_ptr<Asset::Mesh> meshAsset,
		std::shared_ptr<Render::Material> material = nullptr
	);
	~MeshInstance3D() override = default;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Base Node Components
	///
	void OnTreeEnter() override;
	void OnTreeExit() override;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// MeshInstance3D Components
	///
	void SetMeshAsset(std::shared_ptr<Asset::Mesh> meshAsset) noexcept;
	[[nodiscard]] std::shared_ptr<Asset::Mesh> GetMeshAsset() const noexcept;
	void SetMaterial(std::shared_ptr<Render::Material> material) noexcept;
	[[nodiscard]] std::shared_ptr<Render::Material> GetMaterial() const noexcept;
	[[nodiscard]] Render::Mesh* GetMesh() const noexcept;
	void SetMesh(std::shared_ptr<Render::Mesh> proceduralMesh) noexcept;
	/// Issues the draw command for this single mesh instance
	void Draw(Math::Matrix4x4 const& worldMatrix) const;
private:
	/// Source mesh asset blueprint
	std::shared_ptr<Asset::Mesh> m_MeshAsset{};
	/// Shader/material rules shared by all instances
	std::shared_ptr<Render::Material> m_Material{};
};
}
