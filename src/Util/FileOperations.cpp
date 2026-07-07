#include "Util/FileOperations.h"

#include <fstream>
#include <sstream>

#include "Core/Log.h"

namespace Crescent::Util {

std::string ReadFile(std::string_view filePath) {
	std::ifstream file(filePath.data());
	if (file.is_open() == false) {
		Log::Error("Failed to open file: ", filePath);
		return "";
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

}
