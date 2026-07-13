#include "Node/Node3D/Geometry/InstancedVisual3D.h"

#include "Node/Tree.h"
#include "Render/BatchRenderer.h"

namespace Crescent {

void InstancedVisual3D::OnTreeEnter() {
	GeometryInstance3D::OnTreeEnter();
	if (m_Tree != nullptr) {
		m_Tree->GetBatchRenderer()->Register(this);
	}
}

void InstancedVisual3D::OnTreeExit() {
	GeometryInstance3D::OnTreeExit();
	if (m_Tree != nullptr) {
		m_Tree->GetBatchRenderer()->Unregister(this);
	}
}

}
