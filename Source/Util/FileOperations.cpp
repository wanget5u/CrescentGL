#include "Util/FileOperations.h"

#include <filesystem>
#include <fstream>
#include <sstream>

#include "Core/Log.h"

namespace Crescent::Util {

std::string ResolveAssetPath(const std::string_view filePath) {
#ifdef CRESCENT_ASSET_ROOT
	std::filesystem::path rootPath = std::filesystem::path(CRESCENT_ASSET_ROOT) / filePath;
	if (std::filesystem::exists(rootPath)) {
		return rootPath.generic_string();
	}
#endif
	return std::string(filePath);
}

std::string ReadFile(std::string_view filePath) {
	std::string resolvedPath = ResolveAssetPath(filePath);
	std::ifstream file(resolvedPath);
	if (file.is_open() == false) {
		Log::Error("Failed to open file: ", resolvedPath);
		return "";
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

}
