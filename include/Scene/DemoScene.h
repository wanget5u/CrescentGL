#pragma once
#include <memory>
#include "Math/Matrix/Matrix4x4.h"
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
	f32 m_Angle{};
	Math::Matrix4x4 m_ViewMatrix{};
	Math::Matrix4x4 m_ProjectionMatrix{};
	f32 m_LastAspectRatio{0.0f};
};

}
