#pragma once
#include "Node/Node.h"
#include "Node/Transform.h"

namespace Crescent {
/// Base class for all 3D objects in the scene graph
/// Node3D extends the base Node class by introducing a 3D Transform component.
/// Any object such as meshes, lights, cameras, or audio emitters should
/// derive from Node3D to participate in space transformations
struct Node3D : Node {
	friend struct BatchRenderer;
	/// The local spatial data (position, rotation, scale) of this node
	Transform Transform{};
	Node3D() = default;
	~Node3D() override = default;
	/// Overrides the base infrastructure hook when attaching to an active Scene Tree.
	/// Coordinates initialization and signals children that a spatial hierarchy is
	/// now active under the current context
	void OnTreeEnter() override;
	/// Overrides the base infrastructure hook when detaching from the Scene Tree.
	/// Cleans up spatial bindings and marks the sub-hierarchy as structurally inactive
	void OnTreeExit() override;
	void SetVisible(bool visible) noexcept;
	[[nodiscard]] bool IsVisible() const noexcept;
protected:
	bool m_IsVisible{true};
};
}
