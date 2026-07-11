#pragma once
#include "Core/Core.h"
#include "Math/Matrix/Matrix4x4.h"

namespace Crescent::Render {
	struct Mesh;
	struct Material;

	struct RenderPacket {
		u64 SortKey{0};
		Mesh* MeshObject{nullptr};
		Material* MaterialObject{nullptr};
		Math::Matrix4x4 WorldMatrix{};
	};
}
