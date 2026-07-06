#include "Scene/DemoScene.h"
#include "Asset/AssetLoader.h"
#include "Core/Log.h"
#include "Core/Time.h"
#include "Math/Trigonometry.h"

namespace Crescent {

DemoScene::DemoScene() {
	m_Shader = std::make_unique<Shader>("Shaders/default.vert", "Shaders/default.frag");
	f32 vertices[] = {
		1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
	   -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
	   -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f//
	};
	u32 indices[] = {
		0, 1, 2,
		0, 2, 3
	};
	m_QuadMesh = std::make_unique<Mesh>(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(u32));
	AssetLoader::Instance().LoadTextureAsync("Assets/Textures/Tiles081_1K-JPG_Color.jpg");
	AssetLoader::Instance().LoadTextureAsync("Assets/Textures/Tiles129B_1K-JPG_Color.jpg");
	m_Shader->Use();
	m_Shader->SetInt("texture1", 0);
	m_Shader->SetInt("texture2", 1);
}

void DemoScene::OnUpdate([[maybe_unused]] f32 const deltaTime) {
	u32 loadedTextureID = 0;
	while (AssetLoader::Instance().PopReadyTexture(loadedTextureID)) {
		if (m_Texture1 == 0) {
			m_Texture1 = loadedTextureID;
		} else {
			m_Texture2 = loadedTextureID;
		}
		Log::Info("DemoScene: Successfully received loaded texture {", loadedTextureID, "}.");
	}
	m_AlphaBlendValue = (Math::Sin(Time::GetTotalTime() * 1.25f) * 0.2f) + 0.5f;
	m_ZoomFactorValue = (Math::Sin(Time::GetTotalTime() * 1.5f) * 0.1f) + 0.7f;
}

void DemoScene::OnRender(Window& window) {
	const f32 aspectRatio = window.GetWindowAspectRatio();
	const Math::Matrix4x4 projectionMatrix = Math::Matrix4x4::GetOrthographicProjection(-aspectRatio, aspectRatio, -1.0f, 1.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Texture2);

	m_Shader->Use();
	m_Shader->SetMatrix4("projection", projectionMatrix);
	m_Shader->SetFloat("alphaBlend", m_AlphaBlendValue);
	m_Shader->SetFloat("rotationAngle", Time::GetTotalTime());
	m_Shader->SetFloat("zoomFactor", m_ZoomFactorValue);

	for (u16 a = 0; a < 256; a++) {
		f32 mul = a * 0.95f;
		Math::Matrix4x4 matrix{};
		matrix.Scale(Math::Vector3(m_ZoomFactorValue, m_ZoomFactorValue, m_ZoomFactorValue));
		matrix.RotateLocal(Math::DegreesToRadians((Time::GetTotalTime() * 20.0f + mul) + mul), Math::Vector3(0.0f, 0.0f, 1.0f));
		f32 spacing = 0.015f;
		matrix.TranslateWorld(Math::Vector3(0.5f - (a * spacing), -0.1f + (a * spacing), 0.0f));
		matrix.RotateWorld(Math::DegreesToRadians((Time::GetTotalTime() * 50.0f + mul) + mul), Math::Vector3(0.0f, 0.0f, 1.0f));
		m_Shader->SetMatrix4("transform", matrix);
		m_QuadMesh->Draw();
	}
}

}
