#pragma once
#include <string>

#include "Collection/Collections.h"
#include "Core/Core.h"
#include "Render/Mesh.h"

namespace Crescent::Asset {

enum class Type : u8 {
	Shader, Texture, Mesh, Material
};

struct ShaderData {
	std::string VertexSource;
	std::string FragmentSource;
};

struct TextureData {
	std::string FilePath;
	uchar8* Pixels{nullptr};
	i32 Width{0};
	i32 Height{0};
	i32 Channels{0};
};

struct MeshData {
	DynamicList<Render::Mesh::Vertex> Vertices{};
	DynamicList<u32> Indices{};
};

}
