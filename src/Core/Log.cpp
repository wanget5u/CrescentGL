#include "Core/Log.h"

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

namespace Crescent {
bool Log::EnableANSI() {
#ifdef _WIN32
    HANDLE handleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE handleError = GetStdHandle(STD_ERROR_HANDLE);
    DWORD mode = 0;
    if (handleOutput != INVALID_HANDLE_VALUE && GetConsoleMode(handleOutput, &mode)) {
        SetConsoleMode(handleOutput, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    if (handleError != INVALID_HANDLE_VALUE && GetConsoleMode(handleError, &mode)) {
        SetConsoleMode(handleError, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif
    return true;
}
std::mutex Log::s_LogMutex;
}
