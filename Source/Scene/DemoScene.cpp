#include "Scene/DemoScene.h"

#include <glad/glad.h>

#include "imgui.h"
#include "Asset/AssetLoader.h"
#include "Core/Window.h"
#include "Node/Tree.h"
#include "Node/Node3D/Camera3D.h"
#include "Node/Node3D/Geometry/MeshInstance3D.h"
#include "Node/Node3D/Light/DirectionalLight3D.h"
#include "Render/BatchRenderer.h"
#include "Render/Primitives/BoxMesh.h"

namespace Crescent {

DemoScene::DemoScene(std::string const& name) : Scene(name) {
    std::shared_ptr<TextureAsset> albedoTexture =
        AssetLoader::Instance().GetOrLoad<TextureAsset>("MetalPlates006_2K-JPG_Color.jpg", AssetType::Texture);
    std::shared_ptr<TextureAsset> metallicTexture =
        AssetLoader::Instance().GetOrLoad<TextureAsset>("MetalPlates006_2K-JPG_Metalness.jpg", AssetType::Texture);
    std::shared_ptr<TextureAsset> normalTexture =
        AssetLoader::Instance().GetOrLoad<TextureAsset>("MetalPlates006_2K-JPG_NormalGL.jpg", AssetType::Texture);
    std::shared_ptr<TextureAsset> roughnessTexture =
        AssetLoader::Instance().GetOrLoad<TextureAsset>("MetalPlates006_2K-JPG_Roughness.jpg", AssetType::Texture);

    // m_UnlitMaterial->SetVector3("v3_ObjectColor", Math::Vector3(1.0f, 0.5f, 0.5f));
    // m_UnlitMaterial->SetVector3("v3_LightColor", Math::Vector3(1.0f, 1.0f, 1.0f));
    // m_UnlitMaterial->SetVector3("v3_LightPosition", Math::Vector3(1.0f, 1.0f, 1.0f));

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
    m_Material->PreserveUVCoordinates = true;
    m_Material->UVScale = Math::Vector2(0.2f, 0.2f);

    m_BoxMesh = std::make_shared<BoxMesh>(1.0f, 1.0f, 1.0f);

    m_Cube = m_Tree->AddChild<MeshInstance3D>("Cube");
    m_Cube->SetMesh(m_BoxMesh);
    m_Cube->SetMaterialOverride(m_Material);
    m_Cube->Transform.SetScale(3.0f);
    m_Cube->Transform.SetPosition(Math::Vector3(0.0f, 1.5f, 0.0f));

    m_PreviewCamera->Transform.SetPosition(Math::Vector3(0.0f, 2.5f, 30.0f));

    for (u8 a = 0; a < 128; ++a) {
        PointLight3D* light = m_Tree->AddChild<PointLight3D>("PointLight3D");
        light->Transform.SetPosition(Math::Vector3(0.0f, 1.0f, 0.0f));
        light->SetRange(40.0f);
        light->SetEnergy(5.0f);
        light->SetColor(Math::Vector3(1.0f, 0.85f, 0.75f));
        m_PointLights.PushBack(light);
    }

    m_DirectionalLight = m_Tree->AddChild<DirectionalLight3D>("DirectionalLight3D");
    m_DirectionalLight->SetEnergy(1.5f);
    m_DirectionalLight->SetColor(Math::Vector3(1.0f, 0.85f, 0.65f));
    m_DirectionalLight->Transform.SetRotationEulerDegrees(Math::Vector3(-45.0f, -45.0f, 0.0f));

    MeshInstance3D* floor = m_Tree->AddChild<MeshInstance3D>("Floor");
    floor->SetMesh(m_BoxMesh);
    floor->SetMaterialOverride(m_Material);
    floor->Transform.SetScale(Math::Vector3(1024.0f, 0.5f, 1024.0f));
    floor->Transform.SetPosition(Math::Vector3(0.0f, -0.25f, 0.0f));

    //
    // m_OrbitingCubes = m_Tree->AddChild<MultiMeshInstance3D>("OrbitingCubes");
    // m_OrbitingCubes->SetMesh(m_FloorMesh);
    // m_OrbitingCubes->SetMaterial(m_Material);
    //
    // DynamicList<Math::Matrix4x4> initialTransforms{};
    // constexpr f32 spacing = 3.0f;
    // constexpr f32 centerOffset = (128.0f - 1.0f) / 2.0f;
    // constexpr f32 yOffset = (4.0f - 1.0f) / 2.0f;
    // for (u8 x = 0; x < 128; ++x) {
    //     for (u8 y = 0; y < 1; ++y) {
    //         for (u8 z = 0; z < 128; ++z) {
    //             f32 xPos = (static_cast<f32>(x) - centerOffset) * spacing;
    //             f32 yPos = (static_cast<f32>(y) - yOffset);
    //             f32 zPos = (static_cast<f32>(z) - centerOffset) * spacing;
    //             Math::Matrix4x4 modelMatrix{};
    //             modelMatrix.TranslateWorld(Math::Vector3(xPos, yPos, zPos));
    //             initialTransforms.PushBack(modelMatrix);
    //         }
    //     }
    // }
    // m_OrbitingCubes->SetTransforms(std::move(initialTransforms));
    // m_OrbitingCubes->UploadTransformsToGPU();
}

DemoScene::~DemoScene() {}

void DemoScene::OnUpdate(f32 const deltaTime) {
    m_TotalTime += deltaTime;

    const f32 numLights = static_cast<f32>(m_PointLights.GetSize());

    constexpr f32 baseRadius = 40.0f;
    constexpr f32 starDepth = 0.9f;
    constexpr f32 starPoints = 5.0f;
    constexpr f32 ringRotationSpeed = 0.1f;

    for (u8 a = 0; a < m_PointLights.GetSize(); ++a) {
        f32 nativeAngle = (static_cast<f32>(a) / numLights) * 2.0f * 3.14159265f;
        f32 currentAngle = nativeAngle + (m_TotalTime * ringRotationSpeed);

        f32 starModulation = 1.0f + starDepth * Math::Cos(starPoints * currentAngle);
        f32 dynamicRadius = baseRadius * starModulation;

        f32 xPos = Math::Sin(currentAngle) * dynamicRadius;
        f32 zPos = Math::Cos(currentAngle) * dynamicRadius;

        m_PointLights[a]->Transform.SetPosition(Math::Vector3(xPos, 1.0f, zPos));

        f32 r = 0.15f + (Math::Sin(m_TotalTime * 1.0f + a) * 0.425f) + 0.425f;
        f32 g = 0.15f + (Math::Sin(m_TotalTime * 0.7f + a * 2.0f) * 0.425f) + 0.425f;
        f32 b = 0.15f + (Math::Cos(m_TotalTime * 1.2f + a) * 0.425f) + 0.425f;

        m_PointLights[a]->SetColor(Math::Vector3(r, g, b));
    }
    m_Material->TrySetFloat("f32_Time", m_TotalTime);
}

void DemoScene::OnRender(Window& window) {
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
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

void DemoScene::OnRenderGUI() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 windowPosition = ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y / 2.0f);
    ImVec2 windowPivot = ImVec2(0.0f, 1.0f);
    ImGui::SetNextWindowPos(windowPosition, ImGuiCond_Always, windowPivot);
    ImGui::SetNextWindowSize(ImVec2(450, 0));
    ImGui::SetNextWindowBgAlpha(0.9f);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoSavedSettings;

