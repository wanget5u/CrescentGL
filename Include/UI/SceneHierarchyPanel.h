#pragma once
#include "Panel.h"

namespace Crescent::UI {
struct SceneHierarchyPanel : Panel {
	explicit SceneHierarchyPanel(std::string_view name = "Hierarchy");
	void OnUpdate(f32 deltaTime) override;
	void OnRender() override;
	void OnAttach() override;
	void OnDetach() override;
};
}
