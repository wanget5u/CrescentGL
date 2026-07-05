#pragma once
#include <iostream>
#include <utility>
#include <cassert>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

class Log {
public:
    static bool EnableANSI() {
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        HANDLE hErr = GetStdHandle(STD_ERROR_HANDLE);
        DWORD mode = 0;
        if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &mode))
            SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        if (hErr != INVALID_HANDLE_VALUE && GetConsoleMode(hErr, &mode))
            SetConsoleMode(hErr, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
        return true;
    }

	template <typename... Args>
	static void Print(Args&&... args) {
    	(std::cout << ... << std::forward<Args>(args)) << '\n';
    }
	template <typename... Args>
	static void Info(Args&&... args) {
        static bool ansi = EnableANSI();
        static_cast<void>(ansi);
		std::cout << "\033[1;37m[OK]\033[0m ";
		(std::cout << ... << std::forward<Args>(args)) << '\n';
	}
	template <typename... Args>
	static void Warning(Args&&... args) {
        static bool ansi = EnableANSI();
        static_cast<void>(ansi);
		std::cerr << "\033[1;33m[WARNING]\033[0m ";
		(std::cerr << ... << std::forward<Args>(args)) << '\n';
	}
	template <typename... Args>
	static void Error(Args&&... args) {
        static bool ansi = EnableANSI();
        static_cast<void>(ansi);
		std::cerr << "\033[1;31m[ERROR]\033[0m ";
		(std::cerr << ... << std::forward<Args>(args)) << '\n';
	}
};
