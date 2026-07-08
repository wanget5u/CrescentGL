#include "../../../include/Scene/Nodes3D/Node3D.h"

namespace Crescent::Scene {

void Node3D::OnTreeEnter() {
	Node::OnTreeEnter();
	Node* node = GetParent();
	while (node != nullptr) {
		if (auto* parent3D = dynamic_cast<Node3D*>(node)) {
			Transform.SetParent(&parent3D->Transform);
			return;
		}
		node = node->GetParent();
	}
	Transform.SetParent(nullptr);
}

void Node3D::OnTreeExit() {
	Node::OnTreeExit();
	Transform.SetParent(nullptr);
}

}
