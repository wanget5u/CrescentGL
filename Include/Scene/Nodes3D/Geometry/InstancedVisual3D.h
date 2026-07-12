#pragma once
#include "Scene/Nodes3D/Geometry/GeometryInstance3D.h"

namespace Crescent {
struct InstancedVisual3D : GeometryInstance3D {
	InstancedVisual3D() = default;
	~InstancedVisual3D() override = default;
	void OnTreeEnter() override;
	void OnTreeExit() override;
	virtual void Draw() const = 0;
};
}
