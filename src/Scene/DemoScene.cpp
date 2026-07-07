#include "Scene/DemoScene.h"

#include "Asset/Loader.h"
#include "Asset/Registry.h"
#include "Core/Log.h"
#include "Core/Time.h"
#include "Math/Math.h"
#include "Render/Shader/Shader.h"

namespace Crescent::Scene {

DemoScene::DemoScene() {
	m_ShaderAsset = Asset::Registry::Instance().GetOrLoad<Asset::Shader>("Shaders/unlit", Asset::Type::Shader);
	m_Material = std::make_shared<Render::Material>(m_ShaderAsset);

	std::unique_ptr<Render::Mesh> boxMesh1 = std::make_unique<Render::BoxMesh>(0.5f, 0.5f, 0.5f);
	boxMesh1->SetMaterial(m_Material);
	m_Meshes.PushBack(std::move(boxMesh1));

	std::unique_ptr<Render::Mesh> boxMesh2 = std::make_unique<Render::BoxMesh>(0.5f, 0.5f, 0.5f);
	boxMesh2->SetMaterial(m_Material);
	m_Meshes.PushBack(std::move(boxMesh2));

	std::unique_ptr<Render::Mesh> boxMesh3 = std::make_unique<Render::BoxMesh>(0.5f, 0.5f, 0.5f);
	boxMesh3->SetMaterial(m_Material);
	m_Meshes.PushBack(std::move(boxMesh3));

	m_Material->AlbedoTexture = Asset::Registry::Instance().GetOrLoad<Asset::Texture>("Assets/Textures/Tiles081_1K-JPG_Color.jpg", Asset::Type::Texture);
	m_Material->NormalTexture = Asset::Registry::Instance().GetOrLoad<Asset::Texture>("Assets/Textures/Tiles129B_1K-JPG_Color.jpg", Asset::Type::Texture);
}

void DemoScene::OnUpdate([[maybe_unused]] f32 const deltaTime) {}

void DemoScene::OnRender(Window& window) {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	f32 currentAspectRatio = window.GetAspectRatio();
	if (currentAspectRatio != m_LastAspectRatio) {
		m_LastAspectRatio = currentAspectRatio;
		m_ProjectionMatrix = Math::Matrix4x4::GetPerspectiveProjection(Math::DegreesToRadians(70.0f), m_LastAspectRatio);
	}

	m_Material->Bind();
	m_Material->SetMatrix4("view", m_ViewMatrix);
	m_Material->SetMatrix4("projection", m_ProjectionMatrix);

	f32 totalTime = Time::GetTotalTime();
	m_Angle = Math::DegreesToRadians(totalTime * 90.0f);
	f32 sin0 = Math::Sin((totalTime + 1.0f) * 3.0f);
	Math::Matrix4x4 model0{};
	model0.TranslateWorld(Math::Vector3(-2.0f, sin0, -3.0f));
	model0.RotateLocal(m_Angle, Math::Vector3(0.5f, 1.0f, 0.2f));
	m_Material->SetMatrix4("model", model0);
	m_Meshes[0]->Render();

	f32 sin1 = Math::Sin((totalTime + 1.25f) * 3.0f);
	Math::Matrix4x4 model1{};
	model1.TranslateWorld(Math::Vector3(0.0f, sin1, -4.5f));
	model1.RotateLocal(-m_Angle, Math::Vector3(0.2f, 0.6f, 0.5f));
	m_Material->SetMatrix4("model", model1);
	m_Meshes[1]->Render();

	f32 sin2 = Math::Sin((totalTime + 1.5f) * 3.0f);
	Math::Matrix4x4 model2{};
	model2.TranslateWorld(Math::Vector3(2.0f, sin2, -3.0f));
	model2.RotateLocal(-m_Angle, Math::Vector3(0.2f, 1.0f, 0.5f));
	m_Material->SetMatrix4("model", model2);
	m_Meshes[2]->Render();

	timer += Time::GetVariableDeltaTime();
	if (timer > 0.5f) {
		timer = 0;
		Log::Print("FPS: {}", Math::Ceil(Time::GetFPS()));
	}
}
}
