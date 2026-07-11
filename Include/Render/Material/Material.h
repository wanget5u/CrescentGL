#pragma once
#include <memory>

#include "Core/Core.h"
#include "Math/Matrix/Matrix4x4.h"
#include "Math/Vector/Vector4.h"
#include "Math/Vector/Vector3.h"
#include "Math/Vector/Vector2.h"

namespace Crescent::Asset {
	struct Shader;
	struct Texture;
}
namespace Crescent::Render {
struct Shader;
struct Material {
	explicit Material(std::shared_ptr<Asset::Shader> shaderAsset = nullptr, Math::Vector4 const& baseColor = GetDefaultColor());
	static std::shared_ptr<Asset::Shader> GetDefaultShader();
	static std::shared_ptr<Material> GetDefaultMaterial();
	static Math::Vector4 GetDefaultColor() noexcept;
	void Bind() const;
	void SetBool(std::string_view name, bool value) const;
	void SetInt(std::string_view name, i32 value) const;
	void SetFloat(std::string_view name, f32 value) const;
	void SetVector2(std::string_view name, Math::Vector2 const& vector) const;
	void SetVector3(std::string_view name, Math::Vector3 const& vector) const;
	void SetVector4(std::string_view name, Math::Vector4 const& vector) const;
	void SetMatrix4(std::string_view name, Math::Matrix4x4 const& matrix) const;
	Math::Vector4					TintColor		  {GetDefaultColor()};
	f32								MetallicFactor    {1.0f};
	f32								RoughnessFactor   {1.0f};
	std::shared_ptr<Asset::Shader>  ShaderAsset       {nullptr};
	std::shared_ptr<Asset::Texture> AlbedoTexture     {nullptr};
	std::shared_ptr<Asset::Texture> MetallicTexture   {nullptr};
	std::shared_ptr<Asset::Texture> RoughnessTexture  {nullptr};
	std::shared_ptr<Asset::Texture> NormalTexture     {nullptr};
private:
	[[nodiscard]] static u32 Create1x1Texture(u8 red, u8 green, u8 blue, u8 alpha = 255);
	[[nodiscard]] std::shared_ptr<Asset::Shader> GetActiveShader() const noexcept;
	static u32 GetWhiteFallbackTexture() noexcept;
	static u32 GetNormalFallbackTexture() noexcept;
	void BindAlbedoTexture(std::shared_ptr<Asset::Shader> const& activeShader) const noexcept;
	void BindMetallicTexture(std::shared_ptr<Asset::Shader> const& activeShader) const noexcept;
	void BindRoughnessTexture(std::shared_ptr<Asset::Shader> const& activeShader) const noexcept;
	void BindNormalTexture(std::shared_ptr<Asset::Shader> const& activeShader) const noexcept;
};
}
