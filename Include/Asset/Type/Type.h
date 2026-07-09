#pragma once
#include <string>

#include "Collection/Collections.h"
#include "Core/Core.h"
#include "Render/Mesh.h"

namespace Crescent::Render {
	struct Shader;
	struct Texture;
	struct Mesh;
}

namespace Crescent::Asset {

enum class Type : u8 {
	Shader, Texture, Mesh
};

struct Asset {
	std::string FilePath;
	Type Type;
	std::atomic<bool> IsReady{false};
	virtual ~Asset() = default;
};

struct Shader : Asset {
	std::shared_ptr<Render::Shader> ShaderObject{nullptr};
	struct Data {
		std::string VertexSource;
		std::string FragmentSource;
	};
};

struct Texture : Asset {
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
	};
};

struct Mesh : Asset {
	std::shared_ptr<Render::Mesh> MeshObject{nullptr};
	struct Data {
		DynamicList<Render::Mesh::Vertex> Vertices{};
		DynamicList<u32> Indices{};
	};
};

}
