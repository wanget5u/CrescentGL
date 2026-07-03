#pragma once
#include <fstream>
#include <sstream>
#include "Core/Log.h"
#include "Core/Core.h"

namespace Crescent::Util {

inline std::string ReadFile(char const* filePath) {
	std::ifstream file(filePath);

	if (file.is_open() == false) {
		Log::Error("Failed to open file: ", filePath);
		return "";
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

}