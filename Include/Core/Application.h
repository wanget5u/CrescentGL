#pragma once
#include <atomic>
#include <memory>
#include <mutex>

#include "Core/Core.h"

namespace Crescent {
struct Window;
namespace Scene {
struct Scene;
}
namespace Input {
struct Context;
}
struct Application {
	Application();
	~Application();
	void Run();
private:
	std::unique_ptr<Window> m_MainWindow;
	std::unique_ptr<Window> m_LoadWindow;
	mutable std::mutex m_ActiveSceneMutex;
	std::unique_ptr<Scene::Scene> m_ActiveScene;
	std::atomic<bool> m_Running{true};
	std::atomic<bool> m_WantsFullscreenToggle{false};
	std::atomic<bool> m_RenderThreadSafeToToggle{false};
	std::unique_ptr<Input::Context> m_EditorInputContext;
	std::unique_ptr<Input::Context> m_GameInputContext;
	void SetupGlobalInputActions();
	void SetupUIPanels();
	void RenderThreadLoop();
};
}
