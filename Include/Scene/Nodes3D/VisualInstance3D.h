#pragma once
#include "Scene/Nodes3D/Node3D.h"

namespace Crescent {
struct VisualInstance3D : Node3D {
	friend struct BatchRenderer;
	VisualInstance3D() = default;
	~VisualInstance3D() override = default;
	void OnTreeEnter() override;
	void OnTreeExit() override;
	void SetLayerMask(u8 index) noexcept;
	[[nodiscard]] u8 GetLayerMask() const noexcept;
	template <typename... Args>
	void SetLayerMasks(Args... indices);
	void EnableLayer(u8 maskIndex);
	void DisableLayer(u8 maskIndex);
	bool IsOnLayer(u8 maskIndex) const;
protected:
	u8 m_LayerMask{1};
};
template<typename ... Args>
void VisualInstance3D::SetLayerMasks(Args... indices) {
	static_assert((std::is_integral_v<Args> && ...), "All layer indices must be integers.");
	m_LayerMask = ((1 << indices) | ...);
}
}
