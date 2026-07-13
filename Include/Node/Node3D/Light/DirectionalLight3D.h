#pragma once
#include "Light3D.h"

namespace Crescent {
struct DirectionalLight3D : Light3D {
	friend struct BatchRenderer;
	inline static u32 s_DirectionalLightSSBO{0};
	inline static size_t s_DirectionalLightSSBOCapacity{0};
	DirectionalLight3D() = default;
	~DirectionalLight3D() override = default;
	void OnTreeEnter() override;
	void OnTreeExit() override;
	[[nodiscard]] LightType GetLightType() const noexcept override;
};
}
