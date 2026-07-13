#pragma once
#include "Node/Node3D/Geometry/GeometryInstance3D.h"

namespace Crescent {
struct InstancedVisual3D : GeometryInstance3D {
	friend struct BatchRenderer;
	static inline u32 s_InstancedVisualSSBO{0};
	static inline size_t s_InstancedVisualSSBOCapacity{0};
	InstancedVisual3D() = default;
	~InstancedVisual3D() override = default;
	void OnTreeEnter() override;
	void OnTreeExit() override;
	virtual void Draw() const = 0;
};
}
