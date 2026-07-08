#pragma once

#include "Render/Material/Material.h"
#include "Scene/Scene.h"

namespace Crescent::Render {
	struct Material;
	struct Mesh;
}

namespace Crescent::Scene {
struct Camera3D;
struct MeshInstance3D;
struct MultiMeshInstance3D;
struct DemoScene : Scene {
	DemoScene();
	~DemoScene() override;
	void OnUpdate(f32 deltaTime) override;
	void OnRender(Window& window) override;
private:
	std::shared_ptr<Render::Material> m_Material{nullptr};
	std::shared_ptr<Render::Mesh> m_BoxMesh{nullptr};

	MeshInstance3D* m_CubeNode{nullptr};
	MultiMeshInstance3D* m_OrbitingCubesNode{nullptr};

	f32 m_TotalTime{0.0f};
	f32 m_LastAspectRatio{0.0f};
};

}
