#include "Scene/DemoScene.h"

#include "Asset/Loader.h"
#include "Asset/Registry.h"
#include "Core/Time.h"
#include "Math/Math.h"
#include "Render/Shader/Shader.h"

namespace Crescent::Scene {

constexpr u32 INSTANCE_COUNT = 4096;

DemoScene::DemoScene() {
	m_ShaderAsset = Asset::Registry::Instance().GetOrLoad<Asset::Shader>("Shaders/unlit", Asset::Type::Shader);
	m_Material = std::make_shared<Render::Material>(m_ShaderAsset);

	m_CubeMesh = std::make_unique<Render::BoxMesh>(0.4f, 0.4f, 0.4f);
	m_CubeMesh->SetMaterial(m_Material);

	m_Material->AlbedoTexture = Asset::Registry::Instance().GetOrLoad<Asset::Texture>("Assets/Textures/Tiles081_1K-JPG_Color.jpg", Asset::Type::Texture);
	m_Material->NormalTexture = Asset::Registry::Instance().GetOrLoad<Asset::Texture>("Assets/Textures/Tiles129B_1K-JPG_Color.jpg", Asset::Type::Texture);

	m_InstanceData.Reserve(INSTANCE_COUNT);
	for (u32 a = 0; a < INSTANCE_COUNT; ++a) {
		f32 r = static_cast<f32>(a % 16) / 7.5f;
		f32 g = static_cast<f32>((a / 16) % 16) / 7.5f;
		f32 b = static_cast<f32>(a / 256) / 15.0f;
		m_InstanceData.PushBack({ Math::Matrix4x4{}, Math::Vector4(r, g, b, 1.0f) });
	}

	glGenBuffers(1, &m_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, INSTANCE_COUNT * sizeof(InstanceData), m_InstanceData.GetData(), GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

DemoScene::~DemoScene() {
	if (m_SSBO != 0) {
		glDeleteBuffers(1, &m_SSBO);
	}
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

	f32 totalTime = Time::GetTotalTime();
	m_Angle = Math::DegreesToRadians(totalTime * 45.0f);
	m_GradientAngle = totalTime * 1.5f;
	f32 cosA = Math::Cos(m_GradientAngle);
	f32 sinA = Math::Sin(m_GradientAngle);

	u32 index{0};
	for (u8 x = 0; x < 16; ++x) {
		for (u8 y = 0; y < 16; ++y) {
			for (u8 z = 0; z < 16; ++z) {
				f32 xPos = (static_cast<f32>(x) - 7.5f) * 1.5f;
				f32 yPos = (static_cast<f32>(y) - 7.5f) * 1.5f + Math::Sin(totalTime * 2.0f + x * 0.3f) * 0.5f;
				f32 zPos = (static_cast<f32>(z) - 10.0f) * 1.5f - 20.0f;

				Math::Matrix4x4 modelMatrix{};
				modelMatrix.TranslateWorld(Math::Vector3(xPos, yPos, zPos));
				modelMatrix.RotateLocal(m_Angle + (x + y + z) * 0.2f, Math::Vector3(0.5f, 1.0f, 0.2f));
				m_InstanceData[index].model = modelMatrix;

				f32 rotX = (static_cast<f32>(x) - 7.5f) * cosA - (static_cast<f32>(y) - 7.5f) * sinA;
				f32 rotY = (static_cast<f32>(x) - 7.5f) * sinA + (static_cast<f32>(y) - 7.5f) * cosA;

				f32 r = Math::Clamp((rotX / 15.0f) + 0.5f, 0.0f, 1.0f);
				f32 g = Math::Clamp((rotY / 15.0f) + 0.5f, 0.0f, 1.0f);
				f32 b = static_cast<f32>(z) / 15.0f;
				m_InstanceData[index].color = Math::Vector4(r, g, b, 1.0f);
				index++;
			}
		}
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, INSTANCE_COUNT * sizeof(InstanceData), m_InstanceData.GetData());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBO);

	m_Material->Bind();
	m_Material->SetMatrix4("view", m_ViewMatrix);
	m_Material->SetMatrix4("projection", m_ProjectionMatrix);

	m_CubeMesh->Bind();
	glDrawElementsInstanced(GL_TRIANGLES, static_cast<i32>(m_CubeMesh->GetIndexCount()), GL_UNSIGNED_INT, nullptr, INSTANCE_COUNT);
	m_CubeMesh->Unbind();
}
}
