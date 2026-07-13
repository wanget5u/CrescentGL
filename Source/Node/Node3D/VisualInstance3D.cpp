#include "Node/Node3D/VisualInstance3D.h"

#include "Node/Tree.h"

namespace Crescent {

void VisualInstance3D::OnTreeEnter() {
	Node3D::OnTreeEnter();
}

void VisualInstance3D::OnTreeExit() {
	Node3D::OnTreeExit();
}

void VisualInstance3D::SetLayerMask(const u8 index) noexcept {
	assert(index < 8 && "Layer index out of bounds for u8 bitmask!");
	m_LayerMask = (1 << index);
}

u8 VisualInstance3D::GetLayerMask() const noexcept {
	return m_LayerMask;
}

void VisualInstance3D::EnableLayer(const u8 maskIndex) {
	m_LayerMask |= (1 << maskIndex);
}

void VisualInstance3D::DisableLayer(const u8 maskIndex) {
	m_LayerMask &= ~(1 << maskIndex);
}

bool VisualInstance3D::IsOnLayer(const u8 maskIndex) const {
	return m_LayerMask & (1 << maskIndex);
}

}
