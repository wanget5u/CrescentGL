#pragma once
#include <string>

#include "Core/Core.h"

namespace Crescent::UI {
struct Panel {
	explicit Panel(const std::string_view name) : m_Name(name) {}
	virtual ~Panel() = default;
	virtual void OnUpdate(f32 deltaTime) = 0;
	virtual void OnRender() = 0;
	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;
	[[nodiscard]] virtual bool IsVisible() const noexcept;
	virtual void SetVisibility(bool isVisible);
	[[nodiscard]] f32 GetResolutionScale() const noexcept;
	[[nodiscard]] f32 GetViewportWidth() const noexcept;
	[[nodiscard]] f32 GetViewportHeight() const noexcept;
	void BeginScaledFont() const;
	void EndScaledFont() const;
protected:
	bool m_IsVisible{true};
	std::string m_Name{};
};
}