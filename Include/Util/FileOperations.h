#pragma once
#include <string>

#include <string_view>

namespace Crescent::Util {
std::string ResolveAssetPath(std::string_view filePath);
std::string ReadFile(std::string_view filePath);
}