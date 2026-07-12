#pragma once
#include "Math/Matrix/Matrix4x4.h"
#include "Scene/Nodes3D/Node3D.h"

namespace Crescent {
/// Node3D that defines a viewing frustum and computes the necessary view and projection matrices for rendering
struct Camera3D : Node3D {
	Camera3D() = default;
	~Camera3D() override = default;
	///
	void OnTreeEnter() override;
	void OnTreeExit() override;
	/// Configures the camera's projection properties
	void SetPerspective(f32 fovDegrees, f32 aspectRatio, f32 nearZ = 0.1f, f32 farZ = 1000.0f) noexcept;
	[[nodiscard]] const Math::Matrix4x4& GetViewMatrix() const noexcept;
	[[nodiscard]] const Math::Matrix4x4& GetProjectionMatrix() const noexcept;
private:
	///
	f32 m_FOV{70.f};
	f32 m_AspectRatio{1.777777777777778f};
	f32 m_NearZ{0.1f};
	f32 m_FarZ{1000.0f};
	///
	mutable Math::Matrix4x4 m_ViewMatrix = Math::Matrix4x4::GetIdentity();
	mutable u32 m_CachedTransformVersion{0};
	mutable bool m_ViewMatrixDirty{true};
	///
	Math::Matrix4x4 m_ProjectionMatrix = Math::Matrix4x4::GetIdentity();
};
}
