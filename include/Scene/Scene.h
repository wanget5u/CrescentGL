#pragma once
#include "Core/Core.h"
#include "Core/Window.h"

namespace Crescent::Scene {

struct Scene {
	virtual ~Scene() = default;
	virtual void OnUpdate(f32 deltaTime) = 0;
	virtual void OnRender(Window& window) = 0;
};

}
