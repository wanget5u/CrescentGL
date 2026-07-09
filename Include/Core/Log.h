#pragma once
#include <iostream>
#include <utility>
#include <mutex>
#include <format>

namespace Crescent {
struct Log {
	static bool EnableANSI();

	template <typename... Args>
	static void Print(std::format_string<Args...> fmt, Args&&... args) {
		std::scoped_lock lock(s_LogMutex);
		std::cout << std::format(fmt, std::forward<Args>(args)...) << '\n';
	}
	template <typename... Args>
	static void Info(std::format_string<Args...> fmt, Args&&... args) {
		std::scoped_lock lock(s_LogMutex);
		static bool ansi = EnableANSI();
		static_cast<void>(ansi);
		std::cout << "\033[1;37m[OK]\033[0m " << std::format(fmt, std::forward<Args>(args)...) << '\n';
	}
	template <typename... Args>
	static void Warning(std::format_string<Args...> fmt, Args&&... args) {
		std::scoped_lock lock(s_LogMutex);
		static bool ansi = EnableANSI();
		static_cast<void>(ansi);
		std::cerr << "\033[1;33m[WARNING]\033[0m " << std::format(fmt, std::forward<Args>(args)...) << '\n';
	}
	template <typename... Args>
	static void Error(std::format_string<Args...> fmt, Args&&... args) {
		std::scoped_lock lock(s_LogMutex);
		static bool ansi = EnableANSI();
		static_cast<void>(ansi);
		std::cerr << "\033[1;31m[ERROR]\033[0m " << std::format(fmt, std::forward<Args>(args)...) << '\n';
	}
private:
	static std::mutex s_LogMutex;
};
}