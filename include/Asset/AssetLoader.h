#pragma once
#include <atomic>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include "Core/Core.h"
#include "Core/Window.h"

namespace Crescent {
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
	bool PopReadyTexture(u32& outTextureID);
private:
	std::string PopNextFilePath();
	void LoadDataFromFilePath(std::string_view filePath);
	void LoadThreadLoop();
	Window* m_LoadWindow					  {nullptr};
	std::thread m_LoadThread				  {};
	std::atomic<bool> m_Running				  {false};
	std::mutex m_AssetMutex					  {};
	std::queue<std::string> m_TextureLoadQueue{};
	std::queue<u32> m_ReadyTextures			  {};
};
}
