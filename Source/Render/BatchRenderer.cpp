#include <algorithm>
#include "Render/BatchRenderer.h"

#include "Core/Time.h"
#include "glad/glad.h"
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

namespace Crescent {

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

void BatchRenderer::PrepareFrame(Camera3D const* camera) {
    GPUDisposalQueue::Flush();
    RenderStats::Instance().Reset();
    GPU::RenderData renderData{};
    renderData.View = camera->GetViewMatrix();
    renderData.Projection = camera->GetProjectionMatrix();
    renderData.ViewProjection = renderData.Projection * renderData.View;
    renderData.CameraPosition = camera->Transform.GetPosition();
    renderData.Time = Time::GetTotalTime();

    u16 lightCount{0};
    ResolveLight3DRenderGroup(renderData, lightCount);

    renderData.PointLightCount = lightCount;
    glBindBuffer(GL_UNIFORM_BUFFER, m_SceneDataUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GPU::RenderData), &renderData);
    m_OpaquePackets.Clear();
    m_TransparentPackets.Clear();
    ResolveMeshInstance3DRenderGroup(camera->Transform.GetPosition());

    if (m_OpaquePackets.GetSize() > 1) {
        std::sort(m_OpaquePackets.GetData(), m_OpaquePackets.GetData() + m_OpaquePackets.GetSize(),
            [](RenderPacket const& a, RenderPacket const& b) {return a.SortKey < b.SortKey;});
    }

    if (m_TransparentPackets.GetSize() > 1) {
        std::sort(m_TransparentPackets.GetData(), m_TransparentPackets.GetData() + m_TransparentPackets.GetSize(),
            [](RenderPacket const& a, RenderPacket const& b) {return a.DistanceToCamera > b.DistanceToCamera;});
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
    m_OpaquePackets.Clear();
    m_TransparentPackets.Clear();
}

void BatchRenderer::RenderScene(Camera3D const* camera) {
    StartGPUQuery();
    PrepareFrame(camera);

    auto renderPacketList = [](DynamicList<RenderPacket> const& packets, u16& lastShaderID, u16& lastMaterialID) {
        for (size_t a = 0; a < packets.GetSize(); ++a) {
            RenderPacket const& packet = packets[a];

            if (packet.MeshObject == nullptr || packet.MaterialObject == nullptr) {
                continue;
            }

            u16 shaderID = static_cast<u16>((packet.SortKey >> 40) & 0xFFFF);
            u16 materialID = static_cast<u16>((packet.SortKey >> 24) & 0xFFFF);

            if (shaderID != lastShaderID || materialID != lastMaterialID) {
                packet.MaterialObject->Bind();

                if (std::shared_ptr<ShaderAsset> activeShader = packet.MaterialObject->GetActiveShader();
                    activeShader != nullptr &&
                    activeShader->IsReady == true &&
                    activeShader->ShaderObject != nullptr) {
                    activeShader->ShaderObject->TrySetBool("u_IsInstanced", false);
                }

                lastShaderID = shaderID;
                lastMaterialID = materialID;
            }

            if (std::shared_ptr<ShaderAsset> activeShader = packet.MaterialObject->GetActiveShader();
                activeShader != nullptr &&
                activeShader->IsReady == true &&
                activeShader->ShaderObject != nullptr) {
                activeShader->ShaderObject->TrySetMatrix4("u_Model", packet.WorldMatrix);
            }

            packet.MeshObject->Bind();
            packet.MeshObject->Draw();
        }
    };

    u16 lastShaderID = 0xFFFF;
    u16 lastMaterialID = 0xFFFF;
    renderPacketList(m_OpaquePackets, lastShaderID, lastMaterialID);

    if (m_TransparentPackets.GetSize() > 0) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        renderPacketList(m_TransparentPackets, lastShaderID, lastMaterialID);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    // InstancedVisual3D
    RenderGroup<InstancedVisual3D>* instancedVisual3DRenderGroup = GetRenderGroup<InstancedVisual3D>();
    for (size_t a = 0; a < instancedVisual3DRenderGroup->Registered.GetSize(); ++a) {
        InstancedVisual3D* instancedVisual3D = instancedVisual3DRenderGroup->Registered[a];

        if (instancedVisual3D == nullptr || instancedVisual3D->IsVisible() == false) {
            continue;
        }

        std::shared_ptr<Material> material = instancedVisual3D->GetMaterialOverride();
        if (material == nullptr) {
            continue;
        }

        u16 shaderID{0};
        if (material->Shader != nullptr && material->Shader->ShaderObject != nullptr) {
            shaderID = static_cast<u16>(material->Shader->ShaderObject->ID);
        }
        else if (Material::GetDefaultShader() != nullptr &&
                 Material::GetDefaultShader()->ShaderObject != nullptr) {
            shaderID = static_cast<u16>(Material::GetDefaultShader()->ShaderObject->ID);
        }

        u16 materialID = static_cast<u16>(material->ID);
        if (shaderID != lastShaderID || materialID != lastMaterialID) {
            material->Bind();
            lastShaderID = shaderID;
            lastMaterialID = materialID;
        }

        if (std::shared_ptr<ShaderAsset> activeShader = material->GetActiveShader();
            activeShader != nullptr &&
            activeShader->IsReady == true &&
            activeShader->ShaderObject != nullptr) {
            activeShader->ShaderObject->TrySetBool("u_IsInstanced", true);
        }

        instancedVisual3D->Draw();
    }
    EndGPUQuery();
}

void BatchRenderer::StartGPUQuery() {
    m_CurrentQueryIndex = (m_CurrentQueryIndex + 1) % FrameQueryCount;
    u32 currentQuery = m_GPUTimerQueryIDs[m_CurrentQueryIndex];
    glBeginQuery(GL_TIME_ELAPSED, currentQuery);
}

void BatchRenderer::EndGPUQuery() {
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
            RenderStats::Instance().GPUFrameTimeMs = static_cast<f32>(elapsedNanoseconds) * 1e-6f;
        }
    }
}

