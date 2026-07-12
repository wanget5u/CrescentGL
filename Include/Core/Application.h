#pragma once
#include <atomic>
#include <memory>

namespace Crescent {
	struct Window;
	struct Scene;
}
namespace Crescent::Input {
	struct Context;
}

namespace Crescent {
struct Application {
	Application();
	~Application();
	static Application& Instance();
	void Run();
	void OnRender() const;
	Scene* GetActiveScene() const;
	Window* GetActiveWindow() const;
private:
	std::unique_ptr<Window> m_MainWindow;
	std::unique_ptr<Scene> m_ActiveScene;
	std::atomic<bool> m_Running{true};
	std::atomic<bool> m_WantsFullscreenToggle{false};
	std::unique_ptr<Input::Context> m_EditorInputContext;
	std::unique_ptr<Input::Context> m_GameInputContext;
	void SetupGlobalInputActions();
	void SetupUIPanels() const;
};
}
