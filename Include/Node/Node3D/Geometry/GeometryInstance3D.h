#pragma once
#include "Node/Node3D/VisualInstance3D.h"

namespace Crescent {
	struct Material;
	struct Mesh;
	struct MeshAsset;
}

namespace Crescent {
struct GeometryInstance3D : VisualInstance3D {
	friend struct BatchRenderer;
	GeometryInstance3D() = default;
	~GeometryInstance3D() override = default;
	void OnTreeEnter() override;
	void OnTreeExit() override;
	void SetMeshAsset(std::shared_ptr<MeshAsset> meshAsset) noexcept;
	[[nodiscard]] std::shared_ptr<MeshAsset> GetMeshAsset() const noexcept;
	void SetMaterialOverride(std::shared_ptr<Material> material) noexcept;
	[[nodiscard]] std::shared_ptr<Material> GetMaterialOverride() const noexcept;
	void SetMaterial(std::shared_ptr<Material> material) noexcept;
	[[nodiscard]] std::shared_ptr<Material> GetMaterial() const noexcept;
	[[nodiscard]] Mesh* GetMesh() const noexcept;
	virtual void SetMesh(std::shared_ptr<Mesh> proceduralMesh) noexcept;
protected:
	std::shared_ptr<MeshAsset> m_MeshAsset{};
	std::shared_ptr<Material> m_Material{};
};
}
