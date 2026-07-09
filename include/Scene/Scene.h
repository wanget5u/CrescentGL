#pragma once
#include <memory>
#include "Core/Core.h"

namespace Crescent {
struct Window;
namespace Math {
struct Vector3;
}
}

namespace Crescent::Scene {
struct Node;
struct Tree;
struct Camera3D;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
struct Scene {
	bool IsAccelerating{false};
	Scene();
	virtual ~Scene();
	virtual void OnUpdate(f32 deltaTime) = 0;
	virtual void OnRender(Window& window) = 0;
	///
	[[nodiscard]] Node* GetRoot() const;
	///
	void MoveCamera(Math::Vector3 const& direction, f32 deltaTime) const;
	void RotateCamera(Math::Vector3 const& eulerDelta) const;
protected:
	///
	std::unique_ptr<Tree> m_Tree{nullptr};
	///
	constexpr static f32 cameraSpeed1 = 0.25f;
	constexpr static f32 cameraSpeed2 = 0.5f;
	std::unique_ptr<Camera3D> m_PreviewCamera{nullptr};
};
}
