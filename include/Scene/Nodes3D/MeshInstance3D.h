#pragma once

#include "Node3D.h"
#include "Asset/Type/Type.h"

namespace Crescent::Render {
	struct Mesh;
	struct Material;
}

namespace Crescent::Scene {
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
	///
	void SetMeshAsset(std::shared_ptr<Asset::Mesh> meshAsset) noexcept;
	///
	[[nodiscard]] std::shared_ptr<Asset::Mesh> GetMeshAsset() const noexcept;
	///
	void SetMaterial(std::shared_ptr<Render::Material> material) noexcept;
	[[nodiscard]] std::shared_ptr<Render::Material> GetMaterial() const noexcept;
	///
	[[nodiscard]] Render::Mesh* GetMesh() const noexcept;
	///
	void SetMesh(std::shared_ptr<Render::Mesh> proceduralMesh) noexcept;
	///
	void Draw(Math::Matrix4x4 const& worldMatrix) const;
private:
	std::shared_ptr<Asset::Mesh> m_MeshAsset{};
	std::shared_ptr<Render::Material> m_Material{};
};
}
