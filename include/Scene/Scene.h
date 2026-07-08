#pragma once
#include "Core/Core.h"
#include "Core/Window.h"
#include "Nodes3D/Camera3D.h"

namespace Crescent::Scene {
struct Camera3D;
struct Scene {
	/// TODO: MOVE THESE OUT OF HERE TO CAMERA OR SOMETHING IDK
	constexpr static f32 cameraSpeed1 = 0.25f;
	constexpr static f32 cameraSpeed2 = 0.5f;
	bool IsAccelerating{false};

	virtual ~Scene() = default;
	virtual void OnUpdate(f32 deltaTime) = 0;
	virtual void OnRender(Window& window) = 0;

	/// TODO: MOVE THESE OUT OF HERE
	void MoveCamera(Math::Vector3 const& direction, f32 deltaTime) const {
		Math::Vector3 position = m_CameraNode->Transform.GetPosition();
		const Math::Vector3 forwardVector = m_CameraNode->Transform.GetForward();
		const Math::Vector3 rightVector = m_CameraNode->Transform.GetRight();
		const Math::Vector3 upVector = m_CameraNode->Transform.GetUp();
		const f32 targetCameraSpeed = IsAccelerating ? cameraSpeed2 : cameraSpeed1;
		position += forwardVector * direction.z * targetCameraSpeed * deltaTime;
		position += rightVector * direction.x * targetCameraSpeed * deltaTime;
		position += upVector * direction.y * targetCameraSpeed * deltaTime;
		m_CameraNode->Transform.SetPosition(position);
	}
	/// TODO: MOVE THESE OUT OF HERE
	void RotateCamera(Math::Vector3 const& eulerDelta) const {
		Math::Vector3 currentEuler = m_CameraNode->Transform.GetRotationEuler();
		currentEuler += eulerDelta;
		currentEuler.z = Math::Clamp(
			currentEuler.z,
			Math::DegreesToRadians(-89.0f),
			Math::DegreesToRadians(89.0f)
		);
		m_CameraNode->Transform.SetRotationEuler(currentEuler);
	}

	Camera3D* m_CameraNode{nullptr};
};

}
