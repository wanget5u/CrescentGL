#pragma once
#include <memory>

#include "Math/Matrix/Matrix4x4.h"
#include "Render/Material/Material.h"
#include "Render/Primitives/BoxMesh.h"
#include "Render/Texture/Texture.h"
#include "Scene/Scene.h"

namespace Crescent::Scene {

struct DemoScene : Scene {
	DemoScene();
	~DemoScene() override = default;
	void OnUpdate(f32 deltaTime) override;
	void OnRender(Window& window) override;
private:
	std::shared_ptr<Render::Material> m_Material{};
	DynamicList<std::unique_ptr<Render::Mesh>> m_Meshes{};
	std::shared_ptr<Asset::Shader> m_ShaderAsset{nullptr};
	std::shared_ptr<Asset::Texture> m_Texture1{nullptr};
	std::shared_ptr<Asset::Texture> m_Texture2{nullptr};
	f32 timer{0};
	f32 m_Angle{};
	Math::Matrix4x4 m_ViewMatrix{};
	Math::Matrix4x4 m_ProjectionMatrix{};
	f32 m_LastAspectRatio{0.0f};
};

}
