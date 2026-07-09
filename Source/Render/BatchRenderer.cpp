#include "Render/BatchRenderer.h"
#include "Render/Material/Material.h"
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
    Math::Matrix4x4 const& viewMatrix = camera->GetViewMatrix();
    Math::Matrix4x4 viewProjection = camera->GetProjectionMatrix() * camera->GetViewMatrix();
    Math::Vector3 const& cameraPos = camera->Transform.GetPosition();

    // Light3D
    RenderGroup<Scene::Light3D>* light3DRenderGroup = GetRenderGroup<Scene::Light3D>();
    DynamicList<Scene::PointLight3D*> activePointLights{};
    for (size_t a = 0; a < light3DRenderGroup->Registered.GetSize(); ++a) {
        Scene::Light3D* light3D = light3DRenderGroup->Registered[a];
        if (light3D != nullptr && light3D->m_IsOn && light3D->IsVisible()) {
            if (light3D->GetLightType() == Scene::LightType::Point) {
                activePointLights.PushBack(static_cast<Scene::PointLight3D*>(light3D));
            }
        }
    }

    std::shared_ptr<Material> lastBoundMaterial = nullptr;
    auto bindCameraAndLights = [&](std::shared_ptr<Material>& material, Math::Matrix4x4 const& modelMat, bool isInstanced) {
        if (material != lastBoundMaterial) {
            material->Bind();
            material->SetMatrix4("u_View", viewMatrix);
            material->SetMatrix4("u_Projection", viewProjection);
            material->SetVector3("u_CameraPosition", cameraPos);
            material->SetInt("u_PointLightCount", static_cast<i32>(activePointLights.GetSize()));
            for (size_t a = 0; a < activePointLights.GetSize() && a < Scene::Light3D::MaxPointLightsPerDrawCall; ++a) {
                std::string prefix = "u_PointLights[" + std::to_string(a) + "].";
                material->SetVector3(prefix + "Position", activePointLights[a]->Transform.GetPosition());
                material->SetVector3(prefix + "Color", activePointLights[a]->m_Color);
                material->SetFloat(prefix + "Energy", activePointLights[a]->m_Energy);
                material->SetFloat(prefix + "Range", activePointLights[a]->m_Range);
                material->SetFloat(prefix + "Constant", activePointLights[a]->m_Constant);
                material->SetFloat(prefix + "Linear", activePointLights[a]->m_Linear);
                material->SetFloat(prefix + "Quadratic", activePointLights[a]->m_Quadratic);
            }
            lastBoundMaterial = material;
        }
        material->SetMatrix4("u_Model", modelMat);
        material->SetFloat("u_IsInstanced", isInstanced ? 1.0f : 0.0f);
    };

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
        bindCameraAndLights(material, meshInstance3D->Transform.GetWorldMatrix(), false);
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
        bindCameraAndLights(material, instancedVisual3D->Transform.GetWorldMatrix(), true);
        instancedVisual3D->Draw();
    }
}

}
