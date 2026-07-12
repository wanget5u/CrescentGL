#pragma once
#include "Core/Core.h"
#include "Math/Matrix/Matrix4x4.h"

namespace Crescent {
	struct Mesh;
	struct Material;
}

namespace Crescent {
struct RenderPacket {
	u64 SortKey{0};
	f32 DistanceToCamera{0.0f};
	bool IsTransparent{false};
	Mesh* MeshObject{nullptr};
	Material* MaterialObject{nullptr};
	Math::Matrix4x4 WorldMatrix{};
};
}
