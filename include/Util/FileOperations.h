#pragma once
#include <fstream>
#include <sstream>
#include "Log.h"
#include "Core.h"

namespace Crescent::Util {

inline std::string ReadFile(char const* filePath) {
	std::ifstream file(filePath);

	if (file.is_open() == false) {
		Log::Error("Failed to open file: ", filePath);
		return nullptr;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

}