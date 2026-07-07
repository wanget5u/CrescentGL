#pragma once
#include <atomic>
#include <mutex>
#include <string>
#include <thread>

#include "Collections/Collections.h"
#include "Core/Core.h"
#include "Core/Window.h"

namespace Crescent {
// TODO: move this somewhere else, gotta make some Data presets for meshes, textures, shaders etc.
struct TextureLoadData {
	std::string FilePath;
	uchar8* Pixels{nullptr};
	i32 Width{0};
	i32 Height{0};
	i32 Channels{0};
};
/// Thread-Safe Singleton utility that offloads file I/O and texture data parsing to a dedicated loading thread.
struct AssetLoader {
	static AssetLoader& Instance() {
		static AssetLoader s_Instance;
		return s_Instance;
	}
	void OnCreate(Window* loadWindow);
	void Shutdown();
	AssetLoader() = default;
	~AssetLoader();
	AssetLoader(const AssetLoader&) = delete;
	AssetLoader& operator=(const AssetLoader&) = delete;
	void LoadTextureAsync(std::string const& filepath);
	[[nodiscard]] bool HasReadyTextures();
	bool PopReadyTextures(Collections::DynamicList<u32>& outTextures);
private:
	std::string PopNextFilePath();
	void LoadDataFromFilePath(std::string_view filePath);
	void LoadThreadLoop();
	Window* m_LoadWindow{nullptr};
	std::thread m_LoadThread{};
	std::atomic<bool> m_Running{false};
	std::mutex m_AssetMutex{};
	Collections::DynamicQueue<std::string> m_TextureLoadQueue{};
	Collections::DynamicList<TextureLoadData> m_ReadyTexturesWrite{};
	Collections::DynamicList<TextureLoadData> m_ReadyTexturesRead{};
};
}
