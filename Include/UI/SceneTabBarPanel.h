#pragma once
#include "Panel.h"

namespace Crescent::UI {
struct SceneTabBarPanel : Panel {
	explicit SceneTabBarPanel(std::string_view name = "Scene Tab Bar");
	void OnUpdate(f32 deltaTime) override;
	void OnRender() override;
	void OnAttach() override;
	void OnDetach() override;
};
}
