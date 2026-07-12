#pragma once
#include <atomic>
#include <memory>

#include "Core/Core.h"
#include "Collection/DynamicList.h"
#include "Input/InputAction.h"
#include "Math/Vector/Vector3.h"

namespace Crescent {
	struct Material;
	struct Window;
	struct Node;
	struct Tree;
	struct Camera3D;
}

namespace Crescent {
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
	void SetSceneMaterial(std::shared_ptr<Material> material);
protected:
	constexpr static f32 CameraSpeed1 = 10.0f;
	constexpr static f32 CameraSpeed2 = 40.0f;
	constexpr static f32 CameraLookSensitivity = 0.002f;
	///
	std::unique_ptr<Tree> m_Tree{nullptr};
	///
	std::unique_ptr<Camera3D> m_PreviewCamera{nullptr};
	Math::Vector3 m_PendingEulerDelta = Math::Vector3::Zero();
	bool m_MoveForward{false};
	bool m_MoveBackward{false};
	bool m_MoveRightward{false};
	bool m_MoveLeftward{false};
	void SetupInputActions();
	void SetupDefaultMaterials();
	struct ActionSubscription {
		Input::Action* ActionPtr{nullptr};
		u32 SubscriptionID{0};
	};
	DynamicList<ActionSubscription> m_InputSubscriptions{};
	std::shared_ptr<Material> m_Material{};
	std::shared_ptr<Material> m_WireframeMaterial{};
	std::shared_ptr<Material> m_UnlitMaterial{};
	std::shared_ptr<Material> m_LitMaterial{};
};
}
