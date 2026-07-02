#pragma once
#include <iostream>
#include <utility>
#include <cassert>

class Log {
public:
	// prints to the default output
	template <typename... Args>
	static void Print(Args&&... args) {
		std::cout << "\033[1;37m[OK]\033[0m ";
		(std::cout << ... << std::forward<Args>(args)) << '\n';
	}
	template <typename... Args>
	static void Warning(Args&&... args) {
		std::cerr << "\033[1;33m[WARNING]\033[0m ";
		(std::cerr << ... << std::forward<Args>(args)) << '\n';
	}
	template <typename... Args>
	static void Error(Args&&... args) {
		std::cerr << "\033[1;31m[ERROR]\033[0m ";
		(std::cerr << ... << std::forward<Args>(args)) << '\n';
	}
};