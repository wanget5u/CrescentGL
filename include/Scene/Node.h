#pragma once
#include <string>
#include <string_view>

#include "Collection/DynamicList.h"
#include "Core/Core.h"

namespace Crescent::Scene {
struct Tree;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Base class for all objects in the scene hierarchy
///
/// The Node class represents a building block of the scene graph. Nodes manage their own lifetimes,
/// handle hierarchical transformations, propagate engine lifecycle updates down to their children
struct Node {
	Node();
	virtual ~Node() = default;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Lifecycle Methods
	///
	/// Called when the node is initialized or enters a live Scene Tree
	void OnCreate();
	/// Processed every frame
	void OnUpdate(f32 deltaTime);
	/// Processed at a fixed time step. Used exclusively for physics computations
	void OnPhysicsUpdate(f32 fixedDeltaTime);
	/// Processed at the end of a frame loop
	void OnPostUpdate();
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Tree Infrastructure
	///
	/// Triggered when this node or its ancestor is attached to an active Scene Tree.
	/// Propagates the call of this method to all children
	virtual void OnTreeEnter();
	/// Triggered right before this node leaves an active Scene Tree.
	/// Propagates the call of this method to all children
	virtual void OnTreeExit();
	/// Retrieves the immediate parent of the node
	/// If nullptr, this node is a root node
	Node* GetParent() const;
	///
	template <typename T>
	T* GetFirstAncestorOfType() const;
	/// Instantiates and appends a new child node to this node
	template <typename T, typename... Args>
	T* AddChild(std::string_view name, Args&&... args);
	/// Batches and appends multiple managed nodes to this parent.
	/// Transfers exclusive ownership of the children list to this node
	void AddChildren(DynamicList<std::unique_ptr<Node>>&& children);
	/// Reserves internal memory capacity for the child storage list to prevent reallocations
	void ReserveChildren(size_t capacity);
	/// Searches for a direct child node matching the specified name
	Node* GetChild(std::string_view name) const;
	/// Severs the connection between this node and a child without deleting the child.
	/// Hands memory control back to the caller.
	std::unique_ptr<Node> DetachChild(Node* child);
	/// Direct removal of a single child node.
	/// Completely destroys the child
	void RemoveChild(Node* child);
	/// Deletes a list of target child nodes efficiently, using batch tree operations.
	/// Nodes are safely unparented, cleaned, and wiped from memory allocations
	void RemoveChildren(const DynamicList<Node*>& children);
	/// Destroys all child nodes managed by this parent node
	void ClearChildren();
	/// Safely schedules this node and its entire sub-hierarchy for deletion at the end of the frame.
	/// This avoids crashing when destroying elements inside execution updates
	void QueueFree();
protected:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Node Members
	///
	/// Global auto-incremented counter used to mint distinct node IDs
	static u64 s_ID;
	/// Unique runtime numerical footprint for tracking this node instance
	u64 m_ID{0};
	/// Readable identifier name assigned to this node
	std::string m_Name{};
	/// Weak pointer reference pointing back to our parent node
	Node* m_Parent{nullptr};
	/// Memory managed internal array storing the unique pointers of child nodes
	DynamicList<std::unique_ptr<Node>, 8> m_Children{};
	/// Deferment flag indicating if this node is flagged to be purged by QueueFree
	bool m_IsQueuedForDeletion{false};
	/// Tree handshake to use the m_Tree
	friend struct Tree;
	/// Internal binding to the overarching execution Tree context.
	Tree* m_Tree{nullptr};
};

template<typename T>
T * Node::GetFirstAncestorOfType() const {
	Node* current = GetParent();
	// parent can be nullptr, meaning this is a root node
	while (current != nullptr) {
		if (T* castedNode = dynamic_cast<T*>(current)) {
			return castedNode;
		}
		current = current->GetParent();
	}
	return nullptr;
}

template<typename T, typename ... Args>
T* Node::AddChild(std::string_view name, Args &&...args) {
	std::unique_ptr<T> child = std::make_unique<T>(std::forward<Args>(args)...);
	T* childPointer = child.get();
	child->m_Name = name;
	child->m_Parent = this;
	child->m_Tree = this->m_Tree;
	child->OnTreeEnter();
	child->OnCreate();
	m_Children.PushBack(std::move(child));
	return childPointer;
}
}
