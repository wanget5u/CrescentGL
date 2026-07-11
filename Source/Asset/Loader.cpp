#include "Asset/Loader.h"

#include <chrono>
#include <filesystem>
#include <thread>

#include "Core/Log.h"
#include "Core/Window.h"
#include "Util/Util.h"

namespace Crescent {

void Asset::Loader::OnCreate(Window* loadWindow) {
	if (m_Running == true) {
		return;
	}
	m_LoadWindow = loadWindow;
	m_Running = true;
	m_LoadThread = std::thread(&Loader::LoadThreadLoop, this);
}

void Asset::Loader::Shutdown() {
	if (m_Running == false) {
		return;
	}
	{
		std::scoped_lock lock(m_AssetMutex);
		m_Running = false;
	}
	m_ConditionVariable.notify_all();
	if (m_LoadThread.joinable() == true) {
		m_LoadThread.join();
	}
}

Asset::Loader::~Loader() {
	Shutdown();
}

void Asset::Loader::LoadAssetAsync(std::string const& filepath, AssetType type) {
	{
		std::scoped_lock lock(m_AssetMutex);
		m_AssetLoadQueue.Push({filepath, type});
	}
	m_ConditionVariable.notify_one();
}

bool Asset::Loader::PopReadyAssets(DynamicList<ReadyPacket>& outAssets) {
	std::scoped_lock lock(m_AssetMutex);
	if (m_ReadyAssetQueue.IsEmpty() == true) {
		return false;
	}
	while (m_ReadyAssetQueue.IsEmpty() == false) {
		outAssets.PushBack(m_ReadyAssetQueue.Pop());
	}
	return true;
}

Asset::LoadRequest Asset::Loader::PopNextRequest() {
	std::scoped_lock lock(m_AssetMutex);
	if (m_AssetLoadQueue.IsEmpty()) {
		return {};
	}
	return m_AssetLoadQueue.Pop();
}

void Asset::Loader::LoadDataFromRequest(LoadRequest const& request) {
	ReadyPacket packet{};
	packet.FilePath = request.FilePath;
	packet.Type = request.Type;
	if (request.Type == AssetType::Shader) {
		Shader::Data shaderData{};
		shaderData.VertexSource = Util::ReadFile(request.FilePath + ".vert");
		shaderData.FragmentSource = Util::ReadFile(request.FilePath + ".frag");
		std::string geometryPath = request.FilePath + ".geom";
		if (std::filesystem::exists(Util::ResolveAssetPath(geometryPath))) {
			shaderData.GeometrySource = Util::ReadFile(geometryPath);
		}
		if (shaderData.VertexSource.empty() || shaderData.FragmentSource.empty()) {
			Log::Error("Failed to load shader files for: {}", request.FilePath);
			return;
		}
		packet.Data = std::move(shaderData);
	}
	else if (request.Type == AssetType::Texture) {
		Texture::Data textureData{};
		textureData.FilePath = request.FilePath;
		std::string resolvedPath = Util::ResolveAssetPath(textureData.FilePath);
		textureData.Pixels = stbi_load(
			resolvedPath.c_str(),
			&textureData.Width,
			&textureData.Height,
			&textureData.Channels,
			0
		);
		if (textureData.Pixels == nullptr) {
			std::string altPath = "Assets/Textures/" + textureData.FilePath;
			std::string resolvedAltPath = Util::ResolveAssetPath(altPath);
			textureData.Pixels = stbi_load(
				resolvedAltPath.c_str(),
				&textureData.Width,
				&textureData.Height,
				&textureData.Channels,
				0
			);
			if (textureData.Pixels != nullptr) {
				textureData.FilePath = altPath;
			}
		}
		if (textureData.Pixels == nullptr) {
			Log::Error("Failed to load texture file: {} ({})", request.FilePath, stbi_failure_reason());
			return;
		}
		packet.Data = std::move(textureData);
	}
	// TODO: Add obj, glTF, FBX support
	// else if (request.Type == Type::Mesh) {
	//
	// }
	{
		std::scoped_lock lock(m_AssetMutex);
		m_ReadyAssetQueue.Push(std::move(packet));
	}
}

void Asset::Loader::LoadThreadLoop() {
	while (m_Running == true) {
		LoadRequest request{};
		{
			std::unique_lock lock(m_AssetMutex);
			m_ConditionVariable.wait(lock, [this] { return m_AssetLoadQueue.IsEmpty() == false || m_Running == false; });
			if (m_Running == false && m_AssetLoadQueue.IsEmpty() == true) {
				break;
			}
			if (m_AssetLoadQueue.IsEmpty() == false) {
				request = m_AssetLoadQueue.Pop();
			}
		}
		if (request.FilePath.empty() == false) {
			Log::Info("Load Thread: Loading '{}'.", request.FilePath);
			LoadDataFromRequest(request);
		}
	}
}

}
