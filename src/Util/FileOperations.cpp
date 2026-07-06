#include "Util/FileOperations.h"
#include "Core/Log.h"
#include <fstream>
#include <sstream>

namespace Crescent::Util {

std::string ReadFile(char const* filePath) {
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
