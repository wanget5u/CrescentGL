#include "../../include/Scene/Transform.h"
#include "../../include/Scene/Nodes3D/Node3D.h"

namespace Crescent::Scene {

void Transform::SetParent(const Transform* parent) noexcept {
	if (m_Parent != parent) {
		m_Parent = parent;
		MarkDirty();
	}
}

const Transform* Transform::GetParent() const noexcept {
	return m_Parent;
}

u32 Transform::GetVersion() const noexcept {
	return m_Version;
}

const Math::Vector3& Transform::GetPosition() const noexcept {
	return m_Position;
}

void Transform::SetPosition(Math::Vector3 const& newPosition) {
	m_Position = newPosition;
	MarkDirty();
}

Math::Vector3 Transform::GetForward() const noexcept {
	return m_Rotation * Math::Vector3::Forward();
}

Math::Vector3 Transform::GetUp() const noexcept {
	return m_Rotation * Math::Vector3::Up();
}

Math::Vector3 Transform::GetRight() const noexcept {
	return m_Rotation * Math::Vector3::Right();
}

const Math::Vector3& Transform::GetScale() const noexcept {
	return m_Scale;
}

void Transform::SetScale(const f32 newScale) {
	m_Scale = Math::Vector3(newScale, newScale, newScale);
	MarkDirty();
}

void Transform::SetScale(Math::Vector3 const &newScale) {
	m_Scale = newScale;
	MarkDirty();
}

const Math::Quaternion& Transform::GetRotation() const noexcept {
	return m_Rotation;
}

void Transform::SetRotation(Math::Quaternion const &newRotation) {
	m_Rotation = newRotation;
	m_EulerDirty = true;
	MarkDirty();
}

const Math::Vector3& Transform::GetRotationEuler() const noexcept {
	if (m_EulerDirty == true) {
		m_RotationEuler = Math::Quaternion::ToEulerAngles(m_Rotation);
		m_EulerDirty = false;
	}
	return m_RotationEuler;
}

void Transform::SetRotationEuler(Math::Vector3 const &newRotation) {
	m_RotationEuler = newRotation;
	m_Rotation = Math::Quaternion::FromEulerAngles(newRotation);
	m_EulerDirty = false;
	MarkDirty();
}

Math::Matrix4x4& Transform::GetLocalMatrix() const noexcept {
	if (m_LocalMatrixDirty == true) {
		Math::Matrix4x4 translation = Math::Matrix4x4::GetTranslation(m_Position);
		Math::Matrix4x4 rotation    = Math::Quaternion::ToMatrix4x4(m_Rotation);
		Math::Matrix4x4 scale       = Math::Matrix4x4::GetScale(m_Scale);

		m_LocalMatrix = translation * rotation * scale;
		m_LocalMatrixDirty = false;
	}
	return m_LocalMatrix;
}

Math::Matrix4x4& Transform::GetWorldMatrix() const noexcept {
	if (m_Parent != nullptr) {
		u32 currentParentVersion = m_Parent->GetVersion();
		if (m_CachedParentVersion != currentParentVersion) {
			m_WorldMatrixDirty = true;
			m_CachedParentVersion = currentParentVersion;
		}
	}
	if (m_WorldMatrixDirty == true || m_LocalMatrixDirty == true) {
		if (m_Parent != nullptr) {
			m_WorldMatrix = m_Parent->GetWorldMatrix() * GetLocalMatrix();
		} else {
			m_WorldMatrix = GetLocalMatrix();
		}
		m_WorldMatrixDirty = false;
	}
	return m_WorldMatrix;
}

void Transform::MarkDirty() noexcept {
	++m_Version;
	m_LocalMatrixDirty = true;
	m_WorldMatrixDirty = true;
}
}
