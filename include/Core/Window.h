#pragma once
#include <atomic>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Core/Core.h"

namespace Crescent {

struct Window {
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Types & Sub-structs
    struct Properties {
       const char8* Title {""};
       u32 Width          {0};
       u32 Height         {0};
       bool Visible       {false};
       Window* ShareWindow{nullptr};

       explicit Properties(
          char8 const* title = "CrescentGL",
          u32 const width = SCREEN_WIDTH,
          u32 const height = SCREEN_HEIGHT,
          bool const visible = true,
          Window* shareWindow = nullptr)
          :
          Title(title),
          Width(width),
          Height(height),
          Visible(visible),
          ShareWindow(shareWindow) {}
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Lifecycle
    explicit Window(Properties const& properties = Properties());
    ~Window() { glfwDestroyWindow(m_Window); }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Static Context & Monitor Utilities
    static void UnbindContext() { glfwMakeContextCurrent(nullptr); }
    static void PollEvents() { glfwPollEvents(); }
    static GLFWmonitor* GetActiveMonitor() {
       GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
       if (primaryMonitor != nullptr) { return primaryMonitor; }
       i32 monitorCount{};
       GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
       if (monitors != nullptr && monitorCount > 0) { return monitors[0]; }
       return nullptr;
    }
    static const GLFWvidmode* GetActiveMonitorVideoMode() {
       GLFWmonitor* monitor = GetActiveMonitor();
       if (monitor == nullptr) { return nullptr; }
       return glfwGetVideoMode(monitor);
    }
    static i32 GetActiveMonitorWidth() {
       const GLFWvidmode* videoMode = GetActiveMonitorVideoMode();
       return videoMode ? videoMode->width : 0;
    }
    static i32 GetActiveMonitorHeight() {
       const GLFWvidmode* videoMode = GetActiveMonitorVideoMode();
       return videoMode ? videoMode->height : 0;
    }
    static i32 GetActiveMonitorRefreshRate() {
       const GLFWvidmode* videoMode = GetActiveMonitorVideoMode();
       return videoMode ? videoMode->refreshRate : 0;
    }
    static i32 GetActiveMonitorVideoModeCenterX(i32 const targetWidth) {
       return (GetActiveMonitorVideoMode()->width - targetWidth) / 2;
    }
    static i32 GetActiveMonitorVideoModeCenterY(i32 const targetHeight) {
       return (GetActiveMonitorVideoMode()->height - targetHeight) / 2;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Window & Render Management
    void OnUpdate()                   const;
    void ShowWindow()                 const { glfwShowWindow(m_Window); }
    void MakeContextCurrent()         const { glfwMakeContextCurrent(m_Window); }
    void SwapBuffers()                const { glfwSwapBuffers(m_Window); }
    void ToggleFullscreen();
    [[nodiscard]] bool IsFullscreen() const { return m_IsFullscreen; }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sizing & Viewport Callback Operations
    void OnResize(u32 const width, u32 const height) {
       m_Properties.Width = width;
       m_Properties.Height = height;
       m_FrameBufferResized = true;
    }
    void CheckViewportResize() {
       if (m_FrameBufferResized.exchange(false) == true) {
          glViewport(0, 0, m_Properties.Width, m_Properties.Height);
       }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Getters
    [[nodiscard]] bool ShouldClose()      const { return glfwWindowShouldClose(m_Window); }
    [[nodiscard]] GLFWwindow* GetWindow() const { return m_Window; }
    [[nodiscard]] i32 GetWindowWidth()    const { return m_Properties.Width; }
    [[nodiscard]] i32 GetWindowHeight()   const { return m_Properties.Height; }
    [[nodiscard]] f32 GetAspectRatio() {
       CheckViewportResize();
       const i32 height = GetWindowHeight();
       if (height == 0) { return 1.0f; }
       return static_cast<f32>(GetWindowWidth()) / static_cast<f32>(height);
    };
private:
    GLFWwindow* m_Window                  {nullptr};
    Properties m_Properties               {"", 0, 0, false, nullptr};
    std::atomic<bool> m_FrameBufferResized{false};
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Windowed & Fullscreen Properties
    bool m_IsFullscreen {false};
    i32 m_WindowedPosX  {0};
    i32 m_WindowedPosY  {0};
    i32 m_WindowedWidth {0};
    i32 m_WindowedHeight{0};
    bool CreateWindowInstance();
    void CenterOnPrimaryMonitor() const {
       i32 xPos = (GetActiveMonitorWidth() - GetWindowWidth()) / 2;
       i32 yPos = (GetActiveMonitorHeight() - GetWindowHeight()) / 2;
       glfwSetWindowPos(m_Window, xPos, yPos);
    }
};

}