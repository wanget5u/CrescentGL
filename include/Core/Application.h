#pragma once
#include "Core/Window.h"

namespace Crescent {

class Application {
public:
	Application();
	~Application();
	void Run();
	[[nodiscard]] bool IsRunning() const;
	void Close();

private:
	Window* m_Window = nullptr;
	bool m_Running = true;
	f32 m_LastFrameTime{};
	f32 m_DeltaTime{};

	void SetupInputActions();
	void ProcessInput() const;
	void UpdateDeltaTime();
};

}
