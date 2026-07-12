#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "UI/UISystem.h"

#include "Input/InputSystem.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Scene/Scene.h"

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
	ImGui::SetCurrentContext(m_ImGuiContext);
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(m_ImGuiContext);
	m_ImGuiContext = nullptr;
	m_PlatformInitialized = false;
}

void System::OnUpdate(f32 const deltaTime) {
	if (m_PlatformInitialized == false || m_RendererInitialized == false) {
		return;
	}
	BeginFrame(deltaTime);
}

void System::OnRenderGUI(f32 const deltaTime) {
	if (m_PlatformInitialized == false || m_RendererInitialized == false) {
		return;
	}
	if (m_ImGuiContext->IO.DisplaySize.x <= 0.0f || m_ImGuiContext->IO.DisplaySize.y <= 0.0f) {
		ImGui::SetCurrentContext(m_ImGuiContext);
		ImGui::EndFrame();
		return;
	}
	UpdateActivePanels(deltaTime);
	EndFrame();
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
	} else if (deltaTime > 0.0f) {
		m_ImGuiContext->IO.DeltaTime = deltaTime;
	}
	ImGui_ImplGlfw_NewFrame();
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

void System::UpdateActivePanels(const f32 deltaTime) {
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
	ImGui::SetCurrentContext(m_ImGuiContext);
	return ImGui::GetIO().WantCaptureMouse;
}

bool System::WantCaptureKeyboard() const {
	if (m_PlatformInitialized == false) {
		return false;
	}
	ImGui::SetCurrentContext(m_ImGuiContext);
	return ImGui::GetIO().WantCaptureKeyboard;
}

void System::OnKeyboardKey(GLFWwindow* window, i32 const key, i32 const scancode, i32 const action, i32 const mods) {
	if (m_PlatformInitialized == false) {
		return;
	}
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void System::OnMouseButton(GLFWwindow* window, i32 const button, i32 const action, i32 const mods) {
	if (m_PlatformInitialized == false) {
		return;
	}
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

void System::OnMouseScroll(GLFWwindow* window, f64 const xOffset, f64 const yOffset) {
	if (m_PlatformInitialized == false) {
		return;
	}
	ImGui_ImplGlfw_ScrollCallback(window, xOffset, yOffset);
}

void System::OnCursorPos(GLFWwindow* window, f64 const xPos, f64 const yPos) {
	if (m_PlatformInitialized == false) {
		return;
	}
	ImGui_ImplGlfw_CursorPosCallback(window, xPos, yPos);
}

void System::OnChar(GLFWwindow* window, u32 const c) {
	if (m_PlatformInitialized == false) {
		return;
	}
	ImGui_ImplGlfw_CharCallback(window, c);
}

void System::OnWindowFocus(GLFWwindow* window, i32 const focused) {
	if (m_PlatformInitialized == false) {
		return;
	}
	ImGui_ImplGlfw_WindowFocusCallback(window, focused);
}

void System::OnCursorEnter(GLFWwindow* window, i32 const entered) {
	if (m_PlatformInitialized == false) {
		return;
	}
	ImGui_ImplGlfw_CursorEnterCallback(window, entered);
}

System::~System() {
	ShutdownRenderer();
	ShutdownPlatform();
}

}
