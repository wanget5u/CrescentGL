#include <algorithm>
#include "Render/BatchRenderer.h"

#include "Core/Time.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Render/RenderStats.h"
#include "Render/Material/Material.h"
#include "Render/Shader/Shader.h"
#include "Render/RenderData.h"
#include "Render/GPUDisposalQueue.h"
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
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GPU::RenderData), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_SceneDataUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void BatchRenderer::PrepareFrame(Scene::Camera3D const* camera) {
    GPUDisposalQueue::Flush();
    Stats::Instance().Reset();
    GPU::RenderData sceneData{};
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
            Math::Matrix4x4 const& worldMat = pointLight3D->Transform.GetWorldMatrix();
            pointLightData.Position = Math::Vector3(worldMat.column3.x, worldMat.column3.y, worldMat.column3.z);
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
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GPU::RenderData), &sceneData);

    m_RenderPackets.Clear();
    RenderGroup<Scene::MeshInstance3D>* meshInstance3DRenderGroup = GetRenderGroup<Scene::MeshInstance3D>();
    Math::Vector3 const camPos = camera->Transform.GetPosition();
    for (size_t a = 0; a < meshInstance3DRenderGroup->Registered.GetSize(); ++a) {
        Scene::MeshInstance3D* meshInstance3D = meshInstance3DRenderGroup->Registered[a];
        if (meshInstance3D == nullptr || meshInstance3D->IsVisible() == false) {
            continue;
        }
        std::shared_ptr<Material> materialPtr = meshInstance3D->GetMaterialOverride();
        if (materialPtr == nullptr) {
            continue;
        }
        Mesh* gpuMesh = meshInstance3D->GetMesh();
        if (gpuMesh == nullptr) {
            continue;
        }
        RenderPacket packet{};
        packet.MeshObject = gpuMesh;
        packet.MaterialObject = materialPtr.get();
        packet.WorldMatrix = meshInstance3D->Transform.GetWorldMatrix();

        u8 layer = meshInstance3D->GetLayerMask();
        u16 shaderID = 0;
        if (materialPtr->ShaderAsset != nullptr && materialPtr->ShaderAsset->ShaderObject != nullptr) {
            shaderID = static_cast<u16>(materialPtr->ShaderAsset->ShaderObject->ID);
        } else if (Material::GetDefaultShader() != nullptr && Material::GetDefaultShader()->ShaderObject != nullptr) {
            shaderID = static_cast<u16>(Material::GetDefaultShader()->ShaderObject->ID);
        }
        u16 materialID = static_cast<u16>(materialPtr->ID);
        u16 meshVAO = static_cast<u16>(gpuMesh->GetVAO());
        f32 dist = (meshInstance3D->Transform.GetPosition() - camPos).Length();
        u8 depth = static_cast<u8>(std::clamp(dist * 2.0f, 0.0f, 255.0f));

        packet.SortKey = (static_cast<u64>(layer) << 56) |
                         (static_cast<u64>(shaderID) << 40) |
                         (static_cast<u64>(materialID) << 24) |
                         (static_cast<u64>(meshVAO) << 8) |
                         static_cast<u64>(depth);
        m_RenderPackets.PushBack(packet);
    }
    if (m_RenderPackets.GetSize() > 1) {
        std::sort(m_RenderPackets.GetData(), m_RenderPackets.GetData() + m_RenderPackets.GetSize(),
            [](RenderPacket const& a, RenderPacket const& b) {return a.SortKey < b.SortKey;});
    }
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
    m_RenderPackets.Clear();
}

void BatchRenderer::RenderScene(Scene::Camera3D const* camera) {
    m_CurrentQueryIndex = (m_CurrentQueryIndex + 1) % FrameQueryCount;
    u32 currentQuery = m_GPUTimerQueryIDs[m_CurrentQueryIndex];
    glBeginQuery(GL_TIME_ELAPSED, currentQuery);
    PrepareFrame(camera);

    u16 lastShaderID = 0xFFFF;
    u16 lastMaterialID = 0xFFFF;
    for (size_t a = 0; a < m_RenderPackets.GetSize(); ++a) {
        RenderPacket const& packet = m_RenderPackets[a];
        if (packet.MeshObject == nullptr || packet.MaterialObject == nullptr) {
            continue;
        }
        u16 shaderID = static_cast<u16>((packet.SortKey >> 40) & 0xFFFF);
        u16 materialID = static_cast<u16>((packet.SortKey >> 24) & 0xFFFF);
        if (shaderID != lastShaderID || materialID != lastMaterialID) {
            packet.MaterialObject->Bind();
            packet.MaterialObject->TrySetBool("u_IsInstanced", false);
            lastShaderID = shaderID;
            lastMaterialID = materialID;
        }
        packet.MaterialObject->TrySetMatrix4("u_Model", packet.WorldMatrix);
        packet.MeshObject->Bind();
        packet.MeshObject->Draw();
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
        u16 shaderID{0};
        if (material->ShaderAsset != nullptr && material->ShaderAsset->ShaderObject != nullptr) {
            shaderID = static_cast<u16>(material->ShaderAsset->ShaderObject->ID);
        }
        else if (Material::GetDefaultShader() != nullptr && Material::GetDefaultShader()->ShaderObject != nullptr) {
            shaderID = static_cast<u16>(Material::GetDefaultShader()->ShaderObject->ID);
        }
        u16 materialID = static_cast<u16>(material->ID);
        if (shaderID != lastShaderID || materialID != lastMaterialID) {
            material->Bind();
            lastShaderID = shaderID;
            lastMaterialID = materialID;
        }
        material->TrySetBool("u_IsInstanced", true);
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
