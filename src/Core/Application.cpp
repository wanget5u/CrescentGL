#include "Core/Application.h"
#include <thread>
#include <chrono>

#include "Asset/AssetLoader.h"
#include "Core/Log.h"
#include "Core/Time.h"
#include "Input/InputSystem.h"
#include "Scene/DemoScene.h"

namespace Crescent {

Application::Application() {
	if (glfwInit() == false) {
		Log::Error("Failed to initialize GLFW.");
		return;
	}
	m_MainWindow = std::make_unique<Window>(Window::Properties("CrescentGL", SCREEN_WIDTH, SCREEN_HEIGHT));
	m_LoadWindow = std::make_unique<Window>(Window::Properties("CrescentGL-AssetLoader", 1, 1, false, m_MainWindow.get()));
	// unbinding the opengl contexts from the main thread
	// so the background threads can take them over
	Window::UnbindContext();
	m_MainWindow->ShowWindow();
	SetupInputActions();

	AssetLoader::Instance().OnCreate(m_LoadWindow.get());
}

Application::~Application() {
	AssetLoader::Instance().Shutdown();
	glfwTerminate();
}

void Application::Run() {
	std::thread renderThread(&Application::RenderThreadLoop, this);
	while (m_Running == true && m_MainWindow->ShouldClose() == false) {
		Window::PollEvents();
		ProcessInput();
		if (m_WantsFullscreenToggle == true) {
			while (m_RenderThreadSafeToToggle == false) {
				std::this_thread::yield();
			}
			m_MainWindow->ToggleFullscreen();
			m_WantsFullscreenToggle = false;
		}
	}
	m_Running = false;
	if (renderThread.joinable()) { renderThread.join(); }
}

void Application::SetupInputActions() {
	Input::System::Instance().OnCreate(m_MainWindow->GetWindow());
	Input::Context& appContext = Input::System::Instance().CreateContext(Input::Context::Type::Editor);
	Input::System::Instance().SetActiveContext(Input::Context::Type::Editor);

	Input::Action& closeAction = appContext.AddAction("Close");
	closeAction.BindKeyboardKey(Input::KeyCode::Escape);
	closeAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (actionEvent.Phase == Input::Action::Phase::Pressed) { CloseAction(); }
	});

	Input::Action& fullscreenAction = appContext.AddAction("Fullscreen");
	fullscreenAction.BindKeyboardKey(Input::KeyCode::F11);
	fullscreenAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (actionEvent.Phase == Input::Action::Phase::Pressed) { m_WantsFullscreenToggle = true; }
	});
}

void Application::RenderThreadHandleFullscreenToggleRequest() {
	if (m_WantsFullscreenToggle == true) {
		Window::UnbindContext();
		m_RenderThreadSafeToToggle = true;
		while (m_WantsFullscreenToggle == true) {
			std::this_thread::yield();
		}
		m_MainWindow->MakeContextCurrent();
		m_RenderThreadSafeToToggle = false;
	}
}

void Application::RenderThreadLoop() {
	m_MainWindow->MakeContextCurrent();
	m_ActiveScene = std::make_unique<DemoScene>();

	while (m_Running == true) {
		RenderThreadHandleFullscreenToggleRequest();
		Time::OnUpdate(static_cast<f32>(glfwGetTime()));
		while (Time::AccumulatorHasSubstep()) {
			Time::ConsumeSubstep();
		}
		m_ActiveScene->OnUpdate(Time::GetVariableDeltaTime());
		m_ActiveScene->OnRender(*m_MainWindow);
		m_MainWindow->SwapBuffers();
	}

	m_ActiveScene.reset();
	Window::UnbindContext();
}

}
