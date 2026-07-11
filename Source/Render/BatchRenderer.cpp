#include "Render/BatchRenderer.h"

#include "Core/Time.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Render/RenderStats.h"
#include "Render/Material/Material.h"
#include "Render/SceneData.h"
#include "Scene/Nodes3D/Camera3D.h"
#include "Scene/Nodes3D/Light/PointLight3D.h"
#include "Scene/Nodes3D/Geometry/MeshInstance3D.h"
#include "Scene/Nodes3D/Geometry/InstancedVisual3D.h"
#include "Scene/Nodes3D/Geometry/MultiMeshInstance3D.h"

namespace Crescent::Render {

BatchRenderer::BatchRenderer() {
    glGenQueries(FrameQueryCount, m_GPUTimerQueryIDs);
}

BatchRenderer::~BatchRenderer() {
    glDeleteQueries(FrameQueryCount, m_GPUTimerQueryIDs);
    for (u32& m_GPUTimerQueryID : m_GPUTimerQueryIDs) {
        m_GPUTimerQueryID = 0;
    }
    if (m_SceneDataUBO != 0) {
        glDeleteBuffers(1, &m_SceneDataUBO);
        m_SceneDataUBO = 0;
    }
}

void BatchRenderer::InitializeBuffers() {
    glGenBuffers(1, &m_SceneDataUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_SceneDataUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GPU::SceneData), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_SceneDataUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void BatchRenderer::PrepareFrame(Scene::Camera3D const* camera) {
    Stats::Instance().Reset();
    GPU::SceneData sceneData{};
    sceneData.View = camera->GetViewMatrix();
    sceneData.Projection = camera->GetProjectionMatrix();
    sceneData.ViewProjection = sceneData.Projection * sceneData.View;
    sceneData.CameraPosition = camera->Transform.GetPosition();
    sceneData.Time = Time::GetTotalTime();

    RenderGroup<Scene::Light3D>* light3DRenderGroup = GetRenderGroup<Scene::Light3D>();
    i32 lightCount{0};
    for (size_t a = 0; a < light3DRenderGroup->Registered.GetSize() && lightCount < Scene::Light3D::MaxPointLightsPerDrawCall; ++a) {
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
    m_CurrentQueryIndex = (m_CurrentQueryIndex + 1) % FrameQueryCount;
    u32 currentQuery = m_GPUTimerQueryIDs[m_CurrentQueryIndex];
    glBeginQuery(GL_TIME_ELAPSED, currentQuery);
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
    glEndQuery(GL_TIME_ELAPSED);
    m_QueryIssued[m_CurrentQueryIndex] = true;
    u32 oldestIndex = (m_CurrentQueryIndex + 1) % FrameQueryCount;
    if (m_QueryIssued[oldestIndex] == true) {
        u32 oldestQuery = m_GPUTimerQueryIDs[oldestIndex];
        GLint available = 0;
        glGetQueryObjectiv(oldestQuery, GL_QUERY_RESULT_AVAILABLE, &available);
        if (available != 0) {
            u64 elapsedNanoseconds = 0;
            glGetQueryObjectui64v(oldestQuery, GL_QUERY_RESULT, &elapsedNanoseconds);
            Stats::Instance().GPUFrameTimeMs = static_cast<f32>(elapsedNanoseconds) * 1e-6f;
        }
    }
}

}
