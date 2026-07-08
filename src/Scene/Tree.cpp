#include "../../include/Scene/Tree.h"

#include "Core/Log.h"

namespace Crescent::Scene {
void Tree::OnCreate() {
	m_BatchRenderer = std::make_unique<Render::BatchRenderer>();
	m_Root = std::make_unique<Node>();
	m_Root->m_Tree = this;
	m_Root->OnTreeEnter();
	m_Root->OnCreate();
}

void Tree::OnUpdate(const f32 deltaTime) {
	if (m_Root != nullptr) {
		m_Root->OnUpdate(deltaTime);
	}
	ProcessDeletionQueue();
}

void Tree::OnPhysicsUpdate(const f32 fixedDeltaTime) {
	if (m_Root != nullptr) {
		m_Root->OnPhysicsUpdate(fixedDeltaTime);
	}
}

void Tree::OnPostUpdate() {
	if (m_Root != nullptr) {
		m_Root->OnPostUpdate();
	}
}

Node * Tree::GetRoot() const {
	return m_Root.get();
}

Render::BatchRenderer* Tree::GetBatchRenderer() const noexcept {
	return m_BatchRenderer.get();
}

Camera3D * Tree::GetActiveCamera() const noexcept {
	return m_ActiveCamera;
}

void Tree::SetActiveCamera(Camera3D* camera) noexcept {
	if (camera == nullptr) {
		Log::Warning("({}) Tree `{}` SetActiveCamera(): Provided missing Camera3D.", m_Root->m_ID, m_Root->m_Name);
		return;
	}
	m_ActiveCamera = camera;
}

void Tree::QueueForDeletion(Node *node) {
	if (node == nullptr) {
		return;
	}
	node->m_IsQueuedForDeletion = true;
	if (m_DeletionQueue.Push(node) == false) {
		Log::Warning("({}) DeletionQueue ring buffer full.", m_Root->m_Name);
	}
}

void Tree::ProcessDeletionQueue() {
	if (m_DeletionQueue.IsEmpty() == true) {
		return;
	}
	while (m_DeletionQueue.IsEmpty() == false) {
		Node* node = m_DeletionQueue.Pop();
		if (node != nullptr && node->m_Parent != nullptr) {
			node->m_Parent->RemoveChild(node);
		}
	}
}

}
