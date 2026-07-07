#include "../../include/Asset/Registry.h"

#include "Render/Shader/Shader.h"
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
		auto it = m_Registry.find(packet.FilePath);
		if (it == m_Registry.end()) {
			continue;
		}
		std::shared_ptr<Asset>& baseAsset = it->second;
		if (packet.Type == Type::Shader) {
			std::shared_ptr<Shader> shaderAsset = std::static_pointer_cast<Shader>(baseAsset);
			auto& shaderData = std::get<Shader::Data>(packet.Data);
			shaderAsset->ShaderObject = std::make_shared<Render::Shader>(
				shaderData.VertexSource, shaderData.FragmentSource
			);
			shaderAsset->IsReady = true;
			Log::Info("Registry: Uploaded Shader '{}' to VRAM.", packet.FilePath);
		}
		else if (packet.Type == Type::Texture) {
			std::shared_ptr<Texture> textureAsset = std::static_pointer_cast<Texture>(baseAsset);
			auto& textureData = std::get<Texture::Data>(packet.Data);
			glGenTextures(1, &textureAsset->TextureID);
			glBindTexture(GL_TEXTURE_2D, textureAsset->TextureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGB,
				textureData.Width,
				textureData.Height,
				0,
				GL_RGB,
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
		// TODO: Add obj, glTF, FBX support
		// else if (packet.Type == Type::Mesh) {
		//
		// }
	}
}

}
