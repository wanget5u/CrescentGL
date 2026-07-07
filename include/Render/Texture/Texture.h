#pragma once

#include "Asset/Type/Type.h"
#include "Core/Core.h"

namespace Crescent::Render {
struct Texture {
	u32 TextureID{0};
	i32 Width{0};
	i32 Height{0};
	i32 Channels{0};
};
}