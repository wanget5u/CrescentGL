#pragma once
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
	explicit Scene(std::string name);
	virtual ~Scene();
	virtual void OnUpdate(f32 deltaTime) = 0;
	virtual void OnRender(Window& window) = 0;
	[[nodiscard]] Node* GetRoot() const;
	void MoveCamera(Math::Vector3 const& direction, f32 deltaTime) const;
	void UpdateCamera(f32 deltaTime) const;
	void RotateCamera(Math::Vector3 const& eulerDelta) const;
	void SetCameraFOV(f32 fov) const;
	void SetSceneMaterial(std::shared_ptr<Material> material);
	[[nodiscard]] std::string GetName() const;
	[[nodiscard]] f32 GetCameraSpeed() const;
	[[nodiscard]] f32 GetCameraFOV() const;
	[[nodiscard]] f32 GetCameraZoom() const;
	[[nodiscard]] Math::Vector3 GetCameraPosition() const;
	[[nodiscard]] bool IsLit() const;
	[[nodiscard]] bool IsUnlit() const;
	[[nodiscard]] bool IsWireframe() const;
protected:
	constexpr static f32 MinCameraSpeed = 0.1f;
	constexpr static f32 DefaultCameraSpeed = 10.0f;
	constexpr static f32 MaxCameraSpeed = 50.0f;
	constexpr static f32 MinCameraFOV = 7.0f;
	constexpr static f32 DefaultCameraFOV = 70.0f;
	constexpr static f32 MaxCameraFOV = 140.0f;
	constexpr static f32 CameraLookSensitivity = 0.002f;
	std::string m_Name{};
	std::unique_ptr<Tree> m_Tree{nullptr};
	std::unique_ptr<Camera3D> m_PreviewCamera{nullptr};
	f32 m_CurrentCameraSpeed{DefaultCameraSpeed};
	f32 m_CurrentCameraFOV{DefaultCameraFOV};
	bool m_MoveForward{false};
	bool m_MoveBackward{false};
	bool m_MoveRightward{false};
	bool m_MoveLeftward{false};
	bool m_IsAccelerating{false};
	void SetupInputActions();
	void SetupDefaultMaterials();
	struct ActionSubscription {
		Input::Action* ActionPtr{nullptr};
		u32 SubscriptionID{0};
	};
	DynamicList<ActionSubscription> m_InputSubscriptions{};
	bool m_Lit{true};
	bool m_Unlit{false};
	bool m_Wireframe{false};
	std::shared_ptr<Material> m_Material{};
	std::shared_ptr<Material> m_WireframeMaterial{};
	std::shared_ptr<Material> m_UnlitMaterial{};
	std::shared_ptr<Material> m_LitMaterial{};
};
}
