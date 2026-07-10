#include "Scene/Scene.h"
#include "Core/Window.h"
#include "Scene/Nodes3D/Camera3D.h"
#include "Scene/Tree.h"

namespace Crescent::Scene {

Scene::Scene() {
	m_Tree = std::make_unique<Tree>();
	m_PreviewCamera = std::make_unique<Camera3D>();
}

Scene::~Scene() = default;

Node * Scene::GetRoot() const {
	return m_Tree->GetRoot();
}

void Scene::MoveCamera(Math::Vector3 const &direction, const f32 deltaTime) const {
	Math::Vector3 position = m_PreviewCamera->Transform.GetPosition();
	const Math::Vector3 forwardVector = m_PreviewCamera->Transform.GetForward();
	const Math::Vector3 rightVector = m_PreviewCamera->Transform.GetRight();
	const Math::Vector3 upVector = m_PreviewCamera->Transform.GetUp();
	const f32 targetCameraSpeed = IsAccelerating ? cameraSpeed2 : cameraSpeed1;
	position += forwardVector * direction.z * targetCameraSpeed * deltaTime;
	position += rightVector * direction.x * targetCameraSpeed * deltaTime;
	position += upVector * direction.y * targetCameraSpeed * deltaTime;
	m_PreviewCamera->Transform.SetPosition(position);
}

void Scene::RotateCamera(Math::Vector3 const &eulerDelta) const {
	Math::Vector3 currentEuler = m_PreviewCamera->Transform.GetRotationEuler();
	currentEuler += eulerDelta;
	currentEuler.x = Math::Clamp(currentEuler.x, Math::DegreesToRadians(-89.0f), Math::DegreesToRadians(89.0f));
	m_PreviewCamera->Transform.SetRotationEuler(currentEuler);
}

void Scene::QueueCameraRotation(Math::Vector3 const& eulerDelta) {
	std::scoped_lock lock(m_CameraMutex);
	m_PendingEulerDelta += eulerDelta;
}

void Scene::SetMoveInputForward(const bool active) {
	std::scoped_lock lock(m_CameraMutex);
	m_MoveForward = active;
}

void Scene::SetMoveInputBackward(const bool active) {
	std::scoped_lock lock(m_CameraMutex);
	m_MoveBackward = active;
}

void Scene::SetMoveInputRightward(const bool active) {
	std::scoped_lock lock(m_CameraMutex);
	m_MoveRightward = active;
}

void Scene::SetMoveInputLeftward(const bool active) {
	std::scoped_lock lock(m_CameraMutex);
	m_MoveLeftward = active;
}

void Scene::UpdateCamera(const f32 deltaTime) {
	Math::Vector3 direction = Math::Vector3::Zero();
	Math::Vector3 eulerDelta = Math::Vector3::Zero();
	{
		std::scoped_lock lock(m_CameraMutex);
		if (m_MoveForward) {
			direction += Math::Vector3::Back();
		}
		if (m_MoveBackward) {
			direction += Math::Vector3::Forward();
		}
		if (m_MoveRightward) {
			direction += Math::Vector3::Right();
		}
		if (m_MoveLeftward) {
			direction += Math::Vector3::Left();
		}
		eulerDelta = m_PendingEulerDelta;
		m_PendingEulerDelta = Math::Vector3::Zero();
	}
	if (direction != Math::Vector3::Zero()) {
		MoveCamera(direction, deltaTime);
	}
	if (eulerDelta != Math::Vector3::Zero()) {
		RotateCamera(eulerDelta);
	}
}

}
