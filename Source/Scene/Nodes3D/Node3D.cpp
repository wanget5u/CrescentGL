#include "../../../Include/Scene/Nodes3D/Node3D.h"

namespace Crescent::Scene {

void Node3D::OnTreeEnter() {
	Node::OnTreeEnter();
	Node3D* parent3D = GetFirstAncestorOfType<Node3D>();
	if (parent3D != nullptr) {
		Transform.SetParent(&parent3D->Transform);
		return;
	}
	// if parent3D is nullptr, then this is a root node
	Transform.SetParent(nullptr);
}

void Node3D::OnTreeExit() {
	Node::OnTreeExit();
	Transform.SetParent(nullptr);
}

void Node3D::SetVisible(const bool visible) noexcept {
	m_IsVisible = visible;
}

bool Node3D::IsVisible() const noexcept {
	return m_IsVisible;
}

}
