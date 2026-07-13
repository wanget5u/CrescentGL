#include "Scene/Scene.h"

#include "Asset/AssetLoader.h"
#include "Core/Application.h"
#include "Core/Window.h"
#include "Input/InputSystem.h"
#include "Node/Tree.h"
#include "Node/Node3D/Camera3D.h"
#include "Node/Node3D/Geometry/GeometryInstance3D.h"
#include "Render/Material/Material.h"

namespace Crescent {

Scene::Scene(const std::string& name) : m_Name(name) {
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
	const f32 targetCameraSpeed = m_IsAccelerating ? (m_CurrentCameraSpeed * 2.5f) : m_CurrentCameraSpeed;
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

void Scene::SetCameraFOV(const f32 fov) const {
	m_PreviewCamera->SetPerspective(fov, Application::Instance().GetActiveWindow()->GetAspectRatio());
}

void Scene::UpdateCamera(const f32 deltaTime) const {
	Math::Vector3 direction = Math::Vector3::Zero();
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
	if (direction != Math::Vector3::Zero()) {
		MoveCamera(direction, deltaTime);
	}
}

void Scene::SetSceneMaterial(std::shared_ptr<Material> material) {
	if (material == nullptr || material->Shader == nullptr) {
		return;
	}
	if (m_Tree != nullptr && m_Tree->GetRoot() != nullptr) {
		m_Tree->GetRoot()->ForEachDescendant([this, material](Node* node) {
			if (GeometryInstance3D* geometryInstance3D = dynamic_cast<GeometryInstance3D*>(node)) {
				std::shared_ptr<Material> activeMaterial = geometryInstance3D->GetMaterialOverride();
				if (activeMaterial == nullptr) {
					activeMaterial = geometryInstance3D->GetMaterial();
				}
				if (activeMaterial != nullptr &&
					activeMaterial != m_LitMaterial &&
					activeMaterial != m_UnlitMaterial &&
					activeMaterial != m_WireframeMaterial) {
					activeMaterial->Shader = material->Shader;
				} else {
					geometryInstance3D->SetMaterialOverride(material);
				}
			}
		});
	}
}

std::string Scene::GetName() const {
	return m_Name;
}

f32 Scene::GetCameraSpeed() const {
	return m_CurrentCameraSpeed;
}

f32 Scene::GetCameraFOV() const {
	return m_CurrentCameraFOV;
}

f32 Scene::GetCameraZoom() const {
	return DefaultCameraFOV / m_CurrentCameraFOV;
}

Math::Vector3 Scene::GetCameraPosition() const {
	return m_PreviewCamera->Transform.GetPosition();
}

bool Scene::IsLit() const {
	return m_Lit;
}

bool Scene::IsUnlit() const {
	return m_Unlit;
}

bool Scene::IsWireframe() const {
	return m_Wireframe;
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
			RotateCamera(Math::Vector3(0.0f, -actionEvent.Value, 0.0f));
		}
	});
	m_InputSubscriptions.PushBack({&lookXAction, lookXSubscription});

	Input::Action& lookYAction = editorContext->AddAction("Look_Y");
	lookYAction.BindMouseAxis(Input::MouseAxis::Y, CameraLookSensitivity);
	u32 lookYSubscription = lookYAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (Input::System::Instance().IsMousePressed(Input::MouseButton::Right)) {
			RotateCamera(Math::Vector3(-actionEvent.Value, 0.0f, 0.0f));
		}
	});
	m_InputSubscriptions.PushBack({&lookYAction, lookYSubscription});

	Input::Action& moveForward = editorContext->AddAction("Move_Forward");
	moveForward.BindKeyboardKey(Input::KeyCode::W);
	u32 moveForwardSubscription = moveForward.Subscribe([this](Input::Action::Event const& actionEvent) {
		m_MoveForward = (actionEvent.Phase != Input::Action::Phase::Released);
	});
	m_InputSubscriptions.PushBack({&moveForward, moveForwardSubscription});

	Input::Action& moveBackwardAction = editorContext->AddAction("Move_Backward");
	moveBackwardAction.BindKeyboardKey(Input::KeyCode::S);
	u32 moveBackwardSubscription = moveBackwardAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		m_MoveBackward = (actionEvent.Phase != Input::Action::Phase::Released);
	});
	m_InputSubscriptions.PushBack({&moveBackwardAction, moveBackwardSubscription});

	Input::Action& moveRightwardAction = editorContext->AddAction("Move_Rightward");
	moveRightwardAction.BindKeyboardKey(Input::KeyCode::D);
	u32 moveRightwardSubscription = moveRightwardAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		m_MoveRightward = (actionEvent.Phase != Input::Action::Phase::Released);
	});
	m_InputSubscriptions.PushBack({&moveRightwardAction, moveRightwardSubscription});

	Input::Action& moveLeftward = editorContext->AddAction("Move_Leftward");
	moveLeftward.BindKeyboardKey(Input::KeyCode::A);
	u32 moveLeftwardSubscription = moveLeftward.Subscribe([this](Input::Action::Event const& actionEvent) {
		m_MoveLeftward = (actionEvent.Phase != Input::Action::Phase::Released);
	});
	m_InputSubscriptions.PushBack({&moveLeftward, moveLeftwardSubscription});

	Input::Action& moveAccelerateAction = editorContext->AddAction("Move_Accelerate");
	moveAccelerateAction.BindKeyboardKey(Input::KeyCode::LeftShift);
	u32 moveAccelerateSubscription = moveAccelerateAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		m_IsAccelerating = (actionEvent.Phase != Input::Action::Phase::Released);
	});
	m_InputSubscriptions.PushBack({&moveAccelerateAction, moveAccelerateSubscription});

	Input::Action& cameraScrollAction = editorContext->AddAction("Camera_Scroll");
	cameraScrollAction.BindMouseAxis(Input::MouseAxis::ScrollY, 1.0f);
	u32 cameraScrollSubscription = cameraScrollAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (actionEvent.Value != 0.0f) {
			if (m_IsAccelerating == true) {
				const f32 scrollDelta = actionEvent.Value * 1.5f;
				m_CurrentCameraSpeed = Math::Clamp(m_CurrentCameraSpeed + scrollDelta, MinCameraSpeed, MaxCameraSpeed);
			}
			else {
				const f32 scrollDelta = actionEvent.Value * 5.0f;
				m_CurrentCameraFOV = Math::Clamp(m_CurrentCameraFOV - scrollDelta, MinCameraFOV, MaxCameraFOV);
				SetCameraFOV(m_CurrentCameraFOV);
			}
		}
	});
	m_InputSubscriptions.PushBack({&cameraScrollAction, cameraScrollSubscription});

	Input::Action& setLitAction = editorContext->AddAction("Set_Lit");
	setLitAction.BindKeyboardKey(Input::KeyCode::F1);
	u32 setLitSubscription = setLitAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (actionEvent.Phase == Input::Action::Phase::Pressed) {
			SetSceneMaterial(m_LitMaterial);
			m_Lit = true;
			m_Unlit = false;
			m_Wireframe = false;
		}
	});
	m_InputSubscriptions.PushBack({&setLitAction, setLitSubscription});

	Input::Action& setUnlitAction = editorContext->AddAction("Set_Unlit");
	setUnlitAction.BindKeyboardKey(Input::KeyCode::F2);
	u32 setUnlitSubscription = setUnlitAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (actionEvent.Phase == Input::Action::Phase::Pressed) {
			SetSceneMaterial(m_UnlitMaterial);
			m_Lit = false;
			m_Unlit = true;
			m_Wireframe = false;
		}
	});
	m_InputSubscriptions.PushBack({&setUnlitAction, setUnlitSubscription});

	Input::Action& setWireframeAction = editorContext->AddAction("Set_Wireframe");
	setWireframeAction.BindKeyboardKey(Input::KeyCode::F3);
	u32 setWireframeSubscription = setWireframeAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (actionEvent.Phase == Input::Action::Phase::Pressed) {
			SetSceneMaterial(m_WireframeMaterial);
			m_Lit = false;
			m_Unlit = false;
			m_Wireframe = true;
		}
	});
	m_InputSubscriptions.PushBack({&setWireframeAction, setWireframeSubscription});
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
	m_Material = std::make_shared<Material>(shaderAsset);
}

}
