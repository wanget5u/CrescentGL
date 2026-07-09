#pragma once
#include "../VisualInstance3D.h"

namespace Crescent::Render {
	struct Material;
	struct Mesh;
}

namespace Crescent::Asset {
	struct Mesh;
}

namespace Crescent::Scene {
struct GeometryInstance3D : VisualInstance3D {
	friend struct Render::BatchRenderer;
	GeometryInstance3D() = default;
	~GeometryInstance3D() override = default;
	void OnTreeEnter() override;
	void OnTreeExit() override;
	void SetMeshAsset(std::shared_ptr<Asset::Mesh> meshAsset) noexcept;
	[[nodiscard]] std::shared_ptr<Asset::Mesh> GetMeshAsset() const noexcept;
	void SetMaterialOverride(std::shared_ptr<Render::Material> material) noexcept;
	[[nodiscard]] std::shared_ptr<Render::Material> GetMaterialOverride() const noexcept;
	void SetMaterial(std::shared_ptr<Render::Material> material) noexcept;
	[[nodiscard]] std::shared_ptr<Render::Material> GetMaterial() const noexcept;
	[[nodiscard]] Render::Mesh* GetMesh() const noexcept;
	virtual void SetMesh(std::shared_ptr<Render::Mesh> proceduralMesh) noexcept;
protected:
	std::shared_ptr<Asset::Mesh> m_MeshAsset{};
	std::shared_ptr<Render::Material> m_Material{};
};
}
