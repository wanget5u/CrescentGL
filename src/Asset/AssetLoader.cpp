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

bool AssetLoader::PopReadyTexture(u32& outTextureID) {
	std::scoped_lock lock(m_AssetMutex);
	if (m_ReadyTextures.empty()) {
		return false;
	}
	outTextureID = m_ReadyTextures.front();
	m_ReadyTextures.pop();
	return true;
}

std::string AssetLoader::PopNextFilePath() {
	std::scoped_lock lock(m_AssetMutex);
	if (m_TextureLoadQueue.empty()) {
		return "";
	}
	std::string filePath = m_TextureLoadQueue.front();
	m_TextureLoadQueue.pop();
	return filePath;
}

void AssetLoader::LoadDataFromFilePath(std::string_view const filePath) {
	i32 width{}, height{}, nrChannels{};
	uchar8* data = stbi_load(filePath.data(), &width, &height, &nrChannels, 0);
	if (data == nullptr) {
		Log::Warning("Load Thread: Failed to load '", filePath, "'.");
		return;
	}
	u32 textureID = 0;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glFinish();
	stbi_image_free(data);
	{
		std::scoped_lock lock(m_AssetMutex);
		m_ReadyTextures.push(textureID);
	}
	Log::Info("Load Thread: Finished loading '", filePath, "' into Texture ID: {", textureID, "}.");
}

void AssetLoader::LoadThreadLoop() {
	m_LoadWindow->MakeContextCurrent();
	while (m_Running == true) {
		std::string filePath = PopNextFilePath();
		if (filePath.empty() == false) {
			Log::Info("Load Thread: Loading '", filePath, "'.");
			LoadDataFromFilePath(filePath);
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}
	}
	Window::UnbindContext();
}

}
