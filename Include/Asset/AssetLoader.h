#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "AssetType.h"

namespace Crescent {
struct AssetLoader {
	static AssetLoader& Instance() {
		static AssetLoader s_Instance;
		return s_Instance;
	}
	AssetLoader(const AssetLoader&) = delete;
	AssetLoader& operator=(const AssetLoader&) = delete;
	template <typename T>
	std::shared_ptr<T> GetOrLoad(std::string filepath, AssetType type);
	[[nodiscard]] bool IsLoaded(std::string filepath) const noexcept;
	void Clear();
	void Shutdown();
private:
	AssetLoader() = default;
	~AssetLoader();
	std::shared_ptr<Asset> LoadAsset(std::string filepath, AssetType type);
	std::shared_ptr<ShaderAsset> ResolveShaderAsset(std::string filePath);
	std::shared_ptr<TextureAsset> ResolveTextureAsset(std::string filepath);
	std::shared_ptr<MeshAsset> ResolveMeshAsset(std::string filepath);
	std::unordered_map<std::string, std::shared_ptr<Asset>> m_Assets{};
};
template <typename T>
std::shared_ptr<T> AssetLoader::GetOrLoad(const std::string filepath, const AssetType type) {
	auto it = m_Assets.find(filepath);
	if (it != m_Assets.end()) {
		return std::static_pointer_cast<T>(it->second);
	}
	std::shared_ptr<Asset> asset = LoadAsset(filepath, type);
	m_Assets[filepath] = asset;
	return std::static_pointer_cast<T>(asset);
}
}