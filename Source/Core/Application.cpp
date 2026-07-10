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
#include "Scene/DemoScene.h"

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
	SetupAndBindInputActions();

	Asset::Loader::Instance().OnCreate(m_LoadWindow.get());
	Random::Initialize();
}

Application::~Application() {
	Asset::Loader::Instance().Shutdown();
	glfwTerminate();
}

void Application::Run() {
	std::thread renderThread(&Application::RenderThreadLoop, this);
	while (m_Running == true && m_MainWindow->ShouldClose() == false) {
		Window::PollEvents();
		Input::System::Instance().OnUpdate();
		if (m_WantsFullscreenToggle == true) {
			while (m_RenderThreadSafeToToggle == false) {
				std::this_thread::yield();
			}
			m_MainWindow->ToggleFullscreen();
			m_WantsFullscreenToggle = false;
		}
	}
	m_Running = false;
	if (renderThread.joinable()) {
		renderThread.join();
	}
}

void Application::CloseAction() {
	m_Running = false;
}

void Application::FullscreenAction() const {
	m_MainWindow->ToggleFullscreen();
}

void Application::SetupAndBindInputActions() {
	Input::System::Instance().OnCreate(m_MainWindow->GetWindow());
	Input::Context& appContext = Input::System::Instance().CreateContext(Input::Context::Type::Editor);
	Input::System::Instance().SetActiveContext(Input::Context::Type::Editor);

	Input::Action& closeAction = appContext.AddAction("Close");
	closeAction.BindKeyboardKey(Input::KeyCode::Escape);
	closeAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (actionEvent.Phase == Input::Action::Phase::Pressed) {
			CloseAction();
		}
	});

	Input::Action& fullscreenAction = appContext.AddAction("Fullscreen");
	fullscreenAction.BindKeyboardKey(Input::KeyCode::F11);
	fullscreenAction.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (actionEvent.Phase == Input::Action::Phase::Pressed) {
			m_WantsFullscreenToggle = true;
		}
	});

	constexpr f32 lookSensitivity = 0.002f;
	Input::Action& focusWindow = appContext.AddAction("Focus_Window");
	focusWindow.BindMouseButton(Input::MouseButton::Right);
	focusWindow.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (actionEvent.Phase == Input::Action::Phase::Pressed) {
			glfwSetInputMode(m_MainWindow->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			f64 x = 0.0;
			f64 y = 0.0;
			Input::System::Instance().GetCursorPos(x, y);
			Input::System::Instance().SetCursorPos(x, y);
		}
		else if (actionEvent.Phase == Input::Action::Phase::Released) {
			glfwSetInputMode(m_MainWindow->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			f64 x = 0.0;
			f64 y = 0.0;
			Input::System::Instance().GetCursorPos(x, y);
			Input::System::Instance().SetCursorPos(x, y);
		}
	});

	Input::Action& lookX = appContext.AddAction("Look_X");
	lookX.BindMouseAxis(Input::MouseAxis::X, lookSensitivity);
	lookX.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (Input::System::Instance().IsMousePressed(Input::MouseButton::Right)) {
			std::scoped_lock lock(m_ActiveSceneMutex);
			if (m_ActiveScene) {
				m_ActiveScene->QueueCameraRotation(Math::Vector3(0.0f, -actionEvent.Value, 0.0f));
			}
		}
	});

	Input::Action& lookY = appContext.AddAction("Look_Y");
	lookY.BindMouseAxis(Input::MouseAxis::Y, lookSensitivity);
	lookY.Subscribe([this](Input::Action::Event const& actionEvent) {
		if (Input::System::Instance().IsMousePressed(Input::MouseButton::Right)) {
			std::scoped_lock lock(m_ActiveSceneMutex);
			if (m_ActiveScene) {
				m_ActiveScene->QueueCameraRotation(Math::Vector3(-actionEvent.Value, 0.0f, 0.0f));
			}
		}
	});

	Input::Action& moveForward = appContext.AddAction("Move_Forward");
	moveForward.BindKeyboardKey(Input::KeyCode::W);
	moveForward.Subscribe([this](Input::Action::Event const& actionEvent) {
		std::scoped_lock lock(m_ActiveSceneMutex);
		m_ActiveScene->SetMoveInputForward(actionEvent.Phase != Input::Action::Phase::Released);
	});

	Input::Action& moveBackward = appContext.AddAction("Move_Backward");
	moveBackward.BindKeyboardKey(Input::KeyCode::S);
	moveBackward.Subscribe([this](Input::Action::Event const& actionEvent) {
		std::scoped_lock lock(m_ActiveSceneMutex);
		m_ActiveScene->SetMoveInputBackward(actionEvent.Phase != Input::Action::Phase::Released);
	});

	Input::Action& moveRightward = appContext.AddAction("Move_Rightward");
	moveRightward.BindKeyboardKey(Input::KeyCode::D);
	moveRightward.Subscribe([this](Input::Action::Event const& actionEvent) {
		std::scoped_lock lock(m_ActiveSceneMutex);
		m_ActiveScene->SetMoveInputRightward(actionEvent.Phase != Input::Action::Phase::Released);
	});

	Input::Action& moveLeftward = appContext.AddAction("Move_Leftward");
	moveLeftward.BindKeyboardKey(Input::KeyCode::A);
	moveLeftward.Subscribe([this](Input::Action::Event const& actionEvent) {
		std::scoped_lock lock(m_ActiveSceneMutex);
		m_ActiveScene->SetMoveInputLeftward(actionEvent.Phase != Input::Action::Phase::Released);
	});

	Input::Action& moveAccelerate = appContext.AddAction("Move_Accelerate");
	moveAccelerate.BindKeyboardKey(Input::KeyCode::LeftShift);
	moveAccelerate.Subscribe([this](Input::Action::Event const& actionEvent) {
		std::scoped_lock lock(m_ActiveSceneMutex);
		m_ActiveScene->IsAccelerating = (actionEvent.Phase != Input::Action::Phase::Released);
	});

	// TODO: add camera zoom with mouse scroll
	// TODO: add acceleration with shift mouse scroll
}

void Application::RenderThreadLoop() {
	m_MainWindow->MakeContextCurrent();
	{
		std::scoped_lock lock(m_ActiveSceneMutex);
		m_ActiveScene = std::make_unique<Scene::DemoScene>();
	}
	while (m_Running == true) {
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
		timer += Time::GetVariableDeltaTime();
		if (timer > 0.5f) {
			timer = 0;
			Log::Print("FPS: {} | Frame Time: {:.3f} ms", Math::Ceil(Time::GetFPS()), Time::GetVariableDeltaTimeMs());
		}
		m_MainWindow->SwapBuffers();
	}
	{
		std::scoped_lock lock(m_ActiveSceneMutex);
		m_ActiveScene.reset();
	}
	Window::UnbindContext();
}

}
