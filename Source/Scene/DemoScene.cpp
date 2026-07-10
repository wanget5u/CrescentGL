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

constexpr u32 INSTANCE_COUNT = 1048576;

DemoScene::DemoScene() {
    // shaders
    std::shared_ptr<Asset::Shader> shaderAsset =
        Asset::Registry::Instance()
        .GetOrLoad<Asset::Shader>("Shaders/lit", Asset::Type::Shader);
    // materials
    m_Material = std::make_shared<Render::Material>(shaderAsset);
    m_Material->AlbedoTexture =
        Asset::Registry::Instance()
        .GetOrLoad<Asset::Texture>("MetalPlates006_2K-JPG_Color.jpg", Asset::Type::Texture);
    m_Material->MetallicTexture =
        Asset::Registry::Instance()
        .GetOrLoad<Asset::Texture>("MetalPlates006_2K-JPG_Metalness.jpg", Asset::Type::Texture);
    m_Material->NormalTexture =
        Asset::Registry::Instance()
        .GetOrLoad<Asset::Texture>("MetalPlates006_2K-JPG_NormalGL.jpg", Asset::Type::Texture);
    m_Material->RoughnessTexture =
        Asset::Registry::Instance()
        .GetOrLoad<Asset::Texture>("MetalPlates006_2K-JPG_Roughness.jpg", Asset::Type::Texture);

    m_Material->SetFloat("u_MetallicFactor", 1.0f);
    // meshes
    m_BoxMesh = std::make_shared<Render::BoxMesh>(1.0f, 1.0f, 1.0f);

    m_Cube = m_Tree->AddChild<MeshInstance3D>("Cube");
    m_Cube->SetMesh(m_BoxMesh);
    m_Cube->SetMaterialOverride(m_Material);
    m_Cube->Transform.SetScale(5.0f);
    m_Cube->Transform.SetPosition(Math::Vector3(0.0f, 0.0f, -10.0f));

    // m_OrbitingCubes = m_Tree->AddChild<MultiMeshInstance3D>("OrbitingCubes");
    // m_OrbitingCubes->SetMesh(m_BoxMesh);
    // m_OrbitingCubes->SetMaterial(m_Material);
    // DynamicList<Math::Matrix4x4> initialTransforms{};
    // for (u8 a = 0; a < 8; ++a) {
    //     initialTransforms.PushBack(Math::Matrix4x4::GetIdentity());
    // }
    // m_OrbitingCubes->SetTransforms(initialTransforms);
    // m_OrbitingCubes->UploadTransformsToGPU();

    for (u8 a = 0; a < 16; ++a) {
        PointLight3D* light = m_Tree->AddChild<PointLight3D>("PointLight3D");
        light->Transform.SetPosition(Math::Vector3(0.0f, 0.0f, -10.0f));
        light->SetRange(500.0f);
        light->SetEnergy(20.0f);
        light->SetColor(Math::Vector3(1.0f, 0.85f, 0.75f));
        m_PointLights.PushBack(light);
    }

    // m_PointLight = m_Tree->AddChild<PointLight3D>("PointLight3D");
    // m_PointLight->Transform.SetPosition(Math::Vector3(0.0f, 0.0f, -10.0f));
    // m_PointLight->SetRange(100.0f);
    // m_PointLight->SetEnergy(10.0f);
    // m_PointLight->SetColor(Math::Vector3(1.0f, 0.85f, 0.7f));

    m_OrbitingCubes = m_Tree->AddChild<MultiMeshInstance3D>("OrbitingCubes");
    m_OrbitingCubes->SetMesh(m_BoxMesh);
    m_OrbitingCubes->SetMaterial(m_Material);
    // init
    DynamicList<Math::Matrix4x4> initialTransforms{};
    initialTransforms.Reserve(INSTANCE_COUNT);
    for (u32 a = 0; a < INSTANCE_COUNT; ++a) {
        initialTransforms.PushBack(Math::Matrix4x4::GetIdentity());
    }
    m_OrbitingCubes->SetTransforms(std::move(initialTransforms));

    constexpr f32 spacing = 3.0f;
    constexpr f32 centerOffset = (256.0f - 1.0f) / 2.0f;
    for (u16 x = 0; x < 256; ++x) {
        for (u16 y = 0; y < 2; ++y) {
            for (u16 z = 0; z < 256; ++z) {
                f32 xPos = (static_cast<f32>(x) - centerOffset) * spacing;
                f32 yPos = (static_cast<f32>(y) - ((13.0f - 1.0f) / 2.0f)) * spacing;
                f32 zPos = (static_cast<f32>(z) - centerOffset) * spacing - 10.0f;
                Math::Matrix4x4 modelMatrix{};
                modelMatrix.TranslateWorld(Math::Vector3(xPos, yPos, zPos));
                initialTransforms.PushBack(modelMatrix);
            }
        }
    }
    for (u16 x = 0; x < 256; ++x) {
        for (u16 y = 0; y < 2; ++y) {
            for (u16 z = 0; z < 256; ++z) {
                f32 xPos = (static_cast<f32>(x) - centerOffset) * spacing;
                f32 yPos = (static_cast<f32>(y) + ((13.0f - 1.0f) / 2.0f)) * spacing;
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
    f32 const numLights = static_cast<f32>(m_PointLights.GetSize());
    f32 horizontalRadius = 30.0f + ((Math::Sin(m_TotalTime * 1.5f) + 0.5f) * 10.0f);
    f32 const ringRotationSpeed = 0.2f;

    for (u8 a = 0; a < m_PointLights.GetSize(); ++a) {
        f32 angle = (static_cast<f32>(a) / numLights) * 2.0f * 3.14159265f;
        f32 currentAngle = angle + (m_TotalTime * ringRotationSpeed);

        f32 xPos = Math::Sin(currentAngle) * horizontalRadius;
        f32 yPos = Math::Sin(m_TotalTime * 1.5f + a) * 2.0f;
        f32 zPos = (Math::Cos(currentAngle) * horizontalRadius) - 10.0f;

        m_PointLights[a]->Transform.SetPosition(Math::Vector3(xPos, yPos, zPos));

        f32 pulseEnergy = 100.0f + (Math::Sin(m_TotalTime * 3.0f + a) * 3.0f);
        m_PointLights[a]->SetEnergy(pulseEnergy);

        f32 r = (Math::Sin(m_TotalTime * 1.0f + a) * 0.5f) + 0.5f;
        f32 g = (Math::Sin(m_TotalTime * 0.7f + a * 2.0f) * 0.5f) + 0.5f;
        f32 b = (Math::Cos(m_TotalTime * 1.2f + a) * 0.5f) + 0.5f;

        m_PointLights[a]->SetColor(Math::Vector3(r, g, b));
    }
    m_Material->SetFloat("u_Time", m_TotalTime);
    m_Cube->Transform.SetRotationEuler(Math::Vector3(
        Math::DegreesToRadians(m_TotalTime * 30.0f),
        Math::DegreesToRadians(m_TotalTime * 45.0f),
        0.0f
    ));
    // DynamicList<Math::Matrix4x4> transforms{};
    // constexpr f32 spacing = 5.0f;
    // constexpr f32 offset = (8.0f - 1.0f) / 2.0f;
    // for (u8 x = 0; x < 16; ++x) {
    //     for (u8 y = 0; y < 16; ++y) {
    //         for (u8 z = 0; z < 16; ++z) {
    //             f32 xPos = (static_cast<f32>(x) - offset) * spacing;
    //             f32 yPos = (static_cast<f32>(y) - offset) * spacing;
    //             f32 zPos = (static_cast<f32>(z) - offset) * spacing - 10.0f;
    //             Math::Matrix4x4 modelMatrix{};
    //             modelMatrix.TranslateWorld(Math::Vector3(xPos, yPos, zPos));
    //             modelMatrix.RotateLocal(Math::DegreesToRadians(m_TotalTime * 10.0f), Math::Vector3::Forward());
    //             transforms.PushBack(modelMatrix);
    //         }
    //     }
    // }
    // m_OrbitingCubes->SetTransforms(transforms);
    // m_OrbitingCubes->UploadTransformsToGPU();
}

void DemoScene::OnRender(Window& window) {
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