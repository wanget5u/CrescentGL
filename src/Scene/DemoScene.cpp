#include "Scene/DemoScene.h"

#include <glad/glad.h>

#include "Asset/Loader.h"
#include "Asset/Registry.h"
#include "Math/Math.h"
#include "Render/Primitives/BoxMesh.h"
#include "Render/Shader/Shader.h"
#include "Scene/Nodes3D/Camera3D.h"
#include "Scene/Nodes3D/MeshInstance3D.h"
#include "Scene/Nodes3D/MultiMeshInstance3D.h"
#include "Scene/Tree.h"
#include "Core/Window.h"
#include "Render/BatchRenderer.h"

namespace Crescent::Scene {

constexpr u32 INSTANCE_COUNT = 524288;

DemoScene::DemoScene() {
    // shaders
    std::shared_ptr<Asset::Shader> shaderAsset =
        Asset::Registry::Instance()
        .GetOrLoad<Asset::Shader>("Shaders/demo", Asset::Type::Shader);
    // materials
    m_Material = std::make_shared<Render::Material>(shaderAsset);
    m_Material->AlbedoTexture =
        Asset::Registry::Instance()
        .GetOrLoad<Asset::Texture>("Assets/Textures/Tiles081_1K-JPG_Color.jpg", Asset::Type::Texture);
    // meshes
    m_BoxMesh = std::make_shared<Render::BoxMesh>(1.0f, 1.0f, 1.0f);

    m_CubeNode = m_Tree->AddChild<MeshInstance3D>("Cube");
    m_CubeNode->SetMesh(m_BoxMesh);
    m_CubeNode->SetMaterial(m_Material);
    m_CubeNode->Transform.SetScale(2.5f);

    m_OrbitingCubesNode = m_Tree->AddChild<MultiMeshInstance3D>("OrbitingCubes");
    m_OrbitingCubesNode->SetMesh(m_BoxMesh);
    m_OrbitingCubesNode->SetMaterial(m_Material);
    // init
    DynamicList<Math::Matrix4x4> initialTransforms{};
    initialTransforms.Reserve(INSTANCE_COUNT);
    for (u32 a = 0; a < INSTANCE_COUNT; ++a) {
        initialTransforms.PushBack(Math::Matrix4x4::GetIdentity());
    }
    m_OrbitingCubesNode->SetTransforms(std::move(initialTransforms));

    constexpr f32 spacing = 10.0f;
    constexpr f32 centerOffset = (64.0f - 1.0f) / 2.0f;
    for (u8 x = 0; x < 64; ++x) {
        for (u8 y = 0; y < 64; ++y) {
            for (u8 z = 0; z < 64; ++z) {
                f32 xPos = (static_cast<f32>(x) - centerOffset) * spacing;
                f32 yPos = (static_cast<f32>(y) - centerOffset) * spacing;
                f32 zPos = (static_cast<f32>(z) - centerOffset) * spacing - 10.0f;
                Math::Matrix4x4 modelMatrix{};
                modelMatrix.TranslateWorld(Math::Vector3(xPos, yPos, zPos));
                initialTransforms.PushBack(modelMatrix);
            }
        }
    }
    m_OrbitingCubesNode->SetTransforms(std::move(initialTransforms));
    m_OrbitingCubesNode->UploadTransformsToGPU();
}

DemoScene::~DemoScene() {}

void DemoScene::OnUpdate(f32 const deltaTime) {
    m_TotalTime += deltaTime;
    m_Material->SetFloat("u_Time", m_TotalTime);
    m_CubeNode->Transform.SetRotationEuler(Math::Vector3(
        Math::DegreesToRadians(m_TotalTime * 30.0f),
        Math::DegreesToRadians(m_TotalTime * 45.0f),
        0.0f
    ));
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