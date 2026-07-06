#include "Scene/DemoScene.h"
#include "Asset/AssetLoader.h"
#include "Core/Log.h"
#include "Core/Time.h"
#include "Math/Math.h"
#include "Render/Shader/Shader.h"

namespace Crescent {

DemoScene::DemoScene() {
	auto shader = std::make_shared<Render::Shader>("Shaders/unlit.vert", "Shaders/unlit.frag");
	m_Material = std::make_shared<Render::Material>(shader, Math::Vector4(0.3f, 0.5f, 0.7f, 1.0f));

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
	while (AssetLoader::Instance().PopReadyTexture(loadedTextureID)) {
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
	const f32 aspectRatio = window.GetAspectRatio();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Texture2);

	Math::Matrix4x4 viewMatrix{};
	Math::Matrix4x4 projection = Math::Matrix4x4::GetPerspectiveProjection(
		Math::DegreesToRadians(70.0f), aspectRatio
	);

	f32 angle = Math::DegreesToRadians(Time::GetTotalTime() * 90.0f);

	m_Material->Shader->SetMatrix4("view", viewMatrix);
	m_Material->Shader->SetMatrix4("projection", projection);

	f32 sin0 = Math::Sin((Time::GetTotalTime() + 1.0f) * 3.0f);
	Math::Matrix4x4 model0{};
	model0.TranslateWorld(Math::Vector3(-2.0f, sin0, -3.0f));
	model0.RotateLocal(angle, Math::Vector3(0.5f, 1.0f, 0.2f));
	m_Material->Shader->SetMatrix4("model", model0);
	m_Meshes[0]->Render();

	f32 sin1 = Math::Sin((Time::GetTotalTime() + 1.25f) * 3.0f);
	Math::Matrix4x4 model1{};
	model1.TranslateWorld(Math::Vector3(0.0f, sin1, -4.5f));
	model1.RotateLocal(-angle, Math::Vector3(0.2f, 1.0f, 0.5f));
	m_Material->Shader->SetMatrix4("model", model1);
	m_Meshes[2]->Render();

	f32 sin2 = Math::Sin((Time::GetTotalTime() + 1.5f) * 3.0f);
	Math::Matrix4x4 model2{};
	model2.TranslateWorld(Math::Vector3(2.0f, sin2, -3.0f));
	model2.RotateLocal(-angle, Math::Vector3(0.2f, 1.0f, 0.5f));
	m_Material->Shader->SetMatrix4("model", model2);
	m_Meshes[1]->Render();

	timer += Time::GetVariableDeltaTime();

	if (timer > 1.0f) {
		timer = 0;
		Log::Print("{}", Math::Ceil(Time::GetFPS()));
	}
}
}
