#pragma once
#include "Panel.h"

namespace Crescent::UI {
struct ToolbarPanel : Panel {
	explicit ToolbarPanel(std::string_view name = "Toolbar");
	void OnUpdate(f32 deltaTime) override;
	void OnRender() override;
	void OnAttach() override;
	void OnDetach() override;
};
}
