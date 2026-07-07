#pragma once
#include <memory>

#include "Core/Core.h"
#include "Math/Vector/Vector4.h"

namespace Crescent::Render {
struct Shader;
struct Material {
	explicit Material(std::shared_ptr<Shader> shader, Math::Vector4 const& baseColor = GetDefaultColor());
	static std::shared_ptr<Material> GetDefaultMaterial();
	static Math::Vector4 GetDefaultColor() noexcept;
	[[nodiscard]] static u32 Create1x1Texture(u8 red, u8 green, u8 blue, u8 alpha = 255);
	void Bind() const;
	std::shared_ptr<Shader> Shader{nullptr};
	u32 AlbedoTextureID	   {0};
	Math::Vector4 TintColor{GetDefaultColor()};
	u32 MetallicTextureID  {0};
	u32 RoughnessTextureID {0};
	u32 NormalTextureID    {0};
private:
	static u32 GetWhiteFallbackTexture() noexcept;
	static u32 GetNormalFallbackTexture() noexcept;
	void BindAlbedoTexture() const noexcept;
	void BindMetallicTexture() const noexcept;
	void BindRoughnessTexture() const noexcept;
	void BindNormalTexture() const noexcept;
};
}
