#include "Render/BatchRenderer.h"

#include <algorithm>

#include "Core/Time.h"
#include "glad/glad.h"
#include "Node/Node3D/Camera3D.h"
#include "Node/Node3D/Geometry/MeshInstance3D.h"
#include "Node/Node3D/Geometry/MultiMeshInstance3D.h"
#include "Node/Node3D/Light/PointLight3D.h"
#include "Node/Node3D/Light/DirectionalLight3D.h"
#include "Render/RenderStats.h"
#include "Render/SceneRenderData.h"
#include "Render/Material/Material.h"

namespace Crescent {

BatchRenderer::BatchRenderer() {
    glGenQueries(FrameQueryCount, m_GPUTimerQueryIDs);
}

BatchRenderer::~BatchRenderer() {
    glDeleteQueries(FrameQueryCount, m_GPUTimerQueryIDs);
    for (u32& m_GPUTimerQueryID : m_GPUTimerQueryIDs) {
        m_GPUTimerQueryID = 0;
    }
    // UBO (binding 0) - Default SceneRenderData
    if (s_SceneUBO != 0) {
        glDeleteBuffers(1, &s_SceneUBO);
        s_SceneUBO = 0;
    }
    // SSBO (binding 1) - PointLight3D
    if (PointLight3D::s_PointLightSSBO != 0) {
        glDeleteBuffers(1, &PointLight3D::s_PointLightSSBO);
        PointLight3D::s_PointLightSSBO = 0;
        PointLight3D::s_PointLightSSBOCapacity = 0;
    }
    // SSBO (binding 2 - DirectionalLight3D
    if (DirectionalLight3D::s_DirectionalLightSSBO != 0) {
        glDeleteBuffers(1, &DirectionalLight3D::s_DirectionalLightSSBO);
        DirectionalLight3D::s_DirectionalLightSSBO = 0;
        DirectionalLight3D::s_DirectionalLightSSBOCapacity = 0;
    }
    // SSBO (binding 3) - MultiMesh3D
    if (InstancedVisual3D::s_InstancedVisualSSBO != 0) {
        glDeleteBuffers(1, &InstancedVisual3D::s_InstancedVisualSSBO);
        InstancedVisual3D::s_InstancedVisualSSBO = 0;
        InstancedVisual3D::s_InstancedVisualSSBOCapacity = 0;
    }
    // UBO (binding 4) - Material
    if (Material::s_MaterialUBO != 0) {
        glDeleteBuffers(1, &Material::s_MaterialUBO);
        Material::s_MaterialUBO = 0;
    }
}

void BatchRenderer::InitializeBuffers() {
    // UBO (binding 0) - Default SceneRenderData
    glGenBuffers(1, &s_SceneUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, s_SceneUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GPU::SceneRenderData), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, s_SceneUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // SSBO (binding 1) - PointLight3D
    PointLight3D::s_PointLightSSBOCapacity = sizeof(GPU::PointLightRenderData) * 1024;
    glGenBuffers(1, &PointLight3D::s_PointLightSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, PointLight3D::s_PointLightSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, PointLight3D::s_PointLightSSBOCapacity, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, PointLight3D::s_PointLightSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    // SSBO (binding 2) - DirectionalLight3D
    DirectionalLight3D::s_DirectionalLightSSBOCapacity = sizeof(GPU::DirectionalLightRenderData) * 16;
    glGenBuffers(1, &DirectionalLight3D::s_DirectionalLightSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, DirectionalLight3D::s_DirectionalLightSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, DirectionalLight3D::s_DirectionalLightSSBOCapacity, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, DirectionalLight3D::s_DirectionalLightSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    // SSBO (binding 3) - MultiMesh3D
    InstancedVisual3D::s_InstancedVisualSSBOCapacity = sizeof(GPU::InstanceRenderData) * 512;
    glGenBuffers(1, &InstancedVisual3D::s_InstancedVisualSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, InstancedVisual3D::s_InstancedVisualSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, InstancedVisual3D::s_InstancedVisualSSBOCapacity, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, InstancedVisual3D::s_InstancedVisualSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    // UBO (binding 4) - Material
    glGenBuffers(1, &Material::s_MaterialUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, Material::s_MaterialUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GPU::MaterialRenderData), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 4, Material::s_MaterialUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void BatchRenderer::Clear() {
    for (std::pair<const std::type_index, std::unique_ptr<IRenderGroup>>& renderGroup : m_RenderGroups) {
        renderGroup.second->Clear();
    }
    m_RenderGroups.clear();
    m_OpaquePackets.Clear();
    m_TransparentPackets.Clear();
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

void BatchRenderer::RenderScene(Camera3D const* camera) {
    u16 lastShaderID{0};
    u16 lastMaterialID{0};
    StartGPUQuery();
    // Get all the necessary GPU::SceneRenderData
    PrepareFrame(camera);
    // Handles Opaque meshes
    RenderPackets(m_OpaquePackets, lastShaderID, lastMaterialID);
    // Handle Back-to-front transparent meshes
    if (m_TransparentPackets.GetSize() > 0) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        RenderPackets(m_TransparentPackets, lastShaderID, lastMaterialID);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
    RenderInstancedGroups(lastShaderID, lastMaterialID);
    EndGPUQuery();
}

void BatchRenderer::PrepareFrame(Camera3D const* camera) {
    GPUDisposalQueue::Flush();
    RenderStats::Instance().Reset();
    m_OpaquePackets.Clear();
    m_TransparentPackets.Clear();

    GPU::SceneRenderData renderData{};
    renderData.View = camera->GetViewMatrix();
    renderData.Projection = camera->GetProjectionMatrix();
    renderData.ViewProjection = renderData.Projection * renderData.View;
    renderData.CameraPosition = camera->Transform.GetPosition();
    renderData.Time = Time::GetTotalTime();

    // Light resolution & SSBO upload
    DynamicList<GPU::PointLightRenderData> pointLights{};
    pointLights.Reserve(1024);
    DynamicList<GPU::DirectionalLightRenderData> directionalLights{};
    directionalLights.Reserve(16);
    ResolveLight3DRenderGroup(pointLights, directionalLights);
    renderData.PointLightCount = static_cast<u32>(pointLights.GetSize());
    renderData.DirectionalLightCount = static_cast<u32>(directionalLights.GetSize());

    glBindBuffer(GL_UNIFORM_BUFFER, s_SceneUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GPU::SceneRenderData), &renderData);

    if (pointLights.IsEmpty() == false && PointLight3D::s_PointLightSSBO != 0) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, PointLight3D::s_PointLightSSBO);
        const size_t requiredBytes = pointLights.GetSizeInBytes();
        if (PointLight3D::s_PointLightSSBOCapacity < requiredBytes) {
            glBufferData(GL_SHADER_STORAGE_BUFFER, requiredBytes, pointLights.GetData(), GL_DYNAMIC_DRAW);
            PointLight3D::s_PointLightSSBOCapacity = requiredBytes;
        }
        else {
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, requiredBytes, pointLights.GetData());
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
    if (directionalLights.IsEmpty() == false && DirectionalLight3D::s_DirectionalLightSSBO != 0) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, DirectionalLight3D::s_DirectionalLightSSBO);
        const size_t requiredBytes = directionalLights.GetSizeInBytes();
        if (DirectionalLight3D::s_DirectionalLightSSBOCapacity < requiredBytes) {
            glBufferData(GL_SHADER_STORAGE_BUFFER, requiredBytes, directionalLights.GetData(), GL_DYNAMIC_DRAW);
            DirectionalLight3D::s_DirectionalLightSSBOCapacity = requiredBytes;
        }
        else {
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, requiredBytes, directionalLights.GetData());
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
    ResolveMeshInstance3DRenderGroup(camera->Transform.GetPosition());
    // Sorting render packets for optimal draw calls and ensuring correct depth order
    SortPackets();
}

void BatchRenderer::ResolveLight3DRenderGroup(DynamicList<GPU::PointLightRenderData>& pointLights, DynamicList<GPU::DirectionalLightRenderData>& directionalLights) {
    RenderGroup<Light3D>* light3DRenderGroup = GetRenderGroup<Light3D>();
    for (size_t a = 0; a < light3DRenderGroup->Registered.GetSize(); ++a) {
        Light3D* light3D = light3DRenderGroup->Registered[a];

        if (light3D->IsOn() == false || light3D->IsVisible() == false) {
            continue;
        }
        if (light3D->GetLightType() == LightType::Point) {
            PointLight3D* pointLight3D = dynamic_cast<PointLight3D*>(light3D);
            GPU::PointLightRenderData pointLightData = GPU::PointLightRenderData();

            pointLightData.Position = pointLight3D->Transform.GetPosition();
            pointLightData.Range = pointLight3D->GetRange();
            pointLightData.Color = pointLight3D->GetColor();
            pointLightData.Energy = pointLight3D->GetEnergy();
            pointLightData.Constant = pointLight3D->GetConstant();
            pointLightData.Linear = pointLight3D->GetLinear();
            pointLightData.Quadratic = pointLight3D->GetQuadratic();
            pointLights.PushBack(pointLightData);
        }
        else if (light3D->GetLightType() == LightType::Directional) {
            DirectionalLight3D* directionalLight3D = dynamic_cast<DirectionalLight3D*>(light3D);
            GPU::DirectionalLightRenderData directionalLightData = GPU::DirectionalLightRenderData();

            directionalLightData.Direction = directionalLight3D->Transform.GetForward();
            directionalLightData.Energy = directionalLight3D->GetEnergy();
            directionalLightData.Color = directionalLight3D->GetColor();
            directionalLightData.CascadeCount = 0;
            directionalLights.PushBack(directionalLightData);
        }
    }
}

void BatchRenderer::ResolveMeshInstance3DRenderGroup(Math::Vector3 const& cameraPosition) {
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

        packet.SortKey = static_cast<u64>(layer) << 56 | static_cast<u64>(shaderID) << 40 |
                         static_cast<u64>(materialID) << 24 | static_cast<u64>(meshVAO) << 8 |
                         static_cast<u64>(depth);

        if (packet.IsTransparent == true) {
            m_TransparentPackets.PushBack(packet);
        }
        else {
            m_OpaquePackets.PushBack(packet);
        }
    }
}

void BatchRenderer::SortPackets() {
    if (m_OpaquePackets.GetSize() > 1) {
        std::sort(m_OpaquePackets.GetData(), m_OpaquePackets.GetData() + m_OpaquePackets.GetSize(),
            [](RenderPacket const& a, RenderPacket const& b) {
                return a.SortKey < b.SortKey;
            });
    }
    if (m_TransparentPackets.GetSize() > 1) {
        std::sort(m_TransparentPackets.GetData(), m_TransparentPackets.GetData() + m_TransparentPackets.GetSize(),
            [](RenderPacket const& a, RenderPacket const& b) {
                return a.DistanceToCamera > b.DistanceToCamera;
            });
    }
}

void BatchRenderer::RenderPackets(DynamicList<RenderPacket> const& packets, u16& lastShaderID, u16& lastMaterialID) const {
    for (size_t a = 0; a < packets.GetSize(); ++a) {
        RenderPacket const& packet = packets[a];

        if (packet.MeshObject == nullptr || packet.MaterialObject == nullptr) {
            continue;
        }
        u16 shaderID = static_cast<u16>((packet.SortKey >> 40) & 0xFFFF);
        u16 materialID = static_cast<u16>((packet.SortKey >> 24) & 0xFFFF);

        if (shaderID != lastShaderID || materialID != lastMaterialID) {
            packet.MaterialObject->Bind();
            UploadMaterialData(packet.MaterialObject);

            if (std::shared_ptr<ShaderAsset> activeShader = packet.MaterialObject->GetActiveShader();
                activeShader != nullptr &&
                activeShader->IsReady == true &&
                activeShader->ShaderObject != nullptr) {
                activeShader->ShaderObject->TrySetBool("b_IsInstanced", false);
            }
            lastShaderID = shaderID;
            lastMaterialID = materialID;
        }
        if (std::shared_ptr<ShaderAsset> activeShader = packet.MaterialObject->GetActiveShader();
            activeShader != nullptr &&
            activeShader->IsReady == true &&
            activeShader->ShaderObject != nullptr) {
            activeShader->ShaderObject->TrySetMatrix4("m4_Model", packet.WorldMatrix);
        }
        packet.MeshObject->Bind();
        packet.MeshObject->Draw();
    }
}

void BatchRenderer::RenderInstancedGroups(u16& lastShaderID, u16& lastMaterialID) {
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
            UploadMaterialData(material.get());
            lastShaderID = shaderID;
            lastMaterialID = materialID;
        }
        if (std::shared_ptr<ShaderAsset> activeShader = material->GetActiveShader();
            activeShader != nullptr &&
            activeShader->IsReady == true &&
            activeShader->ShaderObject != nullptr) {
            activeShader->ShaderObject->TrySetBool("b_IsInstanced", true);
        }
        if (auto* multiMesh = dynamic_cast<MultiMeshInstance3D*>(instancedVisual3D)) {
            auto const& transforms = multiMesh->GetTransforms();
            if (transforms.IsEmpty() == false && InstancedVisual3D::s_InstancedVisualSSBO != 0) {
                DynamicList<GPU::InstanceRenderData> gpuInstances{};
                gpuInstances.Reserve(transforms.GetSize());
                for (size_t a = 0; a < transforms.GetSize(); ++a) {
                    GPU::InstanceRenderData instance{};
                    instance.WorldMatrix = transforms[a];
                    instance.NormalMatrix = Math::Matrix4x4::Transpose(Math::Matrix4x4::Inverse(transforms[a]));
                    instance.ObjectID = static_cast<u32>(a);
                    gpuInstances.PushBack(instance);
                }
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, InstancedVisual3D::s_InstancedVisualSSBO);
                const size_t requiredBytes = gpuInstances.GetSizeInBytes();
                if (InstancedVisual3D::s_InstancedVisualSSBOCapacity < requiredBytes) {
                    glBufferData(GL_SHADER_STORAGE_BUFFER, requiredBytes, gpuInstances.GetData(), GL_DYNAMIC_DRAW);
                    InstancedVisual3D::s_InstancedVisualSSBOCapacity = requiredBytes;
                }
                else {
                    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, requiredBytes, gpuInstances.GetData());
                }
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, InstancedVisual3D::s_InstancedVisualSSBO);
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            }
        }
        instancedVisual3D->Draw();
    }
}

void BatchRenderer::UploadMaterialData(Material const* material) const {
    if (material == nullptr || Material::s_MaterialUBO == 0) {
        return;
    }
    GPU::MaterialRenderData matData{};
    matData.TintColor = material->TintColor;
    matData.MetallicFactor = material->MetallicFactor;
    matData.RoughnessFactor = material->RoughnessFactor;
    matData.AmbientOcclusionFactor = 1.0f;
    matData.PreserveUVCoordinates = material->PreserveUVCoordinates ? 1 : 0;
    matData.UVScale = material->UVScale;
    matData.UVOffset = material->UVOffset;
    glBindBuffer(GL_UNIFORM_BUFFER, Material::s_MaterialUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GPU::MaterialRenderData), &matData);
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

}