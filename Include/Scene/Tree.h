#pragma once
#include <memory>

#include "Core/Core.h"
#include "Collection/DynamicQueue.h"
#include "Scene/Node.h"

namespace Crescent::Render {
struct BatchRenderer;
}
namespace Crescent::Scene {
struct Camera3D;
///
struct Tree {
	Tree();
	~Tree();
	///
	void OnUpdate(f32 deltaTime);
	///
	void OnPhysicsUpdate(f32 fixedDeltaTime);
	///
	void OnPostUpdate();
	///
	[[nodiscard]] Node* GetRoot() const;
	///
	template <typename T, typename... Args>
	T* AddChild(std::string_view name, Args&&... args);
	///
	[[nodiscard]] Render::BatchRenderer* GetBatchRenderer() const noexcept;
	///
	void QueueForDeletion(Node* node);
	///
	void ProcessDeletionQueue();
private:
	///
	std::unique_ptr<Node> m_Root{nullptr};
	///
	std::unique_ptr<Render::BatchRenderer> m_BatchRenderer{nullptr};
	///
	DynamicQueue<Node*, 2048> m_DeletionQueue{};
};
template<typename T, typename ... Args>
T * Tree::AddChild(std::string_view name, Args &&...args) {
	return m_Root->AddChild<T>(name, std::forward<Args>(args)...);
}
}
