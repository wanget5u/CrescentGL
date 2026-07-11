#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "UI/UISystem.h"

#include <thread>

#include "Input/InputSystem.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace Crescent::UI {

void System::OnCreatePlatform(GLFWwindow *window) {
	if (m_PlatformInitialized == true) {
		return;
	}
	IMGUI_CHECKVERSION();
	m_ImGuiContext = ImGui::CreateContext();
	ImGui::SetCurrentContext(m_ImGuiContext);
	ImGuiIO& imGuiIO = ImGui::GetIO(); (void)imGuiIO;
	imGuiIO.Fonts->AddFontFromFileTTF("Assets/Fonts/GoogleSansFlex.ttf", 22.0f);
	imGuiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	imGuiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, false);
	Input::System::Instance().RegisterListener(this);
	m_PlatformInitialized = true;
	m_FramePrepared = false;
}

void System::OnCreateRenderer() {
	if (m_RendererInitialized == true) {
		return;
	}
	ImGui::SetCurrentContext(m_ImGuiContext);
	ImGui_ImplOpenGL3_Init("#version 460");
	m_RendererInitialized = true;
}

void System::ShutdownRenderer() {
	if (m_RendererInitialized == false) {
		return;
	}
	std::scoped_lock lock(m_Mutex);
	for (size_t a = 0; a < m_Panels.GetSize(); ++a) {
		m_Panels[a]->OnDetach();
	}
	m_Panels.Clear();
	ImGui::SetCurrentContext(m_ImGuiContext);
	ImGui_ImplOpenGL3_Shutdown();
	m_RendererInitialized = false;
}

void System::ShutdownPlatform() {
	if (m_PlatformInitialized == false) {
		return;
	}
	Input::System::Instance().UnregisterListener(this);
	std::scoped_lock lock(m_Mutex);
	ImGui::SetCurrentContext(m_ImGuiContext);
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(m_ImGuiContext);
	m_ImGuiContext = nullptr;
	m_PlatformInitialized = false;
}

void System::OnUpdate() {
	if (m_PlatformInitialized == false) {
		return;
	}
	if (m_FramePrepared.load() == true) {
		return;
	}
	std::scoped_lock lock(m_Mutex);
	PlatformNewFrame();
}

void System::OnRenderGUI(f32 const deltaTime) {
	if (m_PlatformInitialized == false || m_RendererInitialized == false) {
		return;
	}
	if (m_ImGuiContext->IO.DisplaySize.x <= 0.0f || m_ImGuiContext->IO.DisplaySize.y <= 0.0f) {
		return;
	}
	// Wait up to 2ms for the main thread to prepare the platform frame during normal play.
	// When the window is grabbed, it freezes glfwPollEvents() on the main thread,
	// break out after 2ms so the render thread keeps animating without freezing
	std::chrono::time_point waitStartTime = std::chrono::steady_clock::now();
	while (m_FramePrepared.load() == false && m_PlatformInitialized == true) {
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - waitStartTime).count() >= 2) {
			break;
		}
		std::this_thread::yield();
	}
	std::lock_guard lock(m_Mutex);
	BeginFrame(deltaTime);
	m_FramePrepared = false;
	OnRenderGUIInternal(deltaTime);
	EndFrame();
}

void System::PlatformNewFrame() {
	ImGui::SetCurrentContext(m_ImGuiContext);
	ImGui_ImplGlfw_NewFrame();
	if (m_ImGuiContext->IO.DisplaySize.x > 0.0f && m_ImGuiContext->IO.DisplaySize.y > 0.0f) {
		m_FramePrepared = true;
	}
}

void System::BeginFrame(f32 const deltaTime) const {
	ImGui::SetCurrentContext(m_ImGuiContext);
	if (m_ImGuiContext->IO.DeltaTime <= 0.0f) {
		if (deltaTime > 0.0f) {
			m_ImGuiContext->IO.DeltaTime = deltaTime;
		}
		else {
			m_ImGuiContext->IO.DeltaTime = 1.0f / 60.0f;
		}
	}
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
}

void System::EndFrame() const {
	ImGui::SetCurrentContext(m_ImGuiContext);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow* backupCurrentContext = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backupCurrentContext);
	}
}

void System::OnRenderGUIInternal(const f32 deltaTime) {
	for (size_t a = 0; a < m_Panels.GetSize(); ++a) {
		m_Panels[a]->OnUpdate(deltaTime);
		if (m_Panels[a]->IsVisible() == true) {
			m_Panels[a]->OnRender();
		}
	}
}

bool System::WantCaptureMouse() const {
	if (m_PlatformInitialized == false) {
		return false;
	}
	std::scoped_lock lock(m_Mutex);
	ImGui::SetCurrentContext(m_ImGuiContext);
	return ImGui::GetIO().WantCaptureMouse;
}

bool System::WantCaptureKeyboard() const {
	if (m_PlatformInitialized == false) {
		return false;
	}
	std::scoped_lock lock(m_Mutex);
	ImGui::SetCurrentContext(m_ImGuiContext);
	return ImGui::GetIO().WantCaptureKeyboard;
}

void System::OnKeyboardKey(GLFWwindow* window, i32 const key, i32 const scancode, i32 const action, i32 const mods) {
	if (m_PlatformInitialized == false) {
		return;
	}
	std::scoped_lock lock(m_Mutex);
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void System::OnMouseButton(GLFWwindow* window, i32 const button, i32 const action, i32 const mods) {
	if (m_PlatformInitialized == false) {
		return;
	}
	std::scoped_lock lock(m_Mutex);
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

void System::OnMouseScroll(GLFWwindow* window, f64 const xOffset, f64 const yOffset) {
	if (m_PlatformInitialized == false) {
		return;
	}
	std::scoped_lock lock(m_Mutex);
	ImGui_ImplGlfw_ScrollCallback(window, xOffset, yOffset);
}

void System::OnCursorPos(GLFWwindow* window, f64 const xPos, f64 const yPos) {
	if (m_PlatformInitialized == false) {
		return;
	}
	std::scoped_lock lock(m_Mutex);
	ImGui_ImplGlfw_CursorPosCallback(window, xPos, yPos);
}

void System::OnChar(GLFWwindow* window, u32 const c) {
	if (m_PlatformInitialized == false) {
		return;
	}
	std::scoped_lock lock(m_Mutex);
	ImGui_ImplGlfw_CharCallback(window, c);
}

void System::OnWindowFocus(GLFWwindow* window, i32 const focused) {
	if (m_PlatformInitialized == false) {
		return;
	}
	std::scoped_lock lock(m_Mutex);
	ImGui_ImplGlfw_WindowFocusCallback(window, focused);
}

void System::OnCursorEnter(GLFWwindow* window, i32 const entered) {
	if (m_PlatformInitialized == false) {
		return;
	}
	std::scoped_lock lock(m_Mutex);
	ImGui_ImplGlfw_CursorEnterCallback(window, entered);
}

System::~System() {
	ShutdownRenderer();
	ShutdownPlatform();
}

}
