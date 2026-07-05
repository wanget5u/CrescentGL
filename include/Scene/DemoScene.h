#pragma once
#include <memory>
#include "Render/Mesh.h"
#include "Scene/Scene.h"
#include "Shader/Shader.h"

namespace Crescent {

struct DemoScene : Scene {
	DemoScene();
	~DemoScene() override = default;

	void OnUpdate(f32 deltaTime) override;
	void OnRender(Window& window) override;

private:
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Mesh>   m_QuadMesh;
	u32                     m_Texture1{0};
	u32                     m_Texture2{0};
	f32                     m_AlphaBlendValue{0.0f};
	f32                     m_ZoomFactorValue{0.0f};
};

}
