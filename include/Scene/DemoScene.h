#pragma once
#include <memory>
#include "Render/Primitives/BoxMesh.h"
#include "Render/Material/Material.h"
#include "Scene/Scene.h"

namespace Crescent {

struct DemoScene : Scene {
	DemoScene();
	~DemoScene() override = default;
	void OnUpdate(f32 deltaTime) override;
	void OnRender(Window& window) override;
private:
	std::shared_ptr<Render::Material> m_Material{};
	Collections::DynamicList<std::unique_ptr<Render::Mesh>> m_Meshes{};
	u32 m_Texture1{0};
	u32 m_Texture2{0};
	f32 timer{0};
};

}
