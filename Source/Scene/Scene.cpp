#include "Scene/Scene.h"

#include "Asset/AssetType.h"
#include "Asset/AssetLoader.h"
#include "Core/Window.h"
#include "Input/InputAction.h"
#include "Input/InputSystem.h"
#include "Render/Material/Material.h"
#include "Scene/Nodes3D/Camera3D.h"
#include "Scene/Nodes3D/Geometry/GeometryInstance3D.h"
#include "Scene/Tree.h"

namespace Crescent {

Scene::Scene() {
	m_Tree = std::make_unique<Tree>();
	m_PreviewCamera = std::make_unique<Camera3D>();
	SetupInputActions();
	SetupDefaultMaterials();
}

Scene::~Scene() {
	for (ActionSubscription const& action : m_InputSubscriptions) {
		action.ActionPtr->Unsubscribe(action.SubscriptionID);
	}
}

Node * Scene::GetRoot() const {
	return m_Tree->GetRoot();
}

void Scene::MoveCamera(Math::Vector3 const &direction, const f32 deltaTime) const {
	Math::Vector3 position = m_PreviewCamera->Transform.GetPosition();
	const Math::Vector3 forwardVector = m_PreviewCamera->Transform.GetForward();
	const Math::Vector3 rightVector = m_PreviewCamera->Transform.GetRight();
	const Math::Vector3 upVector = m_PreviewCamera->Transform.GetUp();
	const f32 targetCameraSpeed = IsAccelerating ? CameraSpeed2 : CameraSpeed1;
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
	m_PendingEulerDelta += eulerDelta;
}

void Scene::SetMoveInputForward(const bool active) {
	m_MoveForward = active;
}

void Scene::SetMoveInputBackward(const bool active) {
	m_MoveBackward = active;
}

void Scene::SetMoveInputRightward(const bool active) {
	m_MoveRightward = active;
}

void Scene::SetMoveInputLeftward(const bool active) {
	m_MoveLeftward = active;
}

void Scene::UpdateCamera(const f32 deltaTime) {
	Math::Vector3 direction = Math::Vector3::Zero();
	Math::Vector3 eulerDelta{};
	if (m_MoveForward == true) {
		direction += Math::Vector3::Back();
	}
	if (m_MoveBackward == true) {
		direction += Math::Vector3::Forward();
	}
	if (m_MoveRightward == true) {
		direction += Math::Vector3::Right();
	}
	if (m_MoveLeftward == true) {
		direction += Math::Vector3::Left();
	}
	eulerDelta = m_PendingEulerDelta;
	m_PendingEulerDelta = Math::Vector3::Zero();
	if (direction != Math::Vector3::Zero()) {
		MoveCamera(direction, deltaTime);
	}
	if (eulerDelta != Math::Vector3::Zero()) {
		RotateCamera(eulerDelta);
	}
}

void Scene::SetSceneMaterial(std::shared_ptr<Material> material) {
	m_Material = material;
	if (m_Tree != nullptr && m_Tree->GetRoot() != nullptr) {
		m_Tree->GetRoot()->ForEachDescendant([material](Node* node) {
			if (GeometryInstance3D* geom = dynamic_cast<GeometryInstance3D*>(node)) {
				if (geom->GetMaterialOverride() != nullptr) {
					geom->SetMaterialOverride(material);
				} else if (geom->GetMaterial() != nullptr) {
					geom->SetMaterial(material);
				} else {
					geom->SetMaterialOverride(material);
				}
			}
		});
	}
}

void Scene::SetupInputActions() {
	Input::Context* editorContext = Input::System::Instance().GetContext(Input::Context::Type::SceneEditor);
	Input::System::Instance().SetContextActive(Input::Context::Type::SceneEditor);

	Input::Action& focusWindowAction = editorContext->AddAction("Focus_Window");
	focusWindowAction.BindMouseButton(Input::MouseButton::Right);
	u32 focusActionSubscription = focusWindowAction.Subscribe([](Input::Action::Event const& actionEvent) {
		GLFWwindow* window = Input::System::Instance().GetWindow();
		if (actionEvent.Phase == Input::Action::Phase::Pressed) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			f64 x = 0.0;
			f64 y = 0.0;
			Input::System::Instance().GetCursorPos(x, y);
			Input::System::Instance().SetCursorPos(x, y);
		}
		else if (actionEvent.Phase == Input::Action::Phase::Released) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			f64 x = 0.0;
			f64 y = 0.0;
			Input::System::Instance().GetCursorPos(x, y);
			Input::System::Instance().SetCursorPos(x, y);
		}
	});
	m_InputSubscriptions.PushBack({&focusWindowAction, focusActionSubscription});

	Input::Action& lookXAction = editorContext->AddAction("Look_X");
	lookXAction.BindMouseAxis(Input::MouseAxis::X, CameraLookSensitivity);
	u32 lookXSubscription = lookXAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (Input::System::Instance().IsMousePressed(Input::MouseButton::Right)) {
			QueueCameraRotation(Math::Vector3(0.0f, -actionEvent.Value, 0.0f));
		}
	});
	m_InputSubscriptions.PushBack({&lookXAction, lookXSubscription});

	Input::Action& lookYAction = editorContext->AddAction("Look_Y");
	lookYAction.BindMouseAxis(Input::MouseAxis::Y, CameraLookSensitivity);
	u32 lookYSubscription = lookYAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (Input::System::Instance().IsMousePressed(Input::MouseButton::Right)) {
			QueueCameraRotation(Math::Vector3(-actionEvent.Value, 0.0f, 0.0f));
		}
	});
	m_InputSubscriptions.PushBack({&lookYAction, lookYSubscription});

	Input::Action& moveForward = editorContext->AddAction("Move_Forward");
	moveForward.BindKeyboardKey(Input::KeyCode::W);
	u32 moveForwardSubscription = moveForward.Subscribe([this](Input::Action::Event const& actionEvent) {
		SetMoveInputForward(actionEvent.Phase != Input::Action::Phase::Released);
	});
	m_InputSubscriptions.PushBack({&moveForward, moveForwardSubscription});

	Input::Action& moveBackwardAction = editorContext->AddAction("Move_Backward");
	moveBackwardAction.BindKeyboardKey(Input::KeyCode::S);
	u32 moveBackwardSubscription = moveBackwardAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		SetMoveInputBackward(actionEvent.Phase != Input::Action::Phase::Released);
	});
	m_InputSubscriptions.PushBack({&moveBackwardAction, moveBackwardSubscription});

	Input::Action& moveRightwardAction = editorContext->AddAction("Move_Rightward");
	moveRightwardAction.BindKeyboardKey(Input::KeyCode::D);
	u32 moveRightwardSubscription = moveRightwardAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		SetMoveInputRightward(actionEvent.Phase != Input::Action::Phase::Released);
	});
	m_InputSubscriptions.PushBack({&moveRightwardAction, moveRightwardSubscription});

	Input::Action& moveLeftward = editorContext->AddAction("Move_Leftward");
	moveLeftward.BindKeyboardKey(Input::KeyCode::A);
	u32 moveLeftwardSubscription = moveLeftward.Subscribe([this](Input::Action::Event const& actionEvent) {
		SetMoveInputLeftward(actionEvent.Phase != Input::Action::Phase::Released);
	});
	m_InputSubscriptions.PushBack({&moveLeftward, moveLeftwardSubscription});

	Input::Action& moveAccelerateAction = editorContext->AddAction("Move_Accelerate");
	moveAccelerateAction.BindKeyboardKey(Input::KeyCode::LeftShift);
	u32 moveAccelerateSubscription = moveAccelerateAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		IsAccelerating = (actionEvent.Phase != Input::Action::Phase::Released);
	});
	m_InputSubscriptions.PushBack({&moveAccelerateAction, moveAccelerateSubscription});

	Input::Action& setLitAction = editorContext->AddAction("Set_Lit");
	setLitAction.BindKeyboardKey(Input::KeyCode::F1);
	u32 setLitSubscription = setLitAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (actionEvent.Phase == Input::Action::Phase::Pressed) {
			SetSceneMaterial(m_LitMaterial);
		}
	});
	m_InputSubscriptions.PushBack({&setLitAction, setLitSubscription});

	Input::Action& setUnlitAction = editorContext->AddAction("Set_Unlit");
	setUnlitAction.BindKeyboardKey(Input::KeyCode::F2);
	u32 setUnlitSubscription = setUnlitAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (actionEvent.Phase == Input::Action::Phase::Pressed) {
			SetSceneMaterial(m_UnlitMaterial);
		}
	});
	m_InputSubscriptions.PushBack({&setUnlitAction, setUnlitSubscription});

	Input::Action& setWireframeAction = editorContext->AddAction("Set_Wireframe");
	setWireframeAction.BindKeyboardKey(Input::KeyCode::F3);
	u32 setWireframeSubscription = setWireframeAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (actionEvent.Phase == Input::Action::Phase::Pressed) {
			SetSceneMaterial(m_WireframeMaterial);
		}
	});
	m_InputSubscriptions.PushBack({&setWireframeAction, setWireframeSubscription});

	// TODO: add camera zoom with mouse scroll
	// TODO: add acceleration with shift mouse scroll
}

void Scene::SetupDefaultMaterials() {
	std::shared_ptr<ShaderAsset> shaderAsset = AssetLoader::Instance()
		.GetOrLoad<ShaderAsset>("Shaders/wire", AssetType::Shader);
	m_WireframeMaterial = std::make_shared<Material>(shaderAsset);
	shaderAsset = AssetLoader::Instance()
		.GetOrLoad<ShaderAsset>("Shaders/unlit", AssetType::Shader);
	m_UnlitMaterial = std::make_shared<Material>(shaderAsset);
	shaderAsset = AssetLoader::Instance()
		.GetOrLoad<ShaderAsset>("Shaders/lit", AssetType::Shader);
	m_LitMaterial = std::make_shared<Material>(shaderAsset);
	m_Material = m_LitMaterial;
}

}
