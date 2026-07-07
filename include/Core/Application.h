#pragma once
#include <atomic>
#include <memory>

#include "Core/Window.h"
#include "Scene/Scene.h"

namespace Crescent {
struct Application {
	Application();
	~Application();
	void Run();
private:
	void CloseAction();
	void FullscreenAction() const;
	void SetupAndBindInputActions();
	std::unique_ptr<Window> m_MainWindow;
	std::unique_ptr<Window> m_LoadWindow;
	std::unique_ptr<Scene::Scene> m_ActiveScene;
	std::atomic<bool> m_Running{true};
	std::atomic<bool> m_WantsFullscreenToggle{false};
	std::atomic<bool> m_RenderThreadSafeToToggle{false};
	void RenderThreadLoop();
};
}