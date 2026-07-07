#include "Asset/AssetLoader.h"
#include <chrono>
#include <thread>
#include "Core/Log.h"
#include "Util/Util.h"

namespace Crescent {

void AssetLoader::OnCreate(Window* loadWindow) {
	if (m_Running == true) { return; }
	m_LoadWindow = loadWindow;
	m_Running = true;
	m_LoadThread = std::thread(&AssetLoader::LoadThreadLoop, this);
}

void AssetLoader::Shutdown() {
	if (m_Running == false) { return; }
	m_Running = false;
	if (m_LoadThread.joinable()) {
		m_LoadThread.join();
	}
}

AssetLoader::~AssetLoader() { Shutdown(); }

void AssetLoader::LoadTextureAsync(std::string const &filepath) {
	std::scoped_lock lock(m_AssetMutex);
	m_TextureLoadQueue.Push(filepath);
}

bool AssetLoader::HasReadyTextures() {
	std::scoped_lock lock(m_AssetMutex);
	return m_ReadyTexturesWrite.IsEmpty() == false;
}

bool AssetLoader::PopReadyTextures(Collections::DynamicList<u32>& outTextures) {
	{
		std::scoped_lock lock(m_AssetMutex);
		if (m_ReadyTexturesWrite.IsEmpty() == true) { return false; }
		std::swap(m_ReadyTexturesRead, m_ReadyTexturesWrite);
	}
	outTextures.Reserve(m_ReadyTexturesRead.GetSize());
	for (size_t a = 0; a < m_ReadyTexturesRead.GetSize(); ++a) {
		const TextureLoadData& data = m_ReadyTexturesRead[a];
		u32 textureID = 0;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, data.Width, data.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.Pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data.Pixels);
		outTextures.PushBack(textureID);
		Log::Info("Main Thread: Uploaded '{}' to VRAM. Texture ID: {}.", data.FilePath, textureID);
	}
	m_ReadyTexturesRead.Clear();
	return true;
}

std::string AssetLoader::PopNextFilePath() {
	std::scoped_lock lock(m_AssetMutex);
	if (m_TextureLoadQueue.IsEmpty() == true) {
		return "";
	}
	std::string filePath = m_TextureLoadQueue.Pop();
	return filePath;
}

void AssetLoader::LoadDataFromFilePath(std::string_view const filePath) {
	TextureLoadData texData;
	texData.FilePath = filePath;
	texData.Pixels = stbi_load(filePath.data(), &texData.Width, &texData.Height, &texData.Channels, 0);
	if (texData.Pixels == nullptr) {
		Log::Warning("Load Thread: Failed to load '", filePath, "'.");
		return;
	}
	{
		std::scoped_lock lock(m_AssetMutex);
		m_ReadyTexturesWrite.PushBack(texData);
	}
	Log::Info("Load Thread: Decoded '", filePath, "' into RAM.");
}

void AssetLoader::LoadThreadLoop() {
	std::string filePath{};
	while (m_Running == true) {
		filePath = PopNextFilePath();
		if (filePath.empty() == false) {
			Log::Info("Load Thread: Loading '", filePath, "'.");
			LoadDataFromFilePath(filePath);
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}
	}
}

}
