#pragma once
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "Collection/DynamicList.h"

namespace Crescent::Scene {
	struct MeshInstance3D;
	struct MultiMeshInstance3D;
	struct PointLight3D;
	struct Camera3D;
}

namespace Crescent::Render {
struct IRenderGroup {
	virtual ~IRenderGroup() = default;
	virtual void FlushLoad() = 0;
	virtual void FlushUnload() = 0;
	virtual void Clear() = 0;
};

template <typename T>
struct RenderGroup : IRenderGroup {
	DynamicList<T*> Registered{};
	DynamicList<T*> Staged{};
	void Register(T* instance, bool isBatchLoading);
	void Unregister(T* instance, bool isBatchUnloading);
	void FlushLoad() override;
	void FlushUnload() override;
	void Clear() override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
struct BatchRenderer {
	BatchRenderer() = default;
	~BatchRenderer() = default;
	void InitializeBuffers();
	void PrepareFrame(Scene::Camera3D const* camera);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Batch Loading / Unloading
	///
	void BeginBatchLoad();
	void EndBatchLoad();
	void BeginBatchUnload();
	void EndBatchUnload();
	void Clear();
	///
	template <typename T>
	RenderGroup<T>* GetRenderGroup();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Registration API (Called by Node3D inside OnTreeEnter() / OnTreeExit())
	template <typename T>
	void Register(T* instance);
	template <typename T>
	void Unregister(T* instance);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Render Execution
	///
	void RenderScene(Scene::Camera3D const* camera);
private:
	bool m_IsBatchLoading{false};
	bool m_IsBatchUnloading{false};
	std::unordered_map<std::type_index, std::unique_ptr<IRenderGroup>> m_RenderGroups{};
	///
	u32 m_SceneDataUBO{0};
	u32 m_DirectionalLightSSBO{0};
	u32 m_InstanceDataSSBO{0};
	u32 m_materialDataUBO{0};
};
template<typename T>
void BatchRenderer::Register(T* instance) {
	GetRenderGroup<T>()->Register(instance, m_IsBatchLoading);
}

template<typename T>
void BatchRenderer::Unregister(T* instance) {
	GetRenderGroup<T>()->Unregister(instance, m_IsBatchLoading);
}
}
