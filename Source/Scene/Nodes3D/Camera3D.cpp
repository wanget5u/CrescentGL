#include "Scene/Nodes3D/Camera3D.h"

namespace Crescent {

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
	u32 const currentTransformVersion = Transform.GetVersion();
	if (m_CachedTransformVersion != currentTransformVersion) {
		m_ViewMatrixDirty = true;
		m_CachedTransformVersion = currentTransformVersion;
	}
	if (m_ViewMatrixDirty == true) {
		Math::Vector3 const& cameraPosition = Transform.GetPosition();
		Math::Vector3 const& forwardVector = Transform.GetForward();
		Math::Vector3 const& upVector = Transform.GetUp();
		m_ViewMatrix = Math::Matrix4x4::GetLookAt(cameraPosition, cameraPosition + forwardVector, upVector);
		m_ViewMatrixDirty = false;
	}
	return m_ViewMatrix;
}

const Math::Matrix4x4& Camera3D::GetProjectionMatrix() const noexcept {
	return m_ProjectionMatrix;
}

}
