#include "Scene/Node.h"
#include "Scene/Tree.h"

namespace Crescent {

u64 Node::s_ID = 0;

Node::Node() : m_ID(++s_ID) {}

Node::~Node() {
	if (m_Tree != nullptr) {
		m_Tree->RemoveFromDeletionQueue(this);
	}
}

void Node::OnCreate() {
	for (size_t a = 0; a < m_Children.GetSize(); ++a) {
		if (m_Children[a] != nullptr) {
			m_Children[a]->OnCreate();
		}
	}
}

void Node::OnUpdate(const f32 deltaTime) {
	for (size_t a = 0; a < m_Children.GetSize(); ++a) {
		if (m_Children[a] != nullptr) {
			m_Children[a]->OnUpdate(deltaTime);
		}
	}
}

void Node::OnPhysicsUpdate(const f32 fixedDeltaTime) {
	for (size_t a = 0; a < m_Children.GetSize(); ++a) {
		if (m_Children[a] != nullptr) {
			m_Children[a]->OnPhysicsUpdate(fixedDeltaTime);
		}
	}
}

void Node::OnPostUpdate() {
	for (size_t a = 0; a < m_Children.GetSize(); ++a) {
		if (m_Children[a] != nullptr) {
			m_Children[a]->OnPostUpdate();
		}
	}
}

void Node::OnTreeEnter() {
	for (size_t a = 0; a < m_Children.GetSize(); ++a) {
		if (m_Children[a] != nullptr) {
			m_Children[a]->m_Tree = m_Tree;
			m_Children[a]->OnTreeEnter();
		}
	}
}

void Node::OnTreeExit() {
	if (m_Tree != nullptr) {
		m_Tree->RemoveFromDeletionQueue(this);
	}
	for (size_t a = 0; a < m_Children.GetSize(); ++a) {
		if (m_Children[a] != nullptr) {
			m_Children[a]->OnTreeExit();
			m_Children[a]->m_Tree = nullptr;
		}
	}
	m_Tree = nullptr;
}

Node* Node::GetParent() const {
	return m_Parent;
}

void Node::AddChildren(DynamicList<std::unique_ptr<Node>>&& children) {
	if (children.IsEmpty() == true) {
		return;
	}
	const size_t originalSize = m_Children.GetSize();
	m_Children.Reserve(originalSize + children.GetSize());
	for (size_t a = 0; a < children.GetSize(); ++a) {
		std::unique_ptr<Node>& child = children[a];
		if (child == nullptr) {
			continue;
		}
		child->m_Parent = this;
		if (m_Tree != nullptr) {
			child->m_Tree = m_Tree;
			child->OnTreeEnter();
			child->OnCreate();
		}
		m_Children.PushBack(std::move(child));
	}
}

void Node::ReserveChildren(const size_t capacity) {
	m_Children.Reserve(capacity);
}

Node * Node::GetChild(const std::string_view name) const {
	for (size_t a = 0; a < m_Children.GetSize(); ++a) {
		if (m_Children[a] != nullptr && m_Children[a]->m_Name == name) {
			return m_Children[a].get();
		}
	}
	return nullptr;
}

std::unique_ptr<Node> Node::DetachChild(Node *child) {
	for (size_t a = 0; a < m_Children.GetSize(); ++a) {
		if (m_Children[a].get() == child) {
			if (m_Tree != nullptr) {
				child->OnTreeExit();
			}
			child->m_Parent = nullptr;
			std::unique_ptr<Node> detachedNode = std::move(m_Children[a]);
			m_Children.RemoveAt(a);
			return detachedNode;
		}
	}
	return nullptr;
}

void Node::RemoveChild(Node *child) {
	std::unique_ptr<Node> discarded = DetachChild(child);
}

void Node::RemoveChildren(const DynamicList<Node*>& children) {
	if (children.IsEmpty() == true || m_Children.IsEmpty() == true) {
		return;
	}
	for (size_t a = 0; a < children.GetSize(); ++a) {
		Node* target = children[a];
		for (size_t b = 0; b < m_Children.GetSize(); ++b) {
			if (m_Children[b].get() == target) {
				if (m_Tree != nullptr) {
					m_Children[b]->OnTreeExit();
				}
				m_Children[b] = nullptr;
				break;
			}
		}
	}
	size_t writeIndex{0};
	for (size_t readIndex = 0; readIndex < m_Children.GetSize(); ++readIndex) {
		if (m_Children[readIndex] != nullptr) {
			if (writeIndex != readIndex) {
				m_Children[writeIndex] = std::move(m_Children[readIndex]);
			}
			writeIndex++;
		}
	}
	while (m_Children.GetSize() > writeIndex) {
		m_Children.PopBack();
	}
}

void Node::ClearChildren() {
	if (m_Children.IsEmpty() == true) {
		return;
	}
	if (m_Tree != nullptr) {
		for (size_t a = 0; a < m_Children.GetSize(); ++a) {
			if (m_Children[a] != nullptr) {
				m_Children[a]->OnTreeExit();
			}
		}
	}
	m_Children.Clear();
}

void Node::QueueFree() {
	if (m_IsQueuedForDeletion == true) {
		return;
	}
	if (m_Tree != nullptr) {
		m_Tree->QueueForDeletion(this);
	}
	else if (m_Parent != nullptr) {
		m_Parent->RemoveChild(this);
	}
}

}
