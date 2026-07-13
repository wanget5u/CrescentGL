#include "Node/Node3D/Light/Light3D.h"

#include "Node/Tree.h"
#include "Render/BatchRenderer.h"

namespace Crescent {

void Light3D::OnTreeEnter() {
	VisualInstance3D::OnTreeEnter();
	m_Tree->GetBatchRenderer()->Register(this);
}

void Light3D::OnTreeExit() {
	VisualInstance3D::OnTreeExit();
	m_Tree->GetBatchRenderer()->Unregister(this);
}

void Light3D::SetIsOn(const bool isOn) noexcept {
	m_IsOn = isOn;
}

bool Light3D::IsOn() const noexcept {
	return m_IsOn;
}

void Light3D::SetCastShadow(const bool castShadow) noexcept {
	m_CastShadow = castShadow;
}

bool Light3D::IsCastingShadow() const noexcept {
	return m_CastShadow;
}

void Light3D::SetEnergy(const f32 energy) noexcept {
	m_Energy = Math::Max(0.0f, energy);
}

f32 Light3D::GetEnergy() const noexcept {
	return m_Energy;
}

void Light3D::SetColor(Math::Vector3 const& color) noexcept {
	m_Color = color;
}

Math::Vector3 const& Light3D::GetColor() const noexcept {
	return m_Color;
}

}
