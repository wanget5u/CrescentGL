#include "Win32Utils.h"

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <windows.h>
#include <cmath>

#include "Core/Application.h"
#include "Core/Core.h"

namespace Crescent::Win32 {

static constexpr UINT_PTR RESIZE_TIMER_ID = 1001;
static const wchar_t* WNDPROC_PROP = L"Crescent_OriginalWndProc";

static LRESULT CALLBACK Win32SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    WNDPROC originalWndProc = reinterpret_cast<WNDPROC>(GetPropW(hwnd, WNDPROC_PROP));
    if (!originalWndProc) {
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    bool killTimerOnReturn = false;
    switch (uMsg) {
        case WM_NCLBUTTONDOWN: {
            if (wParam == HTCAPTION || (wParam >= HTLEFT && wParam <= HTBOTTOMRIGHT)) {
                POINT startPos;
                GetCursorPos(&startPos);
                const i32 dragThresholdX = GetSystemMetrics(SM_CXDRAG);
                const i32 dragThresholdY = GetSystemMetrics(SM_CYDRAG);
                const ULONGLONG startTime = GetTickCount64();
                const UINT dblClickTime = GetDoubleClickTime();
                while ((GetTickCount64() - startTime) < dblClickTime) {
                    MSG msg;
                    if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                        if (msg.message == WM_QUIT) {
                            PostQuitMessage(static_cast<i32>(msg.wParam));
                            return 0;
                        }
                        if (msg.message == WM_LBUTTONUP || msg.message == WM_NCLBUTTONUP) {
                            PostMessageW(msg.hwnd, msg.message, msg.wParam, msg.lParam);
                            break;
                        }
                        if (msg.message == WM_MOUSEMOVE || msg.message == WM_NCMOUSEMOVE) {
                            POINT currentPos;
                            GetCursorPos(&currentPos);
                            if (std::abs(currentPos.x - startPos.x) > dragThresholdX ||
                                std::abs(currentPos.y - startPos.y) > dragThresholdY) {
                                PostMessageW(msg.hwnd, msg.message, msg.wParam, msg.lParam);
                                break;
                            }
                        }
                        TranslateMessage(&msg);
                        DispatchMessageW(&msg);
                    } else {
                        Application::Instance().OnRender();
                    }
                }
            }
            break;
        }
        case WM_SYSCOMMAND: {
            UINT const cmd = static_cast<UINT>(wParam & 0xFFF0);
            if (cmd == SC_MOVE || cmd == SC_SIZE) {
                SetTimer(hwnd, RESIZE_TIMER_ID, 1, nullptr);
                killTimerOnReturn = true;
            }
            break;
        }
        case WM_ENTERSIZEMOVE: {
            SetTimer(hwnd, RESIZE_TIMER_ID, 1, nullptr);
            break;
        }
        case WM_EXITSIZEMOVE: {
            KillTimer(hwnd, RESIZE_TIMER_ID);
            break;
        }
        case WM_TIMER: {
            if (wParam == RESIZE_TIMER_ID) {
                Application::Instance().OnRender();
            }
            break;
        }
        case WM_SIZING:
        case WM_MOVING: {
            Application::Instance().OnRender();
            break;
        }
        default:
            break;
    }
    LRESULT const result = CallWindowProcW(originalWndProc, hwnd, uMsg, wParam, lParam);
    if (killTimerOnReturn) {
        KillTimer(hwnd, RESIZE_TIMER_ID);
    }
    return result;
}

void SubclassGLFWWindow(GLFWwindow* window) {
    if (window == nullptr) {
        return;
    }
    HWND hwnd = glfwGetWin32Window(window);
    if (!hwnd || GetPropW(hwnd, WNDPROC_PROP) != nullptr) {
        return;
    }
    WNDPROC originalWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(
        hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(Win32SubclassProc)
    ));
    SetPropW(hwnd, WNDPROC_PROP, reinterpret_cast<HANDLE>(originalWndProc));
}

void UnsubclassGLFWWindow(GLFWwindow* window) {
    if (window == nullptr) {
        return;
    }
    HWND hwnd = glfwGetWin32Window(window);
    if (hwnd == nullptr) {
        return;
    }
    WNDPROC originalWndProc = reinterpret_cast<WNDPROC>(GetPropW(hwnd, WNDPROC_PROP));
    if (originalWndProc != nullptr) {
        KillTimer(hwnd, RESIZE_TIMER_ID);
        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(originalWndProc));
        RemovePropW(hwnd, WNDPROC_PROP);
    }
}

bool EnableANSI() {
    HANDLE handleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE handleError = GetStdHandle(STD_ERROR_HANDLE);
    DWORD mode = 0;
    if (handleOutput != INVALID_HANDLE_VALUE && GetConsoleMode(handleOutput, &mode)) {
        SetConsoleMode(handleOutput, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    if (handleError != INVALID_HANDLE_VALUE && GetConsoleMode(handleError, &mode)) {
        SetConsoleMode(handleError, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    return true;
}

}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#include <cstdio>
#include <cstdarg>

extern "C" {
    int __cdecl __mingw_vsnprintf(char* buffer, size_t count, const char* format, va_list argptr) {
        return _vsnprintf(buffer, count, format, argptr);
    }
    int __cdecl __mingw_vsprintf(char* buffer, const char* format, va_list argptr) {
        return vsprintf(buffer, format, argptr);
    }
    int __cdecl __mingw_vsscanf(const char* buffer, const char* format, va_list argptr) {
        return vsscanf(buffer, format, argptr);
    }
}
#pragma warning(pop)
#endif

#endif