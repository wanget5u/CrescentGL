#include "Render/BatchRenderer.h"

#include <cstring>
#include "Core/Time.h"
#include "glad/glad.h"
#include "Render/Material/Material.h"
#include "Render/SceneData.h"
#include "Scene/Nodes3D/Camera3D.h"
#include "Scene/Nodes3D/Light/PointLight3D.h"
#include "Scene/Nodes3D/Geometry/MeshInstance3D.h"
#include "Scene/Nodes3D/Geometry/InstancedVisual3D.h"
#include "Scene/Nodes3D/Geometry/MultiMeshInstance3D.h"

namespace Crescent::Render {

template <typename T>
void RenderGroup<T>::Register(T* instance, const bool isBatchLoading) {
    if (isBatchLoading) {
        Staged.PushBack(instance);
    } else {
        Registered.PushBack(instance);
    }
}

template <typename T>
void RenderGroup<T>::Unregister(T* instance, const bool isBatchUnloading) {
    if (isBatchUnloading) {
        for (size_t a = 0; a < Registered.GetSize(); ++a) {
            if (Registered[a] == instance) {
                Registered[a] = nullptr;
                break;
            }
        }
    } else {
        Registered.Remove(instance);
        Staged.Remove(instance);
    }
}

template <typename T>
void RenderGroup<T>::FlushLoad() {
    if (Staged.IsEmpty() == false) {
        Registered.Reserve(Registered.GetSize() + Staged.GetSize());
        for (size_t a = 0; a < Staged.GetSize(); ++a) {
            Registered.PushBack(Staged[a]);
        }
        Staged.Clear();
    }
}

template <typename T>
void RenderGroup<T>::FlushUnload() {
    size_t writeIndex = 0;
    for (size_t readIndex = 0; readIndex < Registered.GetSize(); ++readIndex) {
        if (Registered[readIndex] != nullptr) {
            Registered[writeIndex] = Registered[readIndex];
            writeIndex++;
        }
    }
    while (Registered.GetSize() > writeIndex) {
        Registered.PopBack();
    }
}

template <typename T>
void RenderGroup<T>::Clear() {
    Registered.Clear();
    Staged.Clear();
}

template<typename T>
RenderGroup<T>* BatchRenderer::GetRenderGroup() {
    std::type_index typeIndex = typeid(T);
    std::unordered_map<std::type_index, std::unique_ptr<IRenderGroup>>::iterator it
        = m_RenderGroups.find(typeIndex);
    if (it == m_RenderGroups.end()) {
        std::unique_ptr<RenderGroup<T>> renderGroup = std::make_unique<RenderGroup<T>>();
        RenderGroup<T>* ptr = renderGroup.get();
        m_RenderGroups[typeIndex] = std::move(renderGroup);
        return ptr;
    }
    return static_cast<RenderGroup<T>*>(it->second.get());
}

template struct RenderGroup<Scene::MeshInstance3D>;
template struct RenderGroup<Scene::InstancedVisual3D>;
template struct RenderGroup<Scene::Light3D>;
template RenderGroup<Scene::MeshInstance3D>* BatchRenderer::GetRenderGroup<Scene::MeshInstance3D>();
template RenderGroup<Scene::InstancedVisual3D>* BatchRenderer::GetRenderGroup<Scene::InstancedVisual3D>();
template RenderGroup<Scene::Light3D>* BatchRenderer::GetRenderGroup<Scene::Light3D>();

void BatchRenderer::InitializeBuffers() {
    glGenBuffers(1, &m_SceneDataUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_SceneDataUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GPU::SceneData), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_SceneDataUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void BatchRenderer::PrepareFrame(Scene::Camera3D const* camera) {
    GPU::SceneData sceneData{};
    sceneData.View = camera->GetViewMatrix();
    sceneData.Projection = camera->GetProjectionMatrix();
    sceneData.ViewProjection = sceneData.Projection * sceneData.View;
    sceneData.CameraPosition = camera->Transform.GetPosition();
    sceneData.Time = Time::GetTotalTime();

    RenderGroup<Scene::Light3D>* light3DRenderGroup = GetRenderGroup<Scene::Light3D>();
    i32 lightCount{0};
    for (size_t a = 0; a < light3DRenderGroup->Registered.GetSize() && lightCount < 32; ++a) {
        Scene::Light3D* light3D = light3DRenderGroup->Registered[a];
        if (light3D->IsOn() && light3D->IsVisible() && light3D->GetLightType() == Scene::LightType::Point) {
            Scene::PointLight3D* pointLight3D = dynamic_cast<Scene::PointLight3D*>(light3D);
            GPU::PointLightData& pointLightData = sceneData.PointLights[lightCount];
            pointLightData.Position = pointLight3D->Transform.GetPosition();
            pointLightData.Range = pointLight3D->GetRange();
            pointLightData.Color = pointLight3D->GetColor();
            pointLightData.Energy = pointLight3D->GetEnergy();
            pointLightData.Constant = pointLight3D->GetConstant();
            pointLightData.Linear = pointLight3D->GetLinear();
            pointLightData.Quadratic = pointLight3D->GetQuadratic();
            lightCount++;
        }
    }
    sceneData.PointLightCount = lightCount;
    glBindBuffer(GL_UNIFORM_BUFFER, m_SceneDataUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GPU::SceneData), &sceneData);
}

void BatchRenderer::BeginBatchLoad() {
    m_IsBatchLoading = true;
}

void BatchRenderer::EndBatchLoad() {
    m_IsBatchLoading = false;
    for (std::pair<const std::type_index, std::unique_ptr<IRenderGroup>>& renderGroup : m_RenderGroups) {
        renderGroup.second->FlushLoad();
    }
}

void BatchRenderer::BeginBatchUnload() {
    m_IsBatchUnloading = true;
}

void BatchRenderer::EndBatchUnload() {
    m_IsBatchUnloading = false;
    for (std::pair<const std::type_index, std::unique_ptr<IRenderGroup>>& renderGroup : m_RenderGroups) {
        renderGroup.second->FlushUnload();
    }
}

void BatchRenderer::Clear() {
    for (std::pair<const std::type_index, std::unique_ptr<IRenderGroup>>& renderGroup : m_RenderGroups) {
        renderGroup.second->Clear();
    }
    m_RenderGroups.clear();
}

void BatchRenderer::RenderScene(Scene::Camera3D const* camera) {
    PrepareFrame(camera);

    // MeshInstance3D
    RenderGroup<Scene::MeshInstance3D>* meshInstance3DRenderGroup = GetRenderGroup<Scene::MeshInstance3D>();
    for (size_t a = 0; a < meshInstance3DRenderGroup->Registered.GetSize(); ++a) {
        Scene::MeshInstance3D* meshInstance3D = meshInstance3DRenderGroup->Registered[a];
        if (meshInstance3D == nullptr || meshInstance3D->IsVisible() == false) {
            continue;
        }
        std::shared_ptr<Material> material = meshInstance3D->GetMaterialOverride();
        if (material == nullptr) {
            continue;
        }
        material->Bind();
        material->SetBool("u_IsInstanced", false);
        material->SetMatrix4("u_View", camera->GetViewMatrix());
        material->SetMatrix4("u_Projection", camera->GetProjectionMatrix());
        meshInstance3D->Draw(meshInstance3D->Transform.GetWorldMatrix());
    }

    // InstancedVisual3D
    RenderGroup<Scene::InstancedVisual3D>* instancedVisual3DRenderGroup = GetRenderGroup<Scene::InstancedVisual3D>();
    for (size_t a = 0; a < instancedVisual3DRenderGroup->Registered.GetSize(); ++a) {
        Scene::InstancedVisual3D* instancedVisual3D = instancedVisual3DRenderGroup->Registered[a];
        if (instancedVisual3D == nullptr || instancedVisual3D->IsVisible() == false) {
            continue;
        }
        std::shared_ptr<Material> material = instancedVisual3D->GetMaterialOverride();
        if (material == nullptr) {
            continue;
        }
        material->Bind();
        material->SetBool("u_IsInstanced", true);
        material->SetMatrix4("u_View", camera->GetViewMatrix());
        material->SetMatrix4("u_Projection", camera->GetProjectionMatrix());
        instancedVisual3D->Draw();
    }
}

}
