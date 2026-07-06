#include "Scene/DemoScene.h"
#include "Asset/AssetLoader.h"
#include "Core/Log.h"
#include "Core/Time.h"
#include "Math/Trigonometry.h"

namespace Crescent {

DemoScene::DemoScene() {
	m_Shader = std::make_unique<Shader>("Shaders/default.vert", "Shaders/default.frag");
	f32 vertices[] = {
		// Front Face (-Z)
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 0
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 1
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 2
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 3
		// Back Face (+Z)
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 4
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 5
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 6
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // 7
		// Left Face (-X)
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // 8
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 9
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // 10
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 11
		// Right Face (+X)
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // 12
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 13
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // 14
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 15
		// Bottom Face (-Y)
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // 16
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // 17
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 18
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 19
		// Top Face (+Y)
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 20
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 21
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // 22
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // 23
	};
	u32 indices[] = {
		// Front Face
		0, 1, 2,
		0, 2, 3,
		// Back Face
		4, 5, 6,
		4, 6, 7,
		// Left Face
		8, 9, 10,
		8, 10, 11,
		// Right Face
		12, 13, 14,
		12, 14, 15,
		// Bottom Face
		16, 17, 18,
		16, 18, 19,
		// Top Face
		20, 21, 22,
		20, 22, 23
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

	m_Shader->Use();

	Math::Matrix4x4 viewMatrix{};

	Math::Matrix4x4 projection = Math::Matrix4x4::GetPerspectiveProjection(
		Math::DegreesToRadians(70.0f), aspectRatio
	);

	Math::Matrix4x4 modelMatrix{};
	viewMatrix.TranslateWorld(Math::Vector3(0.0f,  0.0f,  -2.0f));
	modelMatrix.RotateLocal(Math::DegreesToRadians(Time::GetTotalTime() * 90.f), Math::Vector3(0.5f, 1.0f, 0.2f));
	m_Shader->SetMatrix4("model", modelMatrix);
	m_Shader->SetMatrix4("view", viewMatrix);
	m_Shader->SetMatrix4("projection", projection);
	m_Shader->SetFloat("alphaBlend", m_AlphaBlendValue);
	m_QuadMesh->Draw();
}

}
