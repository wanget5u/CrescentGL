#include "Scene/Tree.h"
#include "Render/BatchRenderer.h"

#include "Core/Log.h"

namespace Crescent {
Tree::Tree() {
	m_BatchRenderer = std::make_unique<BatchRenderer>();
	m_BatchRenderer->InitializeBuffers();
	m_Root = std::make_unique<Node>();
	m_Root->m_Tree = this;
	m_Root->OnTreeEnter();
	m_Root->OnCreate();
}

Tree::~Tree() = default;

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

BatchRenderer* Tree::GetBatchRenderer() const noexcept {
	return m_BatchRenderer.get();
}

void Tree::QueueForDeletion(Node *node) {
	node->m_IsQueuedForDeletion = true;
	m_DeletionQueue.PushBack(node);
}

void Tree::RemoveFromDeletionQueue(Node* node) {
	if (node == nullptr || m_DeletionQueue.IsEmpty() == true) {
		return;
	}
	for (size_t a = 0; a < m_DeletionQueue.GetSize(); ++a) {
		if (m_DeletionQueue[a] == node) {
			m_DeletionQueue[a] = nullptr;
		}
	}
}

void Tree::ProcessDeletionQueue() {
	if (m_DeletionQueue.IsEmpty() == true) {
		return;
	}
	for (size_t a = 0; a < m_DeletionQueue.GetSize(); ++a) {
		Node* node = m_DeletionQueue[a];
		m_DeletionQueue[a] = nullptr;
		if (node != nullptr && node->m_Parent != nullptr) {
			node->m_Parent->RemoveChild(node);
		}
	}
	m_DeletionQueue.Clear();
}

}
