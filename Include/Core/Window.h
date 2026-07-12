#pragma once
#include <atomic>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/Core.h"

namespace Crescent {
/// Manages OS-level windows, monitor configurations, and the primary OpenGL context.
/// This class wraps a `GLFWwindow` instance, managing its lifecycle, display parameters,
/// atomic viewport sizing changes, and contextual resource binding.
struct Window {
    struct Properties {
       const char8* Title{""};
       u32 Width{0};
       u32 Height{0};
       bool Visible{false};
       explicit Properties(
          char8 const* title = "CrescentGL",
          u32 const width = SCREEN_WIDTH,
          u32 const height = SCREEN_HEIGHT,
          bool const visible = true)
          :
          Title(title),
          Width(width),
          Height(height),
          Visible(visible) {}
    };
    explicit Window(Properties const& properties = Properties());
    ~Window();
    static void UnbindContext();
    static void PollEvents();
    static GLFWmonitor* GetActiveMonitor();
    static const GLFWvidmode* GetActiveMonitorVideoMode();
    static i32 GetActiveMonitorWidth();
    static i32 GetActiveMonitorHeight();
    static i32 GetActiveMonitorRefreshRate();
    static i32 GetActiveMonitorVideoModeCenterX(i32 targetWidth);
    static i32 GetActiveMonitorVideoModeCenterY(i32 targetHeight);
    void OnUpdate() const;
    void ShowWindow() const;
    void MakeContextCurrent() const;
    void SwapBuffers() const;
    void ToggleFullscreen();
    [[nodiscard]] bool IsFullscreen() const;
    void OnResize(u32 width, u32 height);
    void OnRefresh();
    void CheckViewportResize();
    [[nodiscard]] bool ShouldClose() const;
    [[nodiscard]] GLFWwindow* GetWindow() const;
    [[nodiscard]] i32 GetWindowWidth() const;
    [[nodiscard]] i32 GetWindowHeight() const;
    [[nodiscard]] f32 GetAspectRatio() const;
private:
    GLFWwindow* m_Window {nullptr};
    Properties m_Properties {"", 0, 0, false};
    std::atomic<u32> m_Width {0};
    std::atomic<u32> m_Height {0};
    std::atomic<bool> m_FrameBufferResized{false};
    bool m_IsFullscreen {false};
	i32 m_WindowedPosX {0};
	i32 m_WindowedPosY {0};
	i32 m_WindowedWidth {0};
	i32 m_WindowedHeight {0};
	std::atomic<f32> m_LastAspectRatio{0.0f};
    bool CreateWindowInstance();
    void CenterOnPrimaryMonitor() const;
    void CalculateAspectRatio();
};

}