#pragma once
#include "Light3D.h"

namespace Crescent::Render {
	struct BatchRenderer;
}

namespace Crescent::Scene {
struct PointLight3D : Light3D {
	friend struct Render::BatchRenderer;
	PointLight3D() = default;
	~PointLight3D() override = default;
	void OnTreeEnter() override;
	void OnTreeExit() override;
	[[nodiscard]] LightType GetLightType() const noexcept override;

	void SetRange(f32 range) noexcept;
	[[nodiscard]] f32 GetRange() const noexcept;

	void SetConstant(f32 constant) noexcept;
	[[nodiscard]] f32 GetConstant() const noexcept;

	void SetLinear(f32 linear) noexcept;
	[[nodiscard]] f32 GetLinear() const noexcept;

	void SetQuadratic(f32 quadratic) noexcept;
	[[nodiscard]] f32 GetQuadratic() const noexcept;

protected:
	f32 m_Range{0.0f};
	f32 m_Constant{1.0f};
	f32 m_Linear{0.7f};
	f32 m_Quadratic{1.8f};
};
}
