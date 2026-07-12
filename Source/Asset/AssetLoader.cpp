#include "Asset/AssetLoader.h"

#include <filesystem>
#include <glad/glad.h>

#include "Core/Log.h"
#include "Render/Mesh.h"
#include "Render/Shader/Shader.h"
#include "stb/stb_image.h"
#include "Util/FileOperations.h"

namespace Crescent {

bool AssetLoader::IsLoaded(const std::string filepath) const noexcept {
	return m_Assets.contains(std::string(filepath));
}

void AssetLoader::Clear() {
	m_Assets.clear();
}

void AssetLoader::Shutdown() {
	Clear();
}

AssetLoader::~AssetLoader() {
	Shutdown();
}

std::shared_ptr<Asset> AssetLoader::LoadAsset(const std::string filepath, const AssetType type) {
	if (type == AssetType::Shader) {
		return ResolveShaderAsset(filepath);
	}
	if (type == AssetType::Texture) {
		return ResolveTextureAsset(filepath);
	}
	if (type == AssetType::Mesh) {
		return ResolveMeshAsset(filepath);
	}
	return std::make_shared<Asset>();
}

std::shared_ptr<ShaderAsset> AssetLoader::ResolveShaderAsset(std::string filePath) {
	std::shared_ptr<ShaderAsset> shaderAsset = std::make_shared<ShaderAsset>();
	shaderAsset->FilePath = filePath;
	shaderAsset->Type = AssetType::Shader;
	std::string vertexSrc = Util::ReadFile(filePath + ".vert");
	std::string fragmentSrc = Util::ReadFile(filePath + ".frag");
	std::string geometrySrc{};
	if (std::filesystem::exists(Util::ResolveAssetPath(filePath + ".geom"))) {
		geometrySrc = Util::ReadFile(filePath + ".geom");
	}
	if (vertexSrc.empty() || fragmentSrc.empty()) {
		Log::Error("AssetLoader: Failed to load shader files for: {}", filePath);
		return shaderAsset;
	}
	shaderAsset->ShaderObject = std::make_shared<Shader>(vertexSrc, fragmentSrc, geometrySrc);
	shaderAsset->IsReady = true;
	Log::Info("AssetLoader: Loaded Shader '{}' into VRAM.", filePath);
	return shaderAsset;
}

std::shared_ptr<TextureAsset> AssetLoader::ResolveTextureAsset(std::string filepath) {
	std::shared_ptr<TextureAsset> textureAsset = std::make_shared<TextureAsset>();
	textureAsset->FilePath = filepath;
	textureAsset->Type = AssetType::Texture;
	std::string resolvedPath = Util::ResolveAssetPath(filepath);
	i32 width{0}, height{0}, channels{0};
	uchar8* pixels = stbi_load(resolvedPath.c_str(), &width, &height, &channels, 0);
	if (pixels == nullptr) {
		std::string altPath = "Assets/Textures/" + filepath;
		std::string resolvedAltPath = Util::ResolveAssetPath(altPath);
		pixels = stbi_load(resolvedAltPath.c_str(), &width, &height, &channels, 0);
		if (pixels != nullptr) {
			textureAsset->FilePath = altPath;
		}
	}
	if (pixels == nullptr) {
		Log::Error("AssetLoader: Failed to load texture file: {} ({})", filepath, stbi_failure_reason());
		return textureAsset;
	}
	glGenTextures(1, &textureAsset->TextureID);
	glBindTexture(GL_TEXTURE_2D, textureAsset->TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	u32 format{};
	switch (channels) {
		case 1:  format = GL_RED;  break;
		case 2:  format = GL_RG;   break;
		case 3:  format = GL_RGB;  break;
		case 4:  format = GL_RGBA; break;
		default: format = GL_RGB;
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(pixels);
	textureAsset->Width = width;
	textureAsset->Height = height;
	textureAsset->Channels = channels;
	textureAsset->IsReady = true;
	Log::Info("AssetLoader: Loaded Texture '{}' into VRAM (ID: {}).", filepath, textureAsset->TextureID);
	return textureAsset;
}

std::shared_ptr<MeshAsset> AssetLoader::ResolveMeshAsset(std::string filepath) {
	std::shared_ptr<MeshAsset> meshAsset = std::make_shared<MeshAsset>();
	meshAsset->FilePath = filepath;
	meshAsset->Type = AssetType::Mesh;
	return meshAsset;
}

}