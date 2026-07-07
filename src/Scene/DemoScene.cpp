#include "Scene/DemoScene.h"
#include "Asset/AssetLoader.h"
#include "Core/Log.h"
#include "Core/Time.h"
#include "Math/Math.h"
#include "Render/Shader/Shader.h"

namespace Crescent {

DemoScene::DemoScene() {
	auto shader = std::make_shared<Render::Shader>("Shaders/unlit.vert", "Shaders/unlit.frag");
	m_Material = std::make_shared<Render::Material>(shader, Math::Vector4(0.7f, 0.5f, 0.7f, 1.0f));

	auto boxMesh1 = std::make_unique<Render::BoxMesh>(1.0f);
	boxMesh1->SetMaterial(m_Material);
	m_Meshes.PushBack(std::move(boxMesh1));

	auto boxMesh2 = std::make_unique<Render::BoxMesh>(1.0f);
	boxMesh2->SetMaterial(m_Material);
	m_Meshes.PushBack(std::move(boxMesh2));

	auto boxMesh3 = std::make_unique<Render::BoxMesh>(1.0f);
	boxMesh3->SetMaterial(m_Material);
	m_Meshes.PushBack(std::move(boxMesh3));

	AssetLoader::Instance().LoadTextureAsync("Assets/Textures/Tiles081_1K-JPG_Color.jpg");
	AssetLoader::Instance().LoadTextureAsync("Assets/Textures/Tiles129B_1K-JPG_Color.jpg");
}

void DemoScene::OnUpdate([[maybe_unused]] f32 const deltaTime) {
	u32 loadedTextureID = 0;
	while (AssetLoader::Instance().PopReadyTexture(loadedTextureID) == true) {
		if (m_Texture1 == 0) {
			m_Texture1 = loadedTextureID;
			if (m_Material) {
				m_Material->AlbedoTextureID = m_Texture1;
			}
		} else {
			m_Texture2 = loadedTextureID;
			if (m_Material) {
				m_Material->NormalTextureID = m_Texture2;
			}
		}
		Log::Info("DemoScene: Successfully received loaded texture {}.", loadedTextureID);
	}
}

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
	m_Material->Shader->SetMatrix4("view", m_ViewMatrix);
	m_Material->Shader->SetMatrix4("projection", m_ProjectionMatrix);

	f32 totalTime = Time::GetTotalTime();
	m_Angle = Math::DegreesToRadians(totalTime * 90.0f);
	f32 sin0 = Math::Sin((totalTime + 1.0f) * 3.0f);
	Math::Matrix4x4 model0{};
	model0.TranslateWorld(Math::Vector3(-2.0f, sin0, -3.0f));
	model0.RotateLocal(m_Angle, Math::Vector3(0.5f, 1.0f, 0.2f));
	m_Material->Shader->SetMatrix4("model", model0);
	m_Meshes[0]->Render();

	f32 sin1 = Math::Sin((totalTime + 1.25f) * 3.0f);
	Math::Matrix4x4 model1{};
	model1.TranslateWorld(Math::Vector3(0.0f, sin1, -4.5f));
	model1.RotateLocal(-m_Angle, Math::Vector3(0.2f, 0.6f, 0.5f));
	m_Material->Shader->SetMatrix4("model", model1);
	m_Meshes[1]->Render();

	f32 sin2 = Math::Sin((totalTime + 1.5f) * 3.0f);
	Math::Matrix4x4 model2{};
	model2.TranslateWorld(Math::Vector3(2.0f, sin2, -3.0f));
	model2.RotateLocal(-m_Angle, Math::Vector3(0.2f, 1.0f, 0.5f));
	m_Material->Shader->SetMatrix4("model", model2);
	m_Meshes[2]->Render();

	timer += Time::GetVariableDeltaTime();
	if (timer > 0.5f) {
		timer = 0;
		Log::Print("FPS: {}", Math::Ceil(Time::GetFPS()));
	}
}
}
