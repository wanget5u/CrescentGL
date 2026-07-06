#pragma once
#include <atomic>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "Core/Core.h"

namespace Crescent {
/// Manages OS-level windows, monitor configurations, and the primary OpenGL context.
/// This class wraps a `GLFWwindow` instance, managing its lifecycle, display parameters,
/// atomic viewport sizing changes, and contextual resource binding.
struct Window {
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Configuration setup used to define window properties at creation.
    struct Properties {
       const char8* Title {""};       /// App title visible in the OS title bar
       u32 Width          {0};        /// Width of the framebuffer region
       u32 Height         {0};        /// Height of the framebuffer region
       bool Visible       {false};    /// Should the window render to the desktop immediately
       Window* ShareWindow{nullptr};  /// Explicit window context pointer used for asynchronous resource sharing
       ////////////////////////////////////////////////////////////////////////////////////////////////////
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
    /// Constructs and registers a window framework instance with GLFW
    explicit Window(Properties const& properties = Properties());
    ~Window() { glfwDestroyWindow(m_Window); }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Detaches any OpenGL rendering context assigned to the current calling thread
    static void UnbindContext() { glfwMakeContextCurrent(nullptr); }
    /// Processes pending OS events (input, window movement, focus lost) inside the main thread loop
    static void PollEvents() { glfwPollEvents(); }
    /// Resolves and returns the system monitor currently in use
    static GLFWmonitor* GetActiveMonitor();
    /// Extracts resolution and refresh rates from the active monitor
    static const GLFWvidmode* GetActiveMonitorVideoMode();
    static i32 GetActiveMonitorWidth();
    static i32 GetActiveMonitorHeight();
    static i32 GetActiveMonitorRefreshRate();
    /// Calculates the horizontal offset to position a custom width dead-center on screen
    static i32 GetActiveMonitorVideoModeCenterX(i32 targetWidth);
    /// Calculates the vertical offset to position a custom height dead-center on screen.
    static i32 GetActiveMonitorVideoModeCenterY(i32 targetHeight);
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Per-frame tracking updates for the desktop container
    void OnUpdate() const;
    /// Sets the visibility flag of the window frame to active
    void ShowWindow() const;
    /// Binds this window's specific OpenGL context to the current thread
    void MakeContextCurrent() const;
    /// Swaps the front and back display color buffers to render finished graphics to the screen
    void SwapBuffers() const;
    /// Flips between dedicated borderless fullscreen and desktop windowed modes
    void ToggleFullscreen();
    [[nodiscard]] bool IsFullscreen() const;
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Callback invoked by OS resize hooks to cache target dimensions.
    void OnResize(u32 width, u32 height);
	/// Evaluates atomic modifications and updates glViewport safely inside the render loop thread
    void CheckViewportResize();
    ////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Checks if the OS or user has initiated a request to quit the window
    [[nodiscard]] bool ShouldClose()      const { return glfwWindowShouldClose(m_Window); }
    [[nodiscard]] GLFWwindow* GetWindow() const { return m_Window; }
    [[nodiscard]] i32 GetWindowWidth()    const { return m_Properties.Width; }
    [[nodiscard]] i32 GetWindowHeight()   const { return m_Properties.Height; }
	/// Computes the calculated aspect ratio (Width / Height) needed by projection matrices
    [[nodiscard]] f32 GetAspectRatio();
private:
    GLFWwindow* m_Window                  {nullptr}; /// Handle pointing directly to the internal GLFW struct instance
    Properties m_Properties               {"", 0, 0, false, nullptr};
    std::atomic<bool> m_FrameBufferResized{false};
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Windowed & Fullscreen Properties
    bool m_IsFullscreen {false};
	i32 m_WindowedPosX  {0}; /// Cached window X coordinates preserved when expanding to fullscreen
	i32 m_WindowedPosY  {0}; /// Cached window Y coordinates preserved when expanding to fullscreen
	i32 m_WindowedWidth {0}; /// Cached window Width dimensions preserved when expanding to fullscreen
	i32 m_WindowedHeight{0}; /// Cached window Height dimensions preserved when expanding to fullscreen
    bool CreateWindowInstance();
	/// Snaps the window borders to align center with the main display monitor
    void CenterOnPrimaryMonitor() const;
};

}