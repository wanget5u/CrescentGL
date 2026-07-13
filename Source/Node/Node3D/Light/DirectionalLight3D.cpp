#include "Node/Node3D/Light/DirectionalLight3D.h"

namespace Crescent {

void DirectionalLight3D::OnTreeEnter() {
	Light3D::OnTreeEnter();
}

void DirectionalLight3D::OnTreeExit() {
	Light3D::OnTreeExit();
}

LightType DirectionalLight3D::GetLightType() const noexcept {
	return LightType::Directional;
}

}
