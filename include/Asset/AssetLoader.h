#pragma once
#include <atomic>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include "Core/Core.h"
#include "Core/Window.h"

namespace Crescent {

struct AssetLoader {
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Lifecycle
	static AssetLoader& Instance() {
		static AssetLoader s_Instance;
		return s_Instance;
	}
	void OnCreate(Window* loadWindow);
	void Shutdown();
	AssetLoader() = default;
	~AssetLoader() { Shutdown(); }
	AssetLoader(const AssetLoader&) = delete;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	AssetLoader& operator=(const AssetLoader&) = delete;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Textures
	void LoadTextureAsync(std::string const& filepath) {
		std::scoped_lock lock(m_AssetMutex);
		m_TextureLoadQueue.push(filepath);
	}
	[[nodiscard]] bool HasReadyTextures() {
		std::scoped_lock lock(m_AssetMutex);
		return m_ReadyTextures.empty() == false;
	}
	bool PopReadyTexture(u32& outTextureID);
private:
	std::string PopNextFilePath();
	void LoadDataFromFilePath(std::string_view filePath);
	void LoadThreadLoop();
	Window* m_LoadWindow					  {nullptr};
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Core State Members
	std::thread m_LoadThread				  {};
	std::atomic<bool> m_Running				  {false};
	std::mutex m_AssetMutex					  {};
	std::queue<std::string> m_TextureLoadQueue{};
	std::queue<u32> m_ReadyTextures			  {};
};

}