    if (ImGui::Begin("Light Controls", nullptr, flags)) {
        ImGui::Text("Directional Light");
        ImGui::Separator();
        f32 lightEnergy = m_DirectionalLight->GetEnergy();
        if (ImGui::SliderFloat("Energy", &lightEnergy, 0.0f, 10.0f, "%.2f") == true) {
            m_DirectionalLight->SetEnergy(lightEnergy);
        }
        Math::Vector3 color = m_DirectionalLight->GetColor();
        f32 colorArray[3] = { color.x, color.y, color.z };
        if (ImGui::ColorEdit3("Color", colorArray) == true) {
            m_DirectionalLight->SetColor(Math::Vector3(colorArray[0], colorArray[1], colorArray[2]));
        }
        Math::Vector3 s_InspectorEulerDegrees = m_DirectionalLight->Transform.GetRotationEulerDegrees();
        if (ImGui::IsItemActive() == false && ImGui::IsItemEdited()  == false) {
            s_InspectorEulerDegrees = m_DirectionalLight->Transform.GetRotationEulerDegrees();
        }
        f32 rotationArray[3] = { s_InspectorEulerDegrees.x, s_InspectorEulerDegrees.y, s_InspectorEulerDegrees.z };
        if (ImGui::SliderFloat3("Rotation", rotationArray, -180.0f, 180.0f, "%.1f") == true) {
            s_InspectorEulerDegrees = Math::Vector3(rotationArray[0], rotationArray[1], rotationArray[2]);
            m_DirectionalLight->Transform.SetRotationEulerDegrees(s_InspectorEulerDegrees);
        }
    }
    ImGui::End();
}

}
