#include "Scene/Nodes3D/Light/PointLight3D.h"

#include "Scene/Tree.h"

namespace Crescent {

void PointLight3D::OnTreeEnter() {
	Light3D::OnTreeEnter();
}

void PointLight3D::OnTreeExit() {
	Light3D::OnTreeExit();
}

LightType PointLight3D::GetLightType() const noexcept {
	return LightType::Point;
}

f32 PointLight3D::GetRange() const noexcept {
	return m_Range;
}

void PointLight3D::SetRange(const f32 range) noexcept {
	m_Range = Math::Max(0.f, range);
}

f32 PointLight3D::GetConstant() const noexcept {
	return m_Constant;
}

void PointLight3D::SetConstant(const f32 constant) noexcept {
	m_Constant = Math::Max(0.0001f, constant);
}

f32 PointLight3D::GetLinear() const noexcept {
	return m_Linear;
}

void PointLight3D::SetLinear(const f32 linear) noexcept {
	m_Linear = Math::Clamp(linear, 0.0f, 1.0f);
}

f32 PointLight3D::GetQuadratic() const noexcept {
	return m_Quadratic;
}

void PointLight3D::SetQuadratic(const f32 quadratic) noexcept {
	m_Quadratic = Math::Clamp(quadratic, 0.0f, 2.0f);
}

}
