#pragma once
#include <string>

#include "stb/stb_image.h"

#include "Collection/Collections.h"
#include "Core/Core.h"
#include "Render/GPUDisposalQueue.h"
#include "Render/Mesh.h"
#include "Render/Shader/Shader.h"

namespace Crescent {
/// Here are defined all the Asset DTO's available for the `Loader.h` to load from disk
enum class AssetType : u8 {
	Shader, Texture, Mesh
};

struct Asset {
	AssetType Type{};
	std::string FilePath{};
	std::atomic<bool> IsReady{false};
	virtual ~Asset() = default;
};

struct ShaderAsset : Asset {
	std::shared_ptr<Shader> ShaderObject{nullptr};
	struct Data {
		std::string VertexSource;
		std::string FragmentSource;
		std::string GeometrySource;
	};
};

struct TextureAsset : Asset {
	~TextureAsset() override {
		if (TextureID != 0) {
			GPUDisposalQueue::SubmitTextureForDeletion(TextureID);
			TextureID = 0;
		}
	}
	u32 TextureID{0};
	i32 Width{0};
	i32 Height{0};
	i32 Channels{0};
	struct Data {
		std::string FilePath;
		uchar8* Pixels{nullptr};
		i32 Width{0};
		i32 Height{0};
		i32 Channels{0};
		~Data() {
			if (Pixels != nullptr) {
				stbi_image_free(Pixels);
				Pixels = nullptr;
			}
		}
		Data() = default;
		Data(const Data&) = delete;
		Data& operator=(const Data&) = delete;
		Data(Data&& other) noexcept {
			FilePath = std::move(other.FilePath);
			Pixels = other.Pixels;
			other.Pixels = nullptr;
			Width = other.Width;
			Height = other.Height;
			Channels = other.Channels;
		}
		Data& operator=(Data&& other) noexcept {
			if (this != &other) {
				if (Pixels != nullptr) {
					stbi_image_free(Pixels);
				}
				FilePath = std::move(other.FilePath);
				Pixels = other.Pixels;
				other.Pixels = nullptr;
				Width = other.Width;
				Height = other.Height;
				Channels = other.Channels;
			}
			return *this;
		}
	};
};

struct MeshAsset : Asset {
	std::shared_ptr<Mesh> MeshObject{nullptr};
	struct Data {
		DynamicList<Mesh::Vertex> Vertices{};
		DynamicList<u32> Indices{};
	};
};

}