#include "../../../../include/Scene/Nodes3D/Geometry/InstancedVisual3D.h"

#include "Render/BatchRenderer.h"
#include "Scene/Tree.h"

namespace Crescent::Scene {

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
