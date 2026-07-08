#pragma once
#include <memory>

#include "Core/Core.h"
#include "Collection/DynamicQueue.h"
#include "Render/BatchRenderer.h"
#include "Scene/Node.h"

namespace Crescent::Scene {
struct Camera3D;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
struct Tree {
	static Tree& Instance() {
		static Tree s_Instance;
		return s_Instance;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Update Cycle
	///
	///
	void OnCreate();
	///
	void OnUpdate(f32 deltaTime);
	///
	void OnPhysicsUpdate(f32 fixedDeltaTime);
	///
	void OnPostUpdate();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Tree Infrastructure
	///
	///
	[[nodiscard]] Node* GetRoot() const;
	///
	[[nodiscard]] Render::BatchRenderer* GetBatchRenderer() const noexcept;
	///
	[[nodiscard]] Camera3D* GetActiveCamera() const noexcept;
	///
	void SetActiveCamera(Camera3D* camera) noexcept;
	///
	void QueueForDeletion(Node* node);
	///
	void ProcessDeletionQueue();
private:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///
	std::unique_ptr<Node> m_Root						  {nullptr};
	///
	Camera3D* m_ActiveCamera							  {nullptr};
	///
	std::unique_ptr<Render::BatchRenderer> m_BatchRenderer{nullptr};
	///
	DynamicQueue<Node*, 2048> m_DeletionQueue			  {};
};
}
