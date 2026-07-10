#pragma once
#include <atomic>
#include <memory>
#include <mutex>
#include "Core/Core.h"
#include "Math/Vector/Vector3.h"

namespace Crescent {
struct Window;
}

namespace Crescent::Scene {
struct Node;
struct Tree;
struct Camera3D;
struct Scene {
	std::atomic<bool> IsAccelerating{false};
	Scene();
	virtual ~Scene();
	virtual void OnUpdate(f32 deltaTime) = 0;
	virtual void OnRender(Window& window) = 0;
	///
	[[nodiscard]] Node* GetRoot() const;
	///
	void MoveCamera(Math::Vector3 const& direction, f32 deltaTime) const;
	void RotateCamera(Math::Vector3 const& eulerDelta) const;
	///
	void QueueCameraRotation(Math::Vector3 const& eulerDelta);
	void SetMoveInputForward(bool active);
	void SetMoveInputBackward(bool active);
	void SetMoveInputRightward(bool active);
	void SetMoveInputLeftward(bool active);
	void UpdateCamera(f32 deltaTime);
protected:
	///
	std::unique_ptr<Tree> m_Tree{nullptr};
	///
	constexpr static f32 cameraSpeed1 = 40.0f;
	constexpr static f32 cameraSpeed2 = 80.0f;
	std::unique_ptr<Camera3D> m_PreviewCamera{nullptr};
	///
	mutable std::mutex m_CameraMutex;
	Math::Vector3 m_PendingEulerDelta = Math::Vector3::Zero();
	bool m_MoveForward{false};
	bool m_MoveBackward{false};
	bool m_MoveRightward{false};
	bool m_MoveLeftward{false};
};
}
