#include "../../Include/Asset/Registry.h"

#include <glad/glad.h>

#include "Render/Mesh.h"
#include "Render/Shader/Shader.h"
#include "Render/GPUDisposalQueue.h"
#include "stb/stb_image.h"

namespace Crescent::Asset {

void Registry::OnUpdate() {
	DynamicList<ReadyPacket> readyPackets{};
	if (Loader::Instance().PopReadyAssets(readyPackets) == false) {
		return;
	}
	readyPackets.Reserve(readyPackets.GetSize());
	for (size_t a = 0; a < readyPackets.GetSize(); ++a) {
		ReadyPacket& packet = readyPackets[a];
		std::unordered_map<std::string, std::shared_ptr<Asset>>::iterator it
			= m_Registry.find(packet.FilePath);
		if (it == m_Registry.end()) {
			continue;
		}
		std::shared_ptr<Asset>& baseAsset = it->second;
		if (packet.Type == AssetType::Shader) {
			std::shared_ptr<Shader> shaderAsset = std::static_pointer_cast<Shader>(baseAsset);
			Shader::Data& shaderData = std::get<Shader::Data>(packet.Data);
			shaderAsset->ShaderObject = std::make_shared<Render::Shader>(
				shaderData.VertexSource, shaderData.FragmentSource, shaderData.GeometrySource
			);
			shaderAsset->IsReady = true;
			Log::Info("Registry: Uploaded Shader '{}' to VRAM.", packet.FilePath);
		}
		else if (packet.Type == AssetType::Texture) {
			std::shared_ptr<Texture> textureAsset = std::static_pointer_cast<Texture>(baseAsset);
			Texture::Data& textureData = std::get<Texture::Data>(packet.Data);
			if (textureData.Pixels == nullptr) {
				Log::Error("Registry: Cannot upload null or invalid texture data for '{}'", packet.FilePath);
				continue;
			}
			glGenTextures(1, &textureAsset->TextureID);
			glBindTexture(GL_TEXTURE_2D, textureAsset->TextureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			u32 format = GL_RGB;
			if (textureData.Channels == 1) {
				format = GL_RED;
			}
			else if (textureData.Channels == 2) {
				format = GL_RG;
			}
			else if (textureData.Channels == 3) {
				format = GL_RGB;
			}
			else if (textureData.Channels == 4) {
				format = GL_RGBA;
			}
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				format,
				textureData.Width,
				textureData.Height,
				0,
				format,
				GL_UNSIGNED_BYTE,
				textureData.Pixels
			);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(textureData.Pixels);
			textureAsset->Width = textureData.Width;
			textureAsset->Height = textureData.Height;
			textureAsset->Channels = textureData.Channels;
			textureAsset->IsReady = true;
			Log::Info("Registry: Uploaded Texture '{}' to VRAM (ID: {}).", packet.FilePath, textureAsset->TextureID);
		}
		else if (packet.Type == AssetType::Mesh) {
			std::shared_ptr<Mesh> meshAsset = std::static_pointer_cast<Mesh>(baseAsset);
			Mesh::Data& meshData = std::get<Mesh::Data>(packet.Data);
			if (meshData.Vertices.GetSize() > 0 && meshData.Indices.GetSize() > 0) {
				meshAsset->MeshObject = std::make_shared<Render::Mesh>(
					reinterpret_cast<const f32*>(meshData.Vertices.GetData()),
					static_cast<u32>(meshData.Vertices.GetSizeInBytes()),
					meshData.Indices.GetData(),
					static_cast<u32>(meshData.Indices.GetSize()),
					Render::Mesh::VertexLayout::CreatePosNormalUV()
				);
				meshAsset->IsReady = true;
				Log::Info("Registry: Uploaded Mesh '{}' to VRAM (VAO: {}, VBO: {}, EBO: {}).",
					packet.FilePath, meshAsset->MeshObject->GetVAO(),
					meshAsset->MeshObject->GetVBO(), meshAsset->MeshObject->GetEBO()
				);
			} else {
				Log::Error("Registry: Mesh data empty for '{}'", packet.FilePath);
			}
		}
	}
}

void Registry::Clear() {
	m_Registry.clear();
}

Texture::~Texture() {
	if (TextureID != 0) {
		Render::GPUDisposalQueue::SubmitTextureForDeletion(TextureID);
		TextureID = 0;
	}
}

}
