#pragma once
#include "Math/Matrix/Matrix4x4.h"
#include "Math/Quaternion/Quaternion.h"
#include "Math/Vector/Vector3.h"

namespace Crescent::Scene {
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Manages 3D spatial properties (Position, Rotation, Scale) and matrix hierarchies.
///
/// The Transform component calculates local and world coordinate systems. Uses dirty flag
/// architecture, meaning matrix updates and Euler conversions are deferred until they
/// are explicitly requested (via getters). It also tracks hierarchy versions to accurately
/// detect parent spatial mutations to avoid unnecessary computations
struct Transform {
	Transform() = default;
	~Transform() = default;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Structural Hierarchy
	///
	/// Assigns a parent transformation to this transform.
	/// Set the parent to nullptr to isolate it to local space
	void SetParent(const Transform* parent) noexcept;
	/// Retrieves the assigned parent transform.
	[[nodiscard]] const Transform* GetParent() const noexcept;
	/// Gets the structural version of this transform.
	/// This value increments every time position, rotation, or scale changes.
	/// Allows child transforms to verify if their cached world space matrices
	/// needs to be updated
	[[nodiscard]] u32 GetVersion() const noexcept;
	/// Retrieves the local position vector
	[[nodiscard]] const Math::Vector3& GetPosition() const noexcept;
	/// Updates the local position vector and invalidates the matrices
	void SetPosition(Math::Vector3 const& newPosition);
	/// Extracts the forward directional vector (-Z) relative to world orientation.
	[[nodiscard]] Math::Vector3 GetForward() const noexcept;
	/// Extracts the upward directional vector (+Y) relative to world orientation.
	[[nodiscard]] Math::Vector3 GetUp() const noexcept;
	/// Extracts the rightward directional vector (+X) relative to world orientation.
	[[nodiscard]] Math::Vector3 GetRight() const noexcept;
	/// Retrieves the local 3D scale vector
	[[nodiscard]] const Math::Vector3& GetScale() const noexcept;
	/// Updates the local scale uniformly across all axes
	void SetScale(f32 newScale);
	/// Updates the local scale non-uniformly across specific axes
	void SetScale(Math::Vector3 const& newScale);
	/// Retrieves the local orientation represented as a Quaternion.
	[[nodiscard]] const Math::Quaternion& GetRotation() const noexcept;
	/// Updates the local orientation via a Quaternion
	void SetRotation(Math::Quaternion const& newRotation);
	/// Retrieves the local orientation converted into Euler angles
	[[nodiscard]] const Math::Vector3& GetRotationEuler() const noexcept;
	/// Updates the local orientation via Euler Angles.
	/// They get automatically converted to Quaternion internally
	void SetRotationEuler(Math::Vector3 const& newRotation);
	/// Computes and returns the local transformations combined into a single matrix
	[[nodiscard]] Math::Matrix4x4& GetLocalMatrix() const noexcept;
	/// Computes and returns the absolute global transformations in world coordinates
	[[nodiscard]] Math::Matrix4x4& GetWorldMatrix() const noexcept;
private:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Transform Members
	///
	const Transform* m_Parent{nullptr};
	Math::Vector3 m_Position = Math::Vector3::Zero();
	Math::Quaternion m_Rotation	= Math::Quaternion::Identity();
	Math::Vector3 m_Scale = Math::Vector3::One();
	/// Cached Euler angle container for tracking human-readable rotation representation
	mutable Math::Vector3 m_RotationEuler = Math::Vector3::Zero();
	mutable bool m_EulerDirty{false};
	/// Cached local transformation matrix combining position, rotation, and scale
	mutable Math::Matrix4x4  m_LocalMatrix = Math::Matrix4x4::GetIdentity();
	mutable bool m_LocalMatrixDirty{false};
	/// Cached world transformation matrix combining local properties with parental structures
	mutable Math::Matrix4x4  m_WorldMatrix = Math::Matrix4x4::GetIdentity();
	mutable bool m_WorldMatrixDirty{true};
	/// Tracker incremented whenever spatial values are updated. Used by children to detect hierarchy modifications
	u32 m_Version{1};
	/// Version snapshot of the parent node recorded during the last world space validation loop
	mutable u32 m_CachedParentVersion{0};
	/// Propagates data invalidation flags throughout this instance when any transformation changes
	void MarkDirty() noexcept;
};
}
