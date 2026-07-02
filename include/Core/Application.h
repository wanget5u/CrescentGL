#pragma once
#include "Core/Window.h"

namespace Crescent {

class Application {
public:
	Application();
	~Application();
	void Run();
	bool IsRunning();
	void Close();

private:
	Window* m_Window = nullptr;
	bool m_Running = true;
	void ProcessAndFocus();
	void ProcessInput();
};

}
