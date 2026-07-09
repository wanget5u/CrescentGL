#pragma once
#include <atomic>
#include <mutex>
#include <thread>
#include <variant>

#include "Collection/Collections.h"
#include "Type/Type.h"

namespace Crescent {
struct Window;
}

namespace Crescent::Asset {
struct LoadRequest {
	std::string FilePath;
	Type Type;
};
struct ReadyPacket {
	std::string FilePath;
	Type Type;
	std::variant<Shader::Data, Texture::Data, Mesh::Data> Data;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Thread-Safe Singleton utility that offloads file I/O and texture data parsing to a dedicated loading thread.
struct Loader {
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
	void LoadAssetAsync(std::string const& filepath, Type type);
	bool PopReadyAssets(DynamicList<ReadyPacket>& outAssets);
private:
	LoadRequest PopNextRequest();
	void LoadDataFromRequest(LoadRequest const& request);
	void LoadThreadLoop();
	Window* m_LoadWindow{nullptr};
	std::thread m_LoadThread{};
	std::atomic<bool> m_Running{false};
	std::mutex m_AssetMutex{};
	DynamicQueue<LoadRequest> m_AssetLoadQueue{};
	DynamicList<ReadyPacket> m_ReadyAssetWrite{};
	DynamicList<ReadyPacket> m_ReadyAssetRead{};
};
}
