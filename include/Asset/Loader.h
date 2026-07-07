#pragma once
#include <atomic>
#include <mutex>
#include <string>
#include <thread>

#include "Collection/Collections.h"
#include "Core/Core.h"
#include "Core/Window.h"
#include "DataTypes/DataType.h"

namespace Crescent::Asset {
/// Thread-Safe Singleton utility that offloads file I/O and texture data parsing to a dedicated loading thread.
struct Loader {
	struct Request {
		std::string FilePath;
	};
	static Loader& Instance() {
		static Loader s_Instance;
		return s_Instance;
	}
	void OnCreate(Window* loadWindow);
	void Shutdown();
	Loader() = default;
	~Loader();
	Loader(const Loader&) = delete;
	Loader& operator=(const Loader&) = delete;
	void LoadTextureAsync(std::string const& filepath);
	[[nodiscard]] bool HasReadyTextures();
	bool PopReadyTextures(DynamicList<u32>& outTextures);
private:
	std::string PopNextFilePath();
	void LoadDataFromFilePath(std::string_view filePath);
	void LoadThreadLoop();
	Window* m_LoadWindow{nullptr};
	std::thread m_LoadThread{};
	std::atomic<bool> m_Running{false};
	std::mutex m_AssetMutex{};
	DynamicQueue<std::string> m_TextureLoadQueue{};
	DynamicList<TextureData> m_ReadyTexturesWrite{};
	DynamicList<TextureData> m_ReadyTexturesRead{};
};
}
