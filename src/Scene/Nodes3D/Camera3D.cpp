#include "../../../include/Scene/Nodes3D/Camera3D.h"

namespace Crescent::Scene {

void Camera3D::OnTreeEnter() {
	Node3D::OnTreeEnter();
}
void Camera3D::OnTreeExit() {
	Node3D::OnTreeExit();
}

void Camera3D::SetPerspective(const f32 fovDegrees, const f32 aspectRatio, const f32 nearZ, const f32 farZ) noexcept {
	m_FOV = fovDegrees;
	m_AspectRatio = aspectRatio;
	m_NearZ = nearZ;
	m_FarZ = farZ;
	m_ProjectionMatrix = Math::Matrix4x4::GetPerspectiveProjection(
		Math::DegreesToRadians(m_FOV),
		m_AspectRatio,
		m_NearZ,
		m_FarZ
	);
}

const Math::Matrix4x4& Camera3D::GetViewMatrix() const noexcept {
	Math::Vector3 const& cameraPosition = Transform.GetPosition();
	Math::Vector3 const& forwardVector = Transform.GetForward();
	Math::Vector3 const& upVector = Transform.GetUp();
	m_ViewMatrix = Math::Matrix4x4::GetLookAt(cameraPosition, cameraPosition + forwardVector, upVector);
	return m_ViewMatrix;
}

const Math::Matrix4x4& Camera3D::GetProjectionMatrix() const noexcept {
	return m_ProjectionMatrix;
}

}
