#include "Core/Application.h"

#include <chrono>
#include <thread>

#include "Core/Log.h"
#include "Core/Random.h"
#include "Core/Time.h"
#include "Core/Window.h"
#include "Scene/Scene.h"
#include "Asset/Loader.h"
#include "Asset/Registry.h"
#include "Input/InputSystem.h"
#include "Input/InputAction.h"
#include "Render/RenderStats.h"
#include "Render/GPUDisposalQueue.h"
#include "Scene/DemoScene.h"
#include "UI/UISystem.h"
#include "UI/DebugPanel.h"

namespace Crescent {

Application::Application() {
	if (glfwInit() == false) {
		Log::Error("Failed to initialize GLFW.");
		return;
	}
	m_MainWindow = std::make_unique<Window>(Window::Properties("CrescentGL", SCREEN_WIDTH, SCREEN_HEIGHT));
	m_LoadWindow = std::make_unique<Window>(Window::Properties("CrescentGL-AssetLoader", 1, 1, false, m_MainWindow.get()));
	Window::UnbindContext();
	m_MainWindow->ShowWindow();
	Random::Initialize();
	Asset::Loader::Instance().OnCreate(m_LoadWindow.get());
	UI::System::Instance().OnCreatePlatform(m_MainWindow->GetWindow());
	Input::System::Instance().OnCreate(m_MainWindow->GetWindow());
	SetupGlobalInputActions();
	Input::System::Instance().SetContextActive(Input::Context::Type::Global);
}

Application::~Application() {
	Asset::Loader::Instance().Shutdown();
	UI::System::Instance().ShutdownPlatform();
	glfwTerminate();
}

void Application::Run() {
	std::thread renderThread(&Application::RenderThreadLoop, this);
	while (m_Running == true && m_MainWindow->ShouldClose() == false) {
		Window::PollEvents();
		UI::System::Instance().OnUpdate();
		Input::System::Instance().OnUpdate();
		if (m_WantsFullscreenToggle == true) {
			while (m_RenderThreadSafeToToggle == false) {
				std::this_thread::yield();
			}
			m_MainWindow->ToggleFullscreen();
			Window::PollEvents();
			m_WantsFullscreenToggle = false;
		}
	}
	m_Running = false;
	if (renderThread.joinable()) {
		renderThread.join();
	}
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
	toggleDebugPanel.BindKeyboardKey(Input::KeyCode::F1);
	toggleDebugPanel.Subscribe([](Input::Action::Event const& actionEvent) {
		if (actionEvent.Phase == Input::Action::Phase::Pressed) {
			UI::DebugPanel* debugPanel = UI::System::Instance().GetPanel<UI::DebugPanel>();
			debugPanel->SetVisibility(!debugPanel->IsVisible());
		}
	});
}

void Application::SetupUIPanels() {
	UI::System::Instance().RegisterPanel<UI::DebugPanel>("Debug");
	// TODO: SceneHierarchy, SceneTab, Toolbar
}

void Application::RenderThreadLoop() {
	m_MainWindow->MakeContextCurrent();
	UI::System::Instance().OnCreateRenderer();
	SetupUIPanels();
	{
		std::scoped_lock lock(m_ActiveSceneMutex);
		m_ActiveScene = std::make_unique<Scene::DemoScene>();
	}
	f64 cpuStartTime{};
	while (m_Running == true) {
		cpuStartTime = glfwGetTime();
		if (m_WantsFullscreenToggle == true) {
			Window::UnbindContext();
			m_RenderThreadSafeToToggle = true;
			while (m_WantsFullscreenToggle == true) {
				std::this_thread::yield();
			}
			m_MainWindow->MakeContextCurrent();
			m_RenderThreadSafeToToggle = false;
		}
		Time::OnUpdate(static_cast<f32>(glfwGetTime()));
		while (Time::AccumulatorHasSubstep()) {
			Time::ConsumeSubstep();
		}
		m_MainWindow->CheckViewportResize();
		Asset::Registry::Instance().OnUpdate();
		m_ActiveScene->UpdateCamera(Time::GetVariableDeltaTime());
		m_ActiveScene->OnUpdate(Time::GetVariableDeltaTime());
		m_ActiveScene->OnRender(*m_MainWindow);
		UI::System::Instance().OnRenderGUI(Time::GetVariableDeltaTime());
		Render::Stats::Instance().CPUFrameTimeMs = static_cast<f32>((glfwGetTime() - cpuStartTime) * 1000.0f);
		m_MainWindow->SwapBuffers();
	}
	Asset::Loader::Instance().Shutdown();
	{
		std::scoped_lock lock(m_ActiveSceneMutex);
		m_ActiveScene.reset();
	}
	UI::System::Instance().ShutdownRenderer();
	Asset::Registry::Instance().Clear();
	Render::GPUDisposalQueue::Flush();
	Window::UnbindContext();
}

}