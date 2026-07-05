#pragma once
#include <atomic>
#include <memory>
#include "Core/Window.h"
#include "Input/InputSystem.h"
#include "Scene/Scene.h"

namespace Crescent {

struct Application {
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Lifecycle
	Application();
	~Application();
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Execution Control
	void Run();
private:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Input Actions
	void CloseAction()            { m_Running = false; }
	void FullscreenAction() const { m_MainWindow->ToggleFullscreen(); }
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Application Update Cycle
	void SetupInputActions();
	void ProcessInput() const { Input::System::Instance().OnUpdate(); }
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Core Window & State Members
	std::unique_ptr<Window> m_MainWindow;
	std::unique_ptr<Window> m_LoadWindow;
	std::unique_ptr<Scene>  m_ActiveScene;
	std::atomic<bool>       m_Running{true};
	std::atomic<bool>       m_WantsFullscreenToggle{false};
	std::atomic<bool>       m_RenderThreadSafeToToggle{false};
	void RenderThreadHandleFullscreenToggleRequest();
	void RenderThreadLoop();
};

}