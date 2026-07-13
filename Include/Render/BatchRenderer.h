#pragma once
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "RenderGroup.h"
#include "RenderPacket.h"
#include "Collection/DynamicList.h"

namespace Crescent {
	struct MeshInstance3D;
	struct MultiMeshInstance3D;
	struct PointLight3D;
	struct Camera3D;
}

namespace Crescent::GPU {
	struct PointLightRenderData;
	struct DirectionalLightRenderData;
	struct SceneRenderData;
}

namespace Crescent {
struct BatchRenderer {
	explicit BatchRenderer();
	~BatchRenderer();
	void InitializeBuffers();
	void Clear();
	///
	void BeginBatchLoad();
	void EndBatchLoad();
	void BeginBatchUnload();
	void EndBatchUnload();
	///
	template <typename T>
	RenderGroup<T>* GetRenderGroup();
	///
	template <typename T>
	void Register(T* instance);
	///
	template <typename T>
	void Unregister(T* instance);
	/// Issues the full frame rendering pipeline for the given camera
	void RenderScene(Camera3D const* camera);
private:
	bool m_IsBatchLoading{false};
	bool m_IsBatchUnloading{false};
	std::unordered_map<std::type_index, std::unique_ptr<IRenderGroup>> m_RenderGroups{};
	///
	DynamicList<RenderPacket> m_OpaquePackets{};
	DynamicList<RenderPacket> m_TransparentPackets{};
	///
	u32 s_SceneUBO{0};
	///
	static constexpr u32 FrameQueryCount = 10;
	bool m_QueryIssued[FrameQueryCount]{false};
	u32 m_GPUTimerQueryIDs[FrameQueryCount]{};
	u32 m_CurrentQueryIndex{0};
	///
	void PrepareFrame(Camera3D const* camera);
	void ResolveLight3DRenderGroup(DynamicList<GPU::PointLightRenderData>& pointLights, DynamicList<GPU::DirectionalLightRenderData>& directionalLights);
	void ResolveMeshInstance3DRenderGroup(Math::Vector3 const& cameraPosition);
	///
	void SortPackets();
	void RenderPackets(DynamicList<RenderPacket> const& packets, u16& lastShaderID, u16& lastMaterialID) const;
	void RenderInstancedGroups(u16& lastShaderID, u16& lastMaterialID);
	void UploadMaterialData(Material const* material) const;
	///
	void StartGPUQuery();
	void EndGPUQuery();
};
template<typename T>
RenderGroup<T>* BatchRenderer::GetRenderGroup() {
	std::type_index typeIndex = typeid(T);
	auto it = m_RenderGroups.find(typeIndex);
	if (it == m_RenderGroups.end()) {
		std::unique_ptr<RenderGroup<T>> renderGroup = std::make_unique<RenderGroup<T>>();
		RenderGroup<T>* renderGroupPtr = renderGroup.get();
		m_RenderGroups[typeIndex] = std::move(renderGroup);
		return renderGroupPtr;
	}
	return static_cast<RenderGroup<T>*>(it->second.get());
}
template<typename T>
void BatchRenderer::Register(T* instance) {
	GetRenderGroup<T>()->Register(instance, m_IsBatchLoading);
}
template<typename T>
void BatchRenderer::Unregister(T* instance) {
	GetRenderGroup<T>()->Unregister(instance, m_IsBatchLoading);
}
}
