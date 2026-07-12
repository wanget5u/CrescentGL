#include "Core/Application.h"

#include <chrono>

#include "Core/Log.h"
#include "Core/Random.h"
#include "Core/Time.h"
#include "Core/Window.h"
#include "Scene/Scene.h"
#include "Asset/AssetLoader.h"
#include "Input/InputSystem.h"
#include "Input/InputAction.h"
#include "Render/RenderStats.h"
#include "Render/GPUDisposalQueue.h"
#include "Scene/DemoScene.h"
#include "UI/UISystem.h"
#include "UI/DebugPanel.h"

namespace Crescent {

static Application* s_Instance = nullptr;

Application::Application() {
	s_Instance = this;
	if (glfwInit() == false) {
		Log::Error("Failed to initialize GLFW.");
		return;
	}
	m_MainWindow = std::make_unique<Window>(Window::Properties("CrescentGL", SCREEN_WIDTH, SCREEN_HEIGHT));
	m_MainWindow->ShowWindow();
	Random::Initialize();
	UI::System::Instance().OnCreatePlatform(m_MainWindow->GetWindow());
	Input::System::Instance().OnCreate(m_MainWindow->GetWindow());
	SetupGlobalInputActions();
	Input::System::Instance().SetContextActive(Input::Context::Type::Global);
}

Application::~Application() {
	m_ActiveScene.reset();
	UI::System::Instance().ShutdownRenderer();
	AssetLoader::Instance().Shutdown();
	GPUDisposalQueue::Flush();
	UI::System::Instance().ShutdownPlatform();
	glfwTerminate();
	s_Instance = nullptr;
}

Application& Application::Instance() {
	return *s_Instance;
}

void Application::Run() {
	m_MainWindow->MakeContextCurrent();
	UI::System::Instance().OnCreateRenderer();
	SetupUIPanels();
	m_ActiveScene = std::make_unique<DemoScene>();
	f64 cpuStartTime{};
	while (m_Running == true && m_MainWindow->ShouldClose() == false) {
		cpuStartTime = glfwGetTime();
		Window::PollEvents();
		if (m_WantsFullscreenToggle == true) {
			m_MainWindow->ToggleFullscreen();
			m_WantsFullscreenToggle = false;
		}
		Time::OnUpdate(static_cast<f32>(glfwGetTime()));
		while (Time::AccumulatorHasSubstep()) {
			Time::ConsumeSubstep();
		}
		m_MainWindow->CheckViewportResize();
		Input::System::Instance().OnUpdate();
		UI::System::Instance().OnUpdate(Time::GetVariableDeltaTime());
		m_ActiveScene->UpdateCamera(Time::GetVariableDeltaTime());
		m_ActiveScene->OnUpdate(Time::GetVariableDeltaTime());
		m_ActiveScene->OnRender(*m_MainWindow);
		UI::System::Instance().OnRenderGUI(Time::GetVariableDeltaTime());
		RenderStats::Instance().CPUFrameTimeMs = static_cast<f32>((glfwGetTime() - cpuStartTime) * 1000.0f);
		m_MainWindow->SwapBuffers();
	}
	m_Running = false;
}

void Application::OnRender() const {
	if (m_Running == false || m_MainWindow == nullptr || m_ActiveScene == nullptr) {
		return;
	}
	if (m_MainWindow->GetWindowWidth() <= 0 || m_MainWindow->GetWindowHeight() <= 0) {
		return;
	}
	m_MainWindow->MakeContextCurrent();
	m_MainWindow->CheckViewportResize();
	Time::OnUpdate(static_cast<f32>(glfwGetTime()));
	while (Time::AccumulatorHasSubstep()) {
		Time::ConsumeSubstep();
	}
	const f32 deltaTime = Time::GetVariableDeltaTime();
	Input::System::Instance().OnUpdate();
	UI::System::Instance().OnUpdate(deltaTime);
	m_ActiveScene->UpdateCamera(deltaTime);
	m_ActiveScene->OnUpdate(deltaTime);
	m_ActiveScene->OnRender(*m_MainWindow);
	UI::System::Instance().OnRenderGUI(deltaTime);
	m_MainWindow->SwapBuffers();
}

Scene* Application::GetActiveScene() const {
	return m_ActiveScene.get();
}

Window * Application::GetActiveWindow() const {
	return m_MainWindow.get();
}

void Application::SetupGlobalInputActions() {
	Input::Context* editorContext = Input::System::Instance().GetContext(Input::Context::Type::Global);

	Input::Action& closeAction = editorContext->AddAction("Close");
	closeAction.BindKeyboardKey(Input::KeyCode::Escape);
	closeAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (actionEvent.Phase == Input::Action::Phase::Pressed) {
			m_Running = false;
		}
	});

	Input::Action& fullscreenAction = editorContext->AddAction("Fullscreen");
	fullscreenAction.BindKeyboardKey(Input::KeyCode::F11);
	fullscreenAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (actionEvent.Phase == Input::Action::Phase::Pressed) {
			m_WantsFullscreenToggle = true;
		}
	});

	Input::Action& toggleDebugPanel = editorContext->AddAction("Toggle_Debug");
	toggleDebugPanel.BindKeyboardKey(Input::KeyCode::F4);
	toggleDebugPanel.Subscribe([](Input::Action::Event const& actionEvent) {
		if (actionEvent.Phase == Input::Action::Phase::Pressed) {
			UI::DebugPanel* debugPanel = UI::System::Instance().GetPanel<UI::DebugPanel>();
			debugPanel->SetVisibility(!debugPanel->IsVisible());
		}
	});
}

void Application::SetupUIPanels() const {
	UI::System::Instance().RegisterPanel<UI::DebugPanel>("Debug");
	// TODO: SceneHierarchy, SceneTab, Toolbar
}

}