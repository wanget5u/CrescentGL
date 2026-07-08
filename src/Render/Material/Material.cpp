#include "Render/Material/Material.h"

#include "Asset/Type/Type.h"
#include "Asset/Registry.h"
#include "Render/Shader/Shader.h"

namespace Crescent::Render {

Material::Material(std::shared_ptr<Asset::Shader> shaderAsset, Math::Vector4 const& baseColor)
	: TintColor(baseColor), ShaderAsset(std::move(shaderAsset)) {}


std::shared_ptr<Asset::Shader> Material::GetDefaultShader() {
	static auto defaultShader = Asset::Registry::Instance().GetOrLoad<Asset::Shader>("Shaders/unlit", Asset::Type::Shader);
	return defaultShader;
}

std::shared_ptr<Material> Material::GetDefaultMaterial() {
	static auto defaultMaterial = std::make_shared<Material>(
		Asset::Registry::Instance().GetOrLoad<Asset::Shader>("Shaders/unlit", Asset::Type::Shader),
		GetDefaultColor()
	);
	return defaultMaterial;
}

Math::Vector4 Material::GetDefaultColor() noexcept {
	return Math::Vector4{ 0.7f, 0.6f, 0.9f, 1.0f };
}

std::shared_ptr<Asset::Shader> Material::GetActiveShader() const noexcept {
	if (ShaderAsset != nullptr && ShaderAsset->IsReady == true && ShaderAsset->ShaderObject != nullptr) {
		return ShaderAsset;
	}
	return GetDefaultShader();
}

void Material::Bind() const {
	std::shared_ptr<Asset::Shader> activeShader = GetActiveShader();
	if (activeShader == nullptr || activeShader->IsReady == false || activeShader->ShaderObject == nullptr) {
		return;
	}
	activeShader->ShaderObject->Use();
	activeShader->ShaderObject->SetVector4("u_TintColor", TintColor);
	BindAlbedoTexture(activeShader);
	BindMetallicTexture(activeShader);
	BindRoughnessTexture(activeShader);
	BindNormalTexture(activeShader);
}

void Material::SetMatrix4(const std::string_view name, Math::Matrix4x4 const& matrix) const {
	auto activeShader = GetActiveShader();
	if (activeShader && activeShader->IsReady && activeShader->ShaderObject) {
		activeShader->ShaderObject->SetMatrix4(name, matrix);
	}
}

void Material::SetVector4(const std::string_view name, Math::Vector4 const& vector) const {
	auto activeShader = GetActiveShader();
	if (activeShader && activeShader->IsReady && activeShader->ShaderObject) {
		activeShader->ShaderObject->SetVector4(name, vector);
	}
}

void Material::SetFloat(const std::string_view name, const f32 value) const {
	auto activeShader = GetActiveShader();
	if (activeShader && activeShader->IsReady && activeShader->ShaderObject) {
		activeShader->ShaderObject->SetFloat(name, value);
	}
}

u32 Material::Create1x1Texture(const u8 red, const u8 green, const u8 blue, const u8 alpha) {
	u32 textureID{};
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	u8 pixel[4] = { red, green, blue, alpha };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;
}

u32 Material::GetWhiteFallbackTexture() noexcept {
	static u32 textureID = Create1x1Texture(255, 255, 255);
	return textureID;
}

u32 Material::GetNormalFallbackTexture() noexcept {
	static u32 textureID = Create1x1Texture(128, 128, 128);
	return textureID;
}

void Material::BindAlbedoTexture(std::shared_ptr<Asset::Shader> const& activeShader) const noexcept {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, AlbedoTexture != nullptr ? AlbedoTexture->TextureID : GetWhiteFallbackTexture());
	activeShader->ShaderObject->SetInt("u_AlbedoMap", 0);
}

void Material::BindMetallicTexture(std::shared_ptr<Asset::Shader> const& activeShader) const noexcept {
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, MetallicTexture != nullptr ? MetallicTexture->TextureID : GetWhiteFallbackTexture());
	activeShader->ShaderObject->SetInt("u_MetallicMap", 1);
}

void Material::BindRoughnessTexture(std::shared_ptr<Asset::Shader> const& activeShader) const noexcept {
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, RoughnessTexture != nullptr ? RoughnessTexture->TextureID : GetWhiteFallbackTexture());
	activeShader->ShaderObject->SetInt("u_RoughnessMap", 2);
}

void Material::BindNormalTexture(std::shared_ptr<Asset::Shader> const& activeShader) const noexcept {
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, NormalTexture != nullptr ? NormalTexture->TextureID : GetNormalFallbackTexture());
	activeShader->ShaderObject->SetInt("u_NormalMap", 3);
}

}
