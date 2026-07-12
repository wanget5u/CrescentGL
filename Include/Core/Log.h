#pragma once
#include <iostream>
#include <utility>
#include <format>

#include "Win32Utils.h"

namespace Crescent {
struct Log {
	template <typename... Args>
	static void Print(std::format_string<Args...> fmt, Args&&... args) {
		std::cout << std::format(fmt, std::forward<Args>(args)...) << '\n';
	}
	template <typename... Args>
	static void Info(std::format_string<Args...> fmt, Args&&... args) {
		static bool ansi = Win32::EnableANSI();
		static_cast<void>(ansi);
		std::cout << "\033[1;37m[OK]\033[0m " << std::format(fmt, std::forward<Args>(args)...) << '\n';
	}
	template <typename... Args>
	static void Warning(std::format_string<Args...> fmt, Args&&... args) {
		static bool ansi = Win32::EnableANSI();
		static_cast<void>(ansi);
		std::cerr << "\033[1;33m[WARNING]\033[0m " << std::format(fmt, std::forward<Args>(args)...) << '\n';
	}
	template <typename... Args>
	static void Error(std::format_string<Args...> fmt, Args&&... args) {
		static bool ansi = Win32::EnableANSI();
		static_cast<void>(ansi);
		std::cerr << "\033[1;31m[ERROR]\033[0m " << std::format(fmt, std::forward<Args>(args)...) << '\n';
	}
};
}
