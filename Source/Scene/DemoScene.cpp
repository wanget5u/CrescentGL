#include "Scene/DemoScene.h"

#include <glad/glad.h>

#include "Asset/Loader.h"
#include "Asset/Registry.h"
#include "Core/Time.h"
#include "Math/Math.h"
#include "Render/Primitives/BoxMesh.h"
#include "Scene/Nodes3D/Camera3D.h"
#include "Scene/Nodes3D/Geometry/MeshInstance3D.h"
#include "Scene/Nodes3D/Geometry/MultiMeshInstance3D.h"
#include "Scene/Tree.h"
#include "Core/Window.h"
#include "Render/BatchRenderer.h"

namespace Crescent::Scene {

DemoScene::DemoScene() {
    std::shared_ptr<Asset::Texture> albedoTexture =
        Asset::Registry::Instance()
        .GetOrLoad<Asset::Texture>("MetalPlates006_2K-JPG_Color.jpg", Asset::AssetType::Texture);
    std::shared_ptr<Asset::Texture> metallicTexture =
        Asset::Registry::Instance()
        .GetOrLoad<Asset::Texture>("MetalPlates006_2K-JPG_Metalness.jpg", Asset::AssetType::Texture);
    std::shared_ptr<Asset::Texture> normalTexture =
        Asset::Registry::Instance()
        .GetOrLoad<Asset::Texture>("MetalPlates006_2K-JPG_NormalGL.jpg", Asset::AssetType::Texture);
    std::shared_ptr<Asset::Texture> roughnessTexture =
        Asset::Registry::Instance()
        .GetOrLoad<Asset::Texture>("MetalPlates006_2K-JPG_Roughness.jpg", Asset::AssetType::Texture);

    if (m_LitMaterial != nullptr) {
        m_LitMaterial->AlbedoTexture = albedoTexture;
        m_LitMaterial->MetallicTexture = metallicTexture;
        m_LitMaterial->NormalTexture = normalTexture;
        m_LitMaterial->RoughnessTexture = roughnessTexture;
    }
    if (m_UnlitMaterial != nullptr) {
        m_UnlitMaterial->AlbedoTexture = albedoTexture;
    }
    if (m_WireframeMaterial != nullptr) {
        m_WireframeMaterial->AlbedoTexture = albedoTexture;
    }
    if (m_Material != nullptr) {
        m_Material->AlbedoTexture = albedoTexture;
        m_Material->MetallicTexture = metallicTexture;
        m_Material->NormalTexture = normalTexture;
        m_Material->RoughnessTexture = roughnessTexture;
    }

    // meshes
    m_FloorMesh = std::make_shared<Render::BoxMesh>(3.0f, 0.5f, 3.0f);
    m_BoxMesh = std::make_shared<Render::BoxMesh>(1.0f, 1.0f, 1.0f);

    m_Cube = m_Tree->AddChild<MeshInstance3D>("Cube");
    m_Cube->SetMesh(m_BoxMesh);
    m_Cube->SetMaterialOverride(m_Material);
    m_Cube->Transform.SetScale(3.0f);
    m_Cube->Transform.SetPosition(Math::Vector3(0.0f, 2.0f, -10.0f));

    m_PreviewCamera->Transform.SetPosition(Math::Vector3(0.0f, 0.0f, 30.0f));

    for (u8 a = 0; a < 32; ++a) {
        PointLight3D* light = m_Tree->AddChild<PointLight3D>("PointLight3D");
        light->Transform.SetPosition(Math::Vector3(0.0f, 1.0f, -10.0f));
        light->SetRange(50.0f);
        light->SetEnergy(20.0f);
        light->SetColor(Math::Vector3(1.0f, 0.85f, 0.75f));
        m_PointLights.PushBack(light);
    }

    m_OrbitingCubes = m_Tree->AddChild<MultiMeshInstance3D>("OrbitingCubes");
    m_OrbitingCubes->SetMesh(m_FloorMesh);
    m_OrbitingCubes->SetMaterial(m_Material);
    // init

    DynamicList<Math::Matrix4x4> initialTransforms{};
    constexpr f32 spacing = 3.0f;
    constexpr f32 centerOffset = (128.0f - 1.0f) / 2.0f;
    constexpr f32 yOffset = (4.0f - 1.0f) / 2.0f;
    for (u16 x = 0; x < 128; ++x) {
        for (u16 y = 0; y < 1; ++y) {
            for (u16 z = 0; z < 128; ++z) {
                f32 xPos = (static_cast<f32>(x) - centerOffset) * spacing;
                f32 yPos = (static_cast<f32>(y) - yOffset);
                f32 zPos = (static_cast<f32>(z) - centerOffset) * spacing - 10.0f;
                Math::Matrix4x4 modelMatrix{};
                modelMatrix.TranslateWorld(Math::Vector3(xPos, yPos, zPos));
                initialTransforms.PushBack(modelMatrix);
            }
        }
    }
    m_OrbitingCubes->SetTransforms(std::move(initialTransforms));
    m_OrbitingCubes->UploadTransformsToGPU();
}

DemoScene::~DemoScene() {}

void DemoScene::OnUpdate(f32 const deltaTime) {
    m_TotalTime += deltaTime;

    const f32 numLights = static_cast<f32>(m_PointLights.GetSize());
    for (u8 a = 0; a < m_PointLights.GetSize(); ++a) {
        constexpr f32 horizontalRadius = 40.0f;
        constexpr f32 ringRotationSpeed = 0.1f;
        const f32 factor = a % 2 ? 1.0f : 2.0f;
        f32 angle = (static_cast<f32>(a) / numLights) * 2.0f * 3.14159265f;
        f32 currentAngle = angle + (m_TotalTime * ringRotationSpeed);
        f32 xPos = Math::Sin(currentAngle) * horizontalRadius * factor;
        f32 zPos = (Math::Cos(currentAngle) * horizontalRadius * factor) - 10.0f;

        m_PointLights[a]->Transform.SetPosition(Math::Vector3(xPos, 1.0f, zPos));

        f32 r = 0.15f + (Math::Sin(m_TotalTime * 1.0f + a) * 0.425f) + 0.425f;
        f32 g = 0.15f + (Math::Sin(m_TotalTime * 0.7f + a * 2.0f) * 0.425f) + 0.425f;
        f32 b = 0.15f + (Math::Cos(m_TotalTime * 1.2f + a) * 0.425f) + 0.425f;

        m_PointLights[a]->SetColor(Math::Vector3(r, g, b));
    }
    m_Material->TrySetFloat("u_Time", m_TotalTime);
}

void DemoScene::OnRender(Window& window) {
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    f32 currentAspectRatio = window.GetAspectRatio();
    if (currentAspectRatio != m_LastAspectRatio) {
        m_LastAspectRatio = currentAspectRatio;
        m_PreviewCamera->SetPerspective(70.0f, m_LastAspectRatio, 0.1f, 1000.0f);
    }
    m_Tree->GetBatchRenderer()->RenderScene(m_PreviewCamera.get());
}

}