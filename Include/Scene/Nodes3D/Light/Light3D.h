#pragma once
#include "Scene/Nodes3D/VisualInstance3D.h"

namespace Crescent::Render {
	struct BatchRenderer;
}
namespace Crescent::Scene {
enum class LightType {
	Point, Directional, Spot
};
struct Light3D : VisualInstance3D {
	friend struct Render::BatchRenderer;
	constexpr static i32 MaxPointLightsPerDrawCall = 32;
	Light3D() = default;
	~Light3D() override = default;
	void OnTreeEnter() override;
	void OnTreeExit() override;
	virtual LightType GetLightType() const noexcept = 0;
	void SetIsOn(bool isOn) noexcept;
	[[nodiscard]] bool IsOn() const noexcept;
	void SetCastShadow(bool castShadow) noexcept;
	[[nodiscard]] bool IsCastingShadow() const noexcept;
	void SetEnergy(f32 energy) noexcept;
	[[nodiscard]] f32 GetEnergy() const noexcept;
	void SetColor(Math::Vector3 const& color) noexcept;
	[[nodiscard]] Math::Vector3 const& GetColor() const noexcept;
protected:
	bool m_IsOn{true};
	bool m_CastShadow{true};
	f32 m_Energy{1.0f};
	Math::Vector3 m_Color{1.0f, 1.0f, 1.0f};
};
}
