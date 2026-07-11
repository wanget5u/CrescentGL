#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <variant>

#include "Collection/Collections.h"
#include "AssetType.h"

namespace Crescent {
struct Window;
}
namespace Crescent::Asset {
struct LoadRequest {
	std::string FilePath;
	AssetType Type;
};
struct ReadyPacket {
	std::string FilePath;
	AssetType Type;
	std::variant<Shader::Data, Texture::Data, Mesh::Data> Data;
};
/// Thread-Safe Singleton utility that offloads file I/O and texture data parsing to a dedicated loading thread.
struct Loader {
	static Loader& Instance() {
		static Loader s_Instance;
		return s_Instance;
	}
	/// Providing a separate AssetLoader window we hook it up to the `m_LoadThread`
	void OnCreate(Window* loadWindow);
	/// Stops the loader from running and detaches itself from the `m_LoadThread`
	void Shutdown();
	Loader(const Loader&) = delete;
	Loader& operator=(const Loader&) = delete;
	/// Pushes the specified filepath and asset to the `m_AssetLoadQueue`
	void LoadAssetAsync(std::string const& filepath, AssetType type);
	/// Retrieves the ready asset from `m_ReadyAssetWrite`
	bool PopReadyAssets(DynamicList<ReadyPacket>& outAssets);
private:
	Loader() = default;
	~Loader();
	/// Retrieves the next LoadRequest from `m_AssetLoadQueue`
	LoadRequest PopNextRequest();
	/// Loads the data from the LoadRequest and pushes it into
	/// `m_ReadyAssetWrite` to be ready to use
	void LoadDataFromRequest(LoadRequest const& request);
	/// Core Loader update loop
	void LoadThreadLoop();
	Window* m_LoadWindow{nullptr};
	std::thread m_LoadThread{};
	std::atomic<bool> m_Running{false};
	std::mutex m_AssetMutex{};
	std::condition_variable m_ConditionVariable{};
	/// The queue that holds all Asset LoadRequests
	/// to be later resolved and loaded
	DynamicQueue<LoadRequest> m_AssetLoadQueue{};
	/// The list of ready asset packets available to use
	DynamicQueue<ReadyPacket> m_ReadyAssetQueue{};
};
}