void BatchRenderer::ResolveLight3DRenderGroup(GPU::RenderData& renderData, u16& lightCount) {
    // filtering light instances:
    // - not visible
    // - off
    RenderGroup<Light3D>* light3DRenderGroup = GetRenderGroup<Light3D>();
    for (size_t a = 0; a < light3DRenderGroup->Registered.GetSize() && lightCount < Light3D::MaxPointLightsPerDrawCall; ++a) {
        Light3D* light3D = light3DRenderGroup->Registered[a];

        if (light3D->IsOn() == false) {
            continue;
        }

        if (light3D->IsVisible() == false) {
            continue;
        }

        if (light3D->GetLightType() == LightType::Point) {
            PointLight3D* pointLight3D = dynamic_cast<PointLight3D*>(light3D);
            GPU::PointLightData& pointLightData = renderData.PointLights[lightCount];

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
}

void BatchRenderer::ResolveMeshInstance3DRenderGroup(Math::Vector3 const& cameraPosition) {
    // filtering mesh instances:
    // - nullptr
    // - hidden
    // - no material
    // - no valid geometry
    RenderGroup<MeshInstance3D>* meshInstance3DRenderGroup = GetRenderGroup<MeshInstance3D>();
    for (size_t a = 0; a < meshInstance3DRenderGroup->Registered.GetSize(); ++a) {
        MeshInstance3D* meshInstance3D = meshInstance3DRenderGroup->Registered[a];

        if (meshInstance3D == nullptr || meshInstance3D->IsVisible() == false) {
            continue;
        }

        std::shared_ptr<Material> materialOverride = meshInstance3D->GetMaterialOverride();
        if (materialOverride == nullptr) {
            continue;
        }

        Mesh* meshObject = meshInstance3D->GetMesh();
        if (meshObject == nullptr) {
            continue;
        }

        f32 distanceToCamera = (meshInstance3D->Transform.GetPosition() - cameraPosition).Length();

        RenderPacket packet{};
        packet.MeshObject = meshObject;
        packet.MaterialObject = materialOverride.get();
        packet.WorldMatrix = meshInstance3D->Transform.GetWorldMatrix();
        packet.DistanceToCamera = distanceToCamera;
        packet.IsTransparent = (materialOverride->TintColor.w < 1.0f);

        u8 layer = meshInstance3D->GetLayerMask();
        u16 shaderID{0};

        if (materialOverride->Shader != nullptr && materialOverride->Shader->ShaderObject != nullptr) {
            shaderID = static_cast<u16>(materialOverride->Shader->ShaderObject->ID);
        }
        else if (Material::GetDefaultShader() != nullptr && Material::GetDefaultShader()->ShaderObject != nullptr) {
            shaderID = static_cast<u16>(Material::GetDefaultShader()->ShaderObject->ID);
        }

        u16 materialID = static_cast<u16>(materialOverride->ID);
        u16 meshVAO = static_cast<u16>(meshObject->GetVAO());
        u8 depth = static_cast<u8>(std::clamp(distanceToCamera * 2.0f, 0.0f, 255.0f));

        packet.SortKey = (static_cast<u64>(layer) << 56) |
                         (static_cast<u64>(shaderID) << 40) |
                         (static_cast<u64>(materialID) << 24) |
                         (static_cast<u64>(meshVAO) << 8) |
                          static_cast<u64>(depth);

        if (packet.IsTransparent == true) {
            m_TransparentPackets.PushBack(packet);
        }
        else {
            m_OpaquePackets.PushBack(packet);
        }
    }
}

}
